#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h> // Asegúrate de instalar esta biblioteca
#include <time.h>        // Para obtener la fecha y hora

// Configuración de la red WiFi
const char* ssid = "";       // Nombre de tu red WiFi
const char* password = ""; // Contraseña de tu red WiFi

// Configuración del broker MQTT
const char* mqtt_server = "192.168.0.232"; // IP del broker MQTT
const int mqtt_port = 1883;               // Puerto del broker MQTT

// Temas para publicar
const char* topics[] = {
  "huerto/temperature",
  "huerto/pressure",
  "huerto/altitude",
  "huerto/humidity"
};

// Configuración de la hora
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 0; // Ajusta esto según tu zona horaria
const int daylightOffset_sec = 3600; // Offset para horario de verano (si aplica)

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
      // Suscribirse a los temas (si necesario)
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
  jsonDoc["controller_id"] = 3;
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

void setup() {
  Serial.begin(115200);

  setup_wifi();

  // Configurar el servidor NTP para obtener la hora
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Publicar datos simulados cada 5 segundos
  static unsigned long lastMsg = 0;
  unsigned long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;

    // Generar datos simulados
    float temperature = random(20, 30);
    float pressure = random(950, 1050);
    float altitude = random(10, 100);
    float humidity = random(30, 70);

    // Publicar datos en formato JSON
    publishData("huerto/temperature", temperature, "temperature");
    publishData("huerto/pressure", pressure, "pressure");
    publishData("huerto/altitude", altitude, "altitude");
    publishData("huerto/humidity", humidity, "humidity");
  }
}
