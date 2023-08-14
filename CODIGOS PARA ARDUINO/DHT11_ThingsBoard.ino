#include <DHT.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

#define DHTPIN 2       // Pin donde está conectado el DHT11
#define DHTTYPE DHT11  // Tipo de sensor DHT (DHT11 o DHT22)

const char* ssid = "WIFI-CIE-UNAM-101";           // Cambia por el nombre de tu red Wi-Fi
const char* password = "";     // Cambia por la contraseña de tu red Wi-Fi

const char* thingsboardServer = "tb.ier.unam.mx"; // Cambia a tu servidor ThingsBoard si es diferente
const char* accessToken = "wdfuaAGMmbW0CbUvpYjN";           // Cambia por tu Access Token de ThingsBoard

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();
  connectToWiFi();
}

void loop() {
  delay(10000);
  float temperature = dht.readTemperature();

  if (isnan(temperature)) {
    Serial.println("Error al leer el sensor DHT!");
    return;
  }

  Serial.print("Temperatura: ");
  Serial.print(temperature);
  Serial.println(" °C");

  // Enviar datos a ThingsBoard
  sendToThingsBoard(temperature);
}

void connectToWiFi() {
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Conectado a la red WiFi");
}

void sendToThingsBoard(float temperature) {
  WiFiClient client;

  if (!client.connect(thingsboardServer, 8080)) {
    Serial.println("Error al conectar con ThingsBoard");
    return;
  }

  String json = "{\"temperature\":";
  json += temperature;
  json += "}";

  Serial.print("Enviando datos JSON a ThingsBoard: ");
  Serial.println(json);

  client.print("POST /api/v1/");
  client.print(accessToken);
  client.print("/telemetry");
  client.println(" HTTP/1.1");
  client.println("Host:" thingsboardServer ); // Cambia esto si estás usando tu propio servidor ThingsBoard
  client.println("Content-Type: application/json");
  client.println("Connection: close");
  client.print("Content-Length: ");
  client.println(json.length());
  client.println();
  client.println(json);
}

