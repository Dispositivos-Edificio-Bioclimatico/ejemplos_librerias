#include <DHT.h>
#include <DHT_U.h>



float temp, humedad;

void setup() {
  // put your setup code here, to run once:
    Serial.begin(9600);
    dht.begin();

}

void loop() {
  // put your main code here, to run repeatedly:
    humedad=dht.readHumidity();
    temp=dht.readTemperature();

    Serial.println("Temperatura:"+ String(temp)+"Humedad:"+String(humedad));

    delay(5000);
}
