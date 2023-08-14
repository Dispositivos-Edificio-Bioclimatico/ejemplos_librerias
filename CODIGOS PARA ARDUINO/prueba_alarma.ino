//le tengo que modificar los print porque solo dice que se conecta a internet y no me arroja otra información 

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

const char* ssid = "WIFI-CIE-UNAM-101";
const char* password = "";
const long utcOffsetInSeconds = 3600; // Ajusta el desfase horario según tu zona horaria
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

const int ledPin = 2; // Pin del LED
const int horaEncendido = 14; // Hora de encendido (formato de 24 horas)
const int horaApagado = 4; // Hora de apagado (formato de 24 horas)

void setup() {
  pinMode(ledPin, OUTPUT);
  
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }

  timeClient.begin();
  timeClient.update();
}

void loop() {
  timeClient.update();

  int currentHour = timeClient.getHours();

  if (currentHour == horaEncendido) {
    digitalWrite(ledPin, HIGH);
  } else if (currentHour == horaApagado) {
    digitalWrite(ledPin, LOW);
  }

  delay(1000); // Espera 1 segundo antes de verificar la hora nuevamente
}
