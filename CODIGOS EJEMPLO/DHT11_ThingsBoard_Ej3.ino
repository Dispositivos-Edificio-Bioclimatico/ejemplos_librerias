/*
 * El ejemplo de esta semana es una conexión hacia la plataforma ThingsBoard para proyectos 
 * de Internet de las Cosas (IoT)
 * Realizado por: Sebastian Carranza - Equipo TodoMaker
 * Propiedad y Derechos: Equipo TodoMaker
 * Para esta semana, necesitamos lo siguiente:
 * ESP32 Dev Kit
 * Sensor DHT11
 * Cables Jumper Male - Female
 * Cable USB a MicroUSB
 * Una red WiFi existente en nuestro hogar, lugar de trabajo, etc.
 * 
 * No habrá una conexión física con algún componente extra.
 */
/*Incluimos primero las librerías*/
#include "ThingsBoard.h"
#include <WiFi.h>
#include "DHT.h"
/*Definimos que el Pin de Datos del sensor estará conectado al pin 23 del ESP32DevKit*/
#define DHTPIN  23      
/*Definimos que el tipo de sensor que estamos utilizando es un DHT11*/
#define DHTTYPE DHT11     // DHT 11
/*Se actualizan algunos parámetros del DHT11 con los puntos definidos anteriormente*/ 
DHT dht(DHTPIN, DHTTYPE);

/*Definimos el nombre y contraseña de la Red WiFi a utilizar*/
#define WIFI_AP             "TAKO"
#define WIFI_PASSWORD       "son500pesos"

/*Definimos 2 variables, el Access Token obtenido de la plataforma, y el servidor al cual se enviarán los datos*/
#define TOKEN               "wdfuaAGMmbW0CbUvpYjN"
#define THINGSBOARD_SERVER  "iot.thingsboard.io"

/*Definimos la velocidad del terminal Serial a una velocidad de 115200*/
#define SERIAL_DEBUG_BAUD   115200
/*Definimos el cliente WiFi que usaremos*/
WiFiClient espClient;
/*Definimos que el cliente MQTT a utilizar para la conexión a la plataforma, es la que definimos líneas arriba*/
ThingsBoard tb(espClient);
/*Incluimos el status del radio WiFi*/
int status = WL_IDLE_STATUS;
/*Iniciamos la función Setup()*/
void setup() {
  /*Iniciamos el terminal Serial a una velocidad de 115200*/
  Serial.begin(SERIAL_DEBUG_BAUD);
  /*Iniciamos la conexión Wifi, con las credenciales*/
  WiFi.begin(WIFI_AP, WIFI_PASSWORD);
  /*Incluimos un retardo de 1 segundo e iniciamos nuestro sensor DHT junto a la función de incio WiFi*/
  delay(1000);
  dht.begin();
  InitWiFi();
}

/*Iniciamos la función Loop()*/
void loop() {
  /*Usamos un retardo de 1 segundo, y agregamos una condicional donde se resalta que, si no se conecta a la red, se pasa a usar la funcion Reconnect()*/
  delay(1000);
  if (WiFi.status() != WL_CONNECTED) {
    reconnect();
  }
  /*Usamos una condicional donde resalta que si no se realiza la conexión, se pasa a usar las credenciales para lograr la conexión a la plataforma*/
  if (!tb.connected()) {
    // Connect to the ThingsBoard
    Serial.print("Connecting to: ");
    Serial.print(THINGSBOARD_SERVER);
    Serial.print(" with token ");
    Serial.println(TOKEN);
    /*Si no se realiza la conexión usando las credenciales, se procede a imprimir la frase de error de conexión*/
    if (!tb.connect(THINGSBOARD_SERVER, TOKEN)) {
      Serial.println("Failed to connect");
      return;
    }
  }
  /*Una vez realizada la conexión, se procede a imprimir la frase que indica el inicio del envío de datos*/
  Serial.println("Sending data...");

  /*Se procede a realizar la lectura de los valores del sensor*/
  float h = dht.readHumidity();       // Lectura de humedad
  float t = dht.readTemperature();     // Lectura de temperatura
  Serial.println("Temperatura: ");
  Serial.println(t);
  Serial.println("Humedad: ");
  Serial.println(h);
  /*Se procede a hacer el envío de los datos del sensor DHT con el respectivo nombre del valor*/
  tb.sendTelemetryInt("Temperatura", t);
  tb.sendTelemetryFloat("Humedad", h);
  /*Agregamos un retardo de 5 segundos e iniciamos un bucle*/
  delay(5000);
  tb.loop();
}
/*Mostramos la función InitWifi, para el inicio de la conexión a la red WiFi*/
void InitWiFi()
{
  /*Imprimimos la frase, para indicar el inicio de la conexión*/
  Serial.println("Connecting to AP ...");
  /*Iniciamos la conexión a la red, y se muestran caracteres indicando el tiempo que tarda en realizar la conexión*/
  WiFi.begin(WIFI_AP, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  /*Una vez realizada la conexión, se mostrará la siguiente frase*/
  Serial.println("Connected to AP");
}
/*Definimos la función Reconnect(), para realizar la conexión a la red*/
void reconnect() {
  /*Se indica una condicional donde, si no se realiza la conexión, se imprimirán caracteres indicando el tiempo que tarda en realizar la conexión*/
  status = WiFi.status();
  if ( status != WL_CONNECTED) {
    WiFi.begin(WIFI_AP, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    /*Una vez realizada la conexión a la red, se procede a imprimir una frase*/
    Serial.println("Connected to AP");
  }
}
