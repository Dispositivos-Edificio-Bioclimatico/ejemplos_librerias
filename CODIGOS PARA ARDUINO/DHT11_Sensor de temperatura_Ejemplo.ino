#include <ESP8266WiFi.h>
#include <DHT.h>

#define DHTTYPE DHT11 
#define DHTPIN 2 //GPIO2
DHT dht(DHTPIN,DHTTYPE,27);
float temp, hum;

void setup() {
  // put your setup code here, to run once:
    Serial.begin(9600);
    dht.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  temp= dht.readTemperature();
  hum= dht.readHumidity();
  Serial.println("******************************************************************");
  Serial.print("Temperatura Actual:");
  Serial.print(temp);
  Serial.println("°C");
  Serial.print("Humedad Actual:");
  Serial.print(hum, 4);
  Serial.println("~");
  Serial.println("******************************************************************");
  Serial.println();

  delay(1000);
}
