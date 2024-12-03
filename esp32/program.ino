#include <WiFi.h>
#include <Wire.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

// Definir los pines I2C
#define I2C_SCL 20
#define I2C_SDA 21

// Configuración de la red WiFi
const char* ssid = "--";
const char* password = "--";

// Configuración del broker MQTT
const char* mqtt_server = "--"; // IP del broker MQTT
const int mqtt_port = 1883;               // Puerto del broker MQTT

// Temas para publicar
const char* topics[] = {
  "huerto/temperature",
  "huerto/pressure",
  "huerto/altitude",
  "huerto/humidity"
};

// Configuración de la hora para España
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600; // UTC+1 para España peninsular
const int daylightOffset_sec = 3600; // UTC+2 en horario de verano

// Configuración del sensor BME280
Adafruit_BME280 bme;

// Cliente WiFi y MQTT
WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando a WiFi...");
  
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Conectado a WiFi");
}

// Obtener timestamp en formato "YYYY-MM-DD HH:MM:SS"
String getTimestamp() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Error obteniendo la hora.");
    return "1970-01-01 00:00:00"; // Valor por defecto si no hay tiempo válido
  }
  char buffer[20];
  strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);
  return String(buffer);
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Intentando conectar al broker MQTT...");
    if (client.connect("ESP32Client")) {
      Serial.println(" Conectado");
      for (const char* topic : topics) {
        client.subscribe(topic);
        Serial.println(String("Suscrito al tema: ") + topic);
      }
    } else {
      Serial.print("Error: ");
      Serial.print(client.state());
      Serial.println(" Reintentando en 5 segundos");
      delay(5000);
    }
  }
}

// Función para construir y publicar datos JSON
void publishData(const char* topic, float value, const char* key) {
  StaticJsonDocument<200> jsonDoc;
  jsonDoc["controller_id"] = 1;
  jsonDoc["timestamp"] = getTimestamp();
  jsonDoc[key] = value;

  char jsonBuffer[200];
  serializeJson(jsonDoc, jsonBuffer);
  
  client.publish(topic, jsonBuffer);
  Serial.print("Publicado en ");
  Serial.print(topic);
  Serial.print(": ");
  Serial.println(jsonBuffer);
}

void printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Error obteniendo la hora.");
    return;
  }
  Serial.println(&timeinfo, "%Y-%m-%d %H:%M:%S");
}

void setup() {
  Serial.begin(115200);

  setup_wifi();

  // Configurar el servidor NTP para obtener la hora
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  // Mostrar la hora local después de sincronizar
  printLocalTime();

  client.setServer(mqtt_server, mqtt_port);

  // Inicializar el sensor BME280
  Wire.begin(I2C_SDA, I2C_SCL);
  if (!bme.begin(0x76)) {
    Serial.println(F("No se pudo encontrar un sensor BME280 válido, ¡verifica el cableado!"));
    while (1);
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Publicar datos del sensor cada 5 segundos
  static unsigned long lastMsg = 0;
  unsigned long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;

    // Leer datos reales del BME280
    float temperature = bme.readTemperature();
    float pressure = bme.readPressure() / 100.0F; // Conversión a hPa
    float altitude = bme.readAltitude(1013.25);  // Ajusta según la presión local
    float humidity = bme.readHumidity();

    // Publicar datos en formato JSON
    publishData("huerto/temperature", temperature, "temperature");
    publishData("huerto/pressure", pressure, "pressure");
    publishData("huerto/altitude", altitude, "altitude");
    publishData("huerto/humidity", humidity, "humidity");
  }
}
