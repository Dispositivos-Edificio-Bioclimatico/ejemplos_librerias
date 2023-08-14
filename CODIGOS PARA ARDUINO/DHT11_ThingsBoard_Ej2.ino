
/*Incluimos primero las librerías*/

#include <ThingsBoard.h>
#include <WIFI.h>
#include "DHT.h"
/*Definimos que el Pin de Datos del sensor estará conectado al pin 23 del ESP32 DevKit*/
#define DHTPIN  2      
/*Definimos que el tipo de sensor que estamos utilizando es un DHT11*/
#define DHTTYPE DHT11     // DHT 11
/*Se actualizan algunos parámetros del DHT11 con los puntos definidos anteriormente*/ 
DHT dht(DHTPIN, DHTTYPE);


/*Definimos el nombre y contraseña de la Red WiFi a utilizar*/
#define WIFI_AP "WIFI_CIE_UNAM_101"
#define WIFI_PASSWORD ""

/*Definimos 3 variables char, donde están puestas las claves de conexión a la plataforma*/
#define TOKEN "qOYLphRwGRjsK0hzZVXQ"
#define THINGSBOARD_SERVER "demo.thingsboard.io"

#define SERIAL_DEBUG_BAUD 115200

WiFiClient espClient;

ThingsBoard tb(espClien);

int status=WL_IDLE_STATUS;


/*Iniciamos la función Setup()*/
void setup() {
  /*Iniciamos nuestro terminal Serial*/
  Serial.begin(SERIAL_DEBUG_BAUD);
  /*Iniciamos la conexión a la plataforma, usando las credenciales necesarias; además, inciamos el sensor DHT*/
  WiFi.begin(WIFI_AP, WIFI_PASWORD);
  delay(1000);
  dht.begin();
  InitWiFi();
}

/*Iniciamos la función Loop()*/
void loop() {
  /*Iniciamos la función Loop de la conexión a la plataforma*/
delay(1000);
    if (WiFi.status()!=WL_CONNECTED){
      reconnect();
    }
  if(!tb.connected()){
    Serial.print("Connecting to:");
    Serial.print(THINGSBOARD_SERVER);
    Serial.print("with token:");
    Serial.println(TOKEN);
    if (!tb.connect(THINGSBOARD_SERVER, TOKEN)){
      Serial.println("Falied to Connect");
      return;
    }
  }
  Serial.println("Sending data...");

  float h=dht.readHumidity();
  float t=dht.readTemperature();
  Serial.println("Temperatura:");
  Serial.println(t);
  Serial.println("Humendad:");
  Serial.println(h);

  tb.sednTelemetryInt("Temperatura",t);
  tb.sendTelemetryFloat("Humedad",h);

  delay(5000);
  tb.loop();

}
void InitWiFi(){

  Serial.println("Connecting to AP ...");

  WiFi.begin(WIFI_AP, WIFI_PASSWORD);

  while(WiFi.status() !=WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  Serial.println("Connect to AP");

}

void reconnect(){

  status=WiFi.status();
  if(status !=WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("connecter to AP");

}
