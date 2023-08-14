#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "WIFI-CIE-UNAM-101";
const char* password = "";
const char* mqtt_server = "tb.ier.unam.mx";
const int mqtt_port = 1883;
const char* your_access_token = "wdfuaAGMmbW0CbUvpYjN";
const char* attribute_name = "Estado_1";

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi!");

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  
  connectToMqtt();
}

void connectToMqtt() {
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect("ESP32Client", your_access_token, "")) {
      Serial.println("Connected to MQTT!");
      String attributeTopic = "v1/devices/me/attributes/" + String(attribute_name);
      client.subscribe(attributeTopic.c_str());
    } else {
      Serial.print("MQTT connection failed, rc=");
      Serial.print(client.state());
      Serial.println(" Retrying in 5 seconds...");
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received in topic: ");
  Serial.println(topic);

  // Convertir el payload de bytes a un string
  char msg[length + 1];
  memcpy(msg, payload, length);
  msg[length] = '\0';

  Serial.print("Payload: ");
  Serial.println(msg);
  // Aquí puedes procesar el mensaje recibido y extraer los datos que necesitas.

  // Ejemplo de cómo obtener el valor numérico del atributo (suponiendo que sea un número):
  float attributeValue = atof(msg);
   
}

void loop() {
  if (!client.connected()) {
    connectToMqtt();
  }
  client.loop();
  // Tu programa principal puede continuar aquí
  delay(5000);
}

