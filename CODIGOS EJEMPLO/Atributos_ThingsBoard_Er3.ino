#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* ssid = "WIFI-CIE-UNAM-101";        // Nombre de tu red WiFi
const char* password = "";  // Contraseña de tu red WiFi

const char* mqttBroker = "tb.ier.unam.mx";
const char* accessToken = "wdfuaAGMmbW0CbUvpYjN"; // El Access Token del dispositivo en ThingsBoard

const char* topic = "v1/devices/me/telemetry";

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  
  client.setServer(mqttBroker, 1883);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  
  int attributeValue = digitalRead(2); // Leer el valor del atributo en el pin 2 (ajusta el número según tu conexión).
  
  StaticJsonDocument<200> jsonDocument;
  jsonDocument["attribute"] = attributeValue;
  
  char payload[256];
  serializeJson(jsonDocument, payload);
  
  client.publish(topic, payload);
  
  delay(5000); // Esperar 5 segundos entre cada envío de datos.
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP32", accessToken, nullptr)) {
      Serial.println("Connected to ThingsBoard MQTT Broker");
    } else {
      Serial.print("Failed to connect to ThingsBoard. Retrying in 5 seconds...");
      delay(5000);
    }
  }
}