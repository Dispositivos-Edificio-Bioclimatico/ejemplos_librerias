#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "WIFI-CIE-UNAM-101";
const char* password = "";
const char* mqttServer = "tb.ier.unam.mx"; // Por ejemplo, demo.thingsboard.io
const char* deviceToken = "wdfuaAGMmbW0CbUvpYjN"; // Lo encuentras en ThingsBoard al crear el dispositivo

#define ledPin 2 // Pin del LED

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
 Serial.begin(115200);
 pinMode(ledPin, OUTPUT);
 WiFi.begin(ssid, password);

 while (WiFi.status() != WL_CONNECTED) {
 delay(1000);
  Serial.println("Connecting to WiFi...");
 }
 Serial.println("Connected to WiFi");
 client.setServer(mqttServer, 1883);
 
 reconnect();
}

void loop() {
 if (!client.connected()) {
 reconnect();
 }
 client.loop();
}

void callback(char* topic, byte* payload, unsigned int length) {
 String message = "";
 for (int i = 0; i < length; i++) {
 message += (char)payload[i];
 }
 Serial.print("Message arrived on topic: ");
 Serial.print(topic);
 Serial.print(". Message: ");
 Serial.println(message);

 if (String(topic) == "v1/devices/me/attributes/led_1") {
 if (message == "true") {
 digitalWrite(ledPin, HIGH);
 Serial.println("LED ON");
 } else if (message == "false") {
 digitalWrite(ledPin, LOW);
 Serial.println("LED OFF");
  }
 }
}

void reconnect() {
 while (!client.connected()) {
 if (client.connect("ESP8266Client", deviceToken, NULL)) {
  Serial.println("Connected to MQTT broker");/*____________________________________________________________________________________________________________*/
 String attributeTopic = "v1/devices/me/attributes/led_1";
 client.subscribe(attributeTopic.c_str());
 client.setCallback(callback);
 } else {
 Serial.print("Failed to connect to MQTT broker, rc=");
 Serial.print(client.state());
 Serial.println(" Retrying in 5 seconds...");
 delay(5000);
  }
 }
}

