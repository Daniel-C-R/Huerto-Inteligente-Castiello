#include <WiFi.h>
#include <Wire.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

// Establecer ID del microcontrolador
#define CONTROLLER_ID 1

// Configuración y calibración del higrómetro capacitivo
#define SOIL_MOISTURE_PIN A0    // Pin del que se leen los datos
#define DRY_SOIL_MOISTURE 2364  // Valor que se considera 0% de humedad
#define WET_SOIL_MOISTURE 1289  // Valor que se considera 100% de humedad

// Definir el brillo máximo del LED
#define RGB_BRIGHTNESS 255

// Configuración de la red WiFi
#define SSID "CAMBIAR"
#define PASS "CAMBIAR"

// Configuración del broker de MQTT
#define MQTT_SERVER "CAMBIAR"
#define MQTT_PORT 1883

// Definir los pines I2C para el BME280
#define I2C_SCL 20
#define I2C_SDA 21

// Temas en los que publicar
const char* topics[] = {
  "huerto/temperature",
  "huerto/pressure",
  "huerto/altitude",
  "huerto/humidity",
  "huerto/soil_moisture"
};

// Objeto BME280
Adafruit_BME280 bme;

// Cliente WiFi y MQTT
WiFiClient espClient;
PubSubClient client(espClient);

// Conexión a WiFi
void setupWiFi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando a WiFi...");

  WiFi.begin(SSID, PASS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("\n");
  Serial.print("Conectado correctamente a la red WiFi: ");
  Serial.print(SSID);
  Serial.print("\n");
}

// Conexión al broker de MQTT
void connectMQTT() {
  // Hay que poner un identificador de conexión único a cada microcontrolador
  // para que el broker los distinga
  String clientId = "ESP32Client-";
  clientId += CONTROLLER_ID;
  
  Serial.println("Conectando al broker de MQTT...");

  while (!client.connected()) {
    if (client.connect(clientId.c_str())) {
      Serial.print("\nConectado al broker MQTT con dirección: ");
      Serial.println(MQTT_SERVER);
    } else {
      Serial.print("Falló la conexión. Código de error: ");
      Serial.print(client.state());
      Serial.print(" Reintentando en 5 segundos\n");
      delay(5000);
    }
  }
}

// Función para construir y publicar datos JSON
void publishData(const char* topic, float value, const char* key) {
  StaticJsonDocument<200> jsonDoc;
  jsonDoc["controller_id"] = CONTROLLER_ID;
  jsonDoc[key] = value;

  char jsonBuffer[200];
  serializeJson(jsonDoc, jsonBuffer);

  client.publish(topic, jsonBuffer);
  Serial.print("Publicado en ");
  Serial.print(topic);
  Serial.print(": ");
  Serial.println(jsonBuffer);
}

void setup() {
  // La luz blanca indica que ha comenzado el proceso de setup
  rgbLedWrite(RGB_BUILTIN, RGB_BRIGHTNESS, RGB_BRIGHTNESS, RGB_BRIGHTNESS);

  Serial.begin(115200);
  Serial.println("Iniciando el proceso de configuración.");
  Serial.print("Microcontrolador con ID: ");
  Serial.println(CONTROLLER_ID);

  // Conectar WiFi
  setupWiFi();

  // Configurar broker MQTT
  client.setServer(MQTT_SERVER, MQTT_PORT);
  connectMQTT();

  // La luz azul oscuro indica que la conexión al broker ha sido
  rgbLedWrite(RGB_BUILTIN, 0, 0, RGB_BRIGHTNESS);

  // Inicializar el sensor BME280
  Wire.begin(I2C_SDA, I2C_SCL);
  if (!bme.begin(0x76)) {
    Serial.println("No se pudo encontrar un sensor BME280 válido, ¡verifica el cableado!");
    // Indicamos en magenta que el BME280 no está bien conectado
    rgbLedWrite(RGB_BUILTIN, RGB_BRIGHTNESS, 0, RGB_BRIGHTNESS);
    while (1)
      ;
  }

  // Configuración del pin de humedad del suelo
  pinMode(SOIL_MOISTURE_PIN, INPUT);
}

void loop() {
  if (!client.connected()) {
    connectMQTT();
  }
  client.loop();

  // Publicar datos del sensor cada 5 segundos
  static unsigned long lastMsg = 0;
  unsigned long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;

    // Leer datos del BME280
    float temperature = bme.readTemperature();
    float pressure = bme.readPressure() / 100.0F;  // Conversión a hPa
    float altitude = bme.readAltitude(1013.25);    // Ajusta según la presión local
    float humidity = bme.readHumidity();

    // Leer datos de humedad del suelo
    int analogValue = analogRead(SOIL_MOISTURE_PIN);
    float soilMoisture = map(analogValue, DRY_SOIL_MOISTURE, WET_SOIL_MOISTURE, 0, 100);
    soilMoisture = constrain(soilMoisture, 0, 100);  // Limitar a valores entre 0 y 100%

    // Publicar datos en formato JSON
    publishData("huerto/temperature", temperature, "temperature");
    publishData("huerto/pressure", pressure, "pressure");
    publishData("huerto/altitude", altitude, "altitude");
    publishData("huerto/humidity", humidity, "humidity");
    publishData("huerto/soil_moisture", soilMoisture, "soil_moisture");

    // Decisión de riego
    if (soilMoisture < 70 || (temperature > 25 && soilMoisture < 85)) {
      rgbLedWrite(RGB_BUILTIN, RGB_BRIGHTNESS, 0, 0);
    } else {
      rgbLedWrite(RGB_BUILTIN, 0, RGB_BRIGHTNESS, 0);
    }
  }
}