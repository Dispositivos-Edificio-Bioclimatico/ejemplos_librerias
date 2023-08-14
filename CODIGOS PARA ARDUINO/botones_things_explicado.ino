

#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>

#define WIFI_AP "WIFI-CIE-UNAM-101"
#define WIFI_PASSWORD ""

#define TOKEN "S6AtZAlYfH6mkhTABdoI"


//Estos son los pines fisicos que reaccionaran a las señales de on off
#define GPIO0 4
#define GPIO2 5
#define GPIO3 16

// EStos son los botones virtuales los cuales se registran en thingsboard
#define GPIO0_PIN 1
#define GPIO2_PIN 2
#define GPIO3_PIN 3


// Esta es la red de thingsboard a la cual se va a conectar 
char thingsboardServer[] = "tb.ier.unam.mx";

WiFiClient wifiClient;

PubSubClient client(wifiClient);

int status = WL_IDLE_STATUS;

// Para que el sistema al inicio se encuentre apagado se colocan como low 
boolean gpioState[] = {false, false, false};

void setup() {
Serial.begin(115200);
// Se colocan los pines como de salida (son los pines en fisico)
pinMode(GPIO0, OUTPUT);
pinMode(GPIO2, OUTPUT);
pinMode(GPIO3, OUTPUT);
delay(10);
InitWiFi();
client.setServer( thingsboardServer, 1883 );
client.setCallback(on_message);
}

void loop() {
if ( !client.connected() ) {
reconnect();
}

client.loop();
}

// La devolución de llamada para cuando se recibe un mensaje PUBLICAR del servidor.
void on_message(const char* topic, byte* payload, unsigned int length) {

Serial.println("On message");

char json[length + 1];
strncpy (json, (char*)payload, length);
json[length] = '\0';

Serial.print("Topic: ");
Serial.println(topic);
Serial.print("Message: ");
Serial.println(json);

// Decodifica la solicitud del msj JSON 
StaticJsonBuffer<200> jsonBuffer;
JsonObject& data = jsonBuffer.parseObject((char*)json);

if (!data.success())
{
Serial.println("parseObject() failed");
return;
}

// Este es el metodo de verificacion para la solicitud del msj
String methodName = String((const char*)data["method"]);

if (methodName.equals("getGpioStatus")) {

// Responde con el estado de los GPIO
String responseTopic = String(topic);
responseTopic.replace("request", "response");
client.publish(responseTopic.c_str(), get_gpio_status().c_str());
} else if (methodName.equals("setGpioStatus")) {

// Actualiza los GPIO con la informacion que tiene y vuelve a responder 
set_gpio_status(data["params"]["pin"], data["params"]["enabled"]);
String responseTopic = String(topic);
responseTopic.replace("request", "response");
client.publish(responseTopic.c_str(), get_gpio_status().c_str());
client.publish("v1/devices/me/attributes", get_gpio_status().c_str());          //de esta parte manda los atributos 
}
}

String get_gpio_status() {

// Prepara la cadena de datos JSON de los GPIO 
StaticJsonBuffer<200> jsonBuffer;
JsonObject& data = jsonBuffer.createObject();
data[String(GPIO0_PIN)] = gpioState[0] ? true : false;
data[String(GPIO2_PIN)] = gpioState[1] ? true : false;
data[String(GPIO3_PIN)] = gpioState[2] ? true : false;
char payload[256];
data.printTo(payload, sizeof(payload));
String strPayload = String(payload);
Serial.print("Get gpio status: ");
Serial.println(strPayload);
return strPayload;
}



void set_gpio_status(int pin, boolean enabled) {
if (pin == GPIO0_PIN) {

// Es la salida de los estados de los GPIO0 
digitalWrite(GPIO0, enabled ? HIGH : LOW);

// Actualiza el estado de los gpio
gpioState[0] = enabled;
} else if (pin == GPIO2_PIN) {

// Salida de los estados de GPIO2
digitalWrite(GPIO2, enabled ? HIGH : LOW);

// Actualiza el estado de los GPIO 
gpioState[1] = enabled;
} else if (pin == GPIO3_PIN) {


digitalWrite(GPIO3, enabled ? HIGH : LOW);


gpioState[2] = enabled;
}
}




void InitWiFi() {
Serial.println("Connecting to AP ...");

// Aca se intenta conectar a la red desde el wifi

WiFi.begin(WIFI_AP, WIFI_PASSWORD);
while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
}
Serial.println("Connected to AP");
}


void reconnect() {
// Este void intenta reconectarse a internet si no lo logra los primeros instantes 
while (!client.connected()) {
status = WiFi.status();
if ( status != WL_CONNECTED) {
WiFi.begin(WIFI_AP, WIFI_PASSWORD);
while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
}
Serial.println("Connected to AP");
}
Serial.print("Connecting to Thingsboard node ...");

// Intenta conectarse a la pag con el id, nombre de ususario y contraseña 
if ( client.connect("ESP8266 Device", TOKEN, NULL) ) {
Serial.println( "[DONE]" );

// Suscribirse para recibir solicitudes de RPC
client.subscribe("v1/devices/me/rpc/request/+");

// Envia frecuentemente los estados de los GPIO
Serial.println("Sending current GPIO status ...");
client.publish("v1/devices/me/attributes", get_gpio_status().c_str());
} else {
Serial.print( "[FAILED] [ rc = " );
Serial.print( client.state() );
Serial.println( " : retrying in 5 seconds]" );

// Espera 5 segundos antes de volver a intentarlo
delay( 5000 );
}
}
}
