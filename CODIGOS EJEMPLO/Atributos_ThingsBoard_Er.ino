#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "WIFI-CIE-UNAM-101";
const char* password = "";
const char* mqttServer = "tb.ier.unam.mx"; 
const char* deviceToken = "wdfuaAGMmbW0CbUvpYjN"; 

WiFiClient espClient;
PubSubClient client(espClient);

#define LED_1 2
#define LED_2 4

int Estado_led_1 = 0;
int Estado_led_2 = 0;


void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  pinMode(LED_1,OUTPUT);
  pinMode(LED_2,OUTPUT);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  client.setServer(mqttServer, 1883);
  client.setCallback(callback);
  reconnect();
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  
  digitalWrite(LED_1,LOW);
  digitalWrite(LED_2,LOW);
  client.loop();
}

void callback(char* topic, byte* payload, unsigned int length) {
  // Procesa los mensajes recibidos desde ThingsBoard
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  Serial.println(message);
  Serial.println(Estado_led_1);
  Serial.println(Estado_led_2);
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP32Client", deviceToken, NULL)) {
      Serial.println("Connected to MQTT broker");
      // Leer los atributos del dispositivo en ThingsBoard
      String attributeTopic1 = "v1/devices/me/attributes/Estado_1";
      String attributeTopic2 = "v1/devices/me/attributes/Estado_3";
      String attributeTopic3 = "v1/devices/me/attributes/1";
      client.subscribe(attributeTopic1.c_str());
      client.subscribe(attributeTopic2.c_str());
      client.subscribe(attributeTopic3.c_str());

      Serial.println(Estado_led_1);
      Serial.println(Estado_led_2);
      Serial.println(LED_1);
      Serial.println(LED_2);
      Serial.println(attributeTopic1.c_str());
      Serial.println(attributeTopic2.c_str());
      Serial.println(attributeTopic3.c_str());
      
    } else {
      Serial.print("Failed to connect to MQTT broker, rc=");
      Serial.print(client.state());
      Serial.println(" Retrying in 5 seconds...");
      delay(5000);
    }
  }
}
