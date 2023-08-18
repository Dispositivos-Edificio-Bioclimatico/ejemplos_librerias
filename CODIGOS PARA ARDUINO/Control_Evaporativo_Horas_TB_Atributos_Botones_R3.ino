#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <NTPClient.h>                  //importamos la librería del cliente NTP
#include <WiFi.h>                       //librería necesaria para la conexión wifi
#include <WiFiUdp.h>                    // importamos librería UDP para comunicar con 
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,4);

#define WIFI_AP "WIFI-CIE-UNAM-101"           // IER IZZI-14A8  WIFI-CIE-UNAM-101  WIFI-IOT-NB
#define WIFI_PASSWORD ""                      // acadier2014  internet2021 7vHOej1tqKVKZn82
#define TOKEN "atbBhetryw345gf"

#define seccion1 25
#define seccion2 26
#define compresor1 4
#define compresor2 2

#define GPIO0 16
#define GPIO2 17

#define GPIO0_PIN 3
#define GPIO2_PIN 5

boolean cs,ss,ca,cb;
char thingsboardServer[] = "tb.ier.unam.mx";
WiFiClient wifiClient;
PubSubClient client(wifiClient);
int status = WL_IDLE_STATUS;

boolean gpioState[] = {false, false};

int hora,minuto,segundo,mh,mtbe,mtba;
int mir=0,sta=0,he,me,ha,ma,mirs,stas;
int count=0;
int countb=0;
int l_activo;
int l2_activo;

//iniciamos el cliente udp para su uso con el server NTP
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "3.mx.pool.ntp.org",-21600,6000);

const byte PIN_BUTTON_A = 12;
const byte PIN_BUTTON_B = 14;
const byte PIN_BUTTON_C = 32;
const byte PIN_BUTTON_D = 35;
const byte PIN_BUTTON_E = 27;
const byte PIN_BUTTON_F = 33;
boolean pressed_a = true;
boolean pressed_b = true;
boolean pressed_c = true;
boolean pressed_d = true;
boolean pressed_e = true;
boolean pressed_f = true;

int STATE1 = 0;
int STATE = 0;

boolean L1 = false; 
boolean L2 = false;

int estadoRele1;
int estadoRele2;

unsigned long tr,ttbe,ttba;         //tiempo real, tiempo tb encendido,tiempo tb apagado.
static unsigned long intertb;       //intervalo tb
///--------------------------------------------------------------ESTABLECER LAS SALIDAS DIGITALES
void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  lcd.clear();

  pinMode(GPIO0, OUTPUT);
  pinMode(GPIO2, OUTPUT);

  pinMode(seccion1, OUTPUT);
  pinMode(seccion2, OUTPUT);
  digitalWrite(seccion1, LOW);
  digitalWrite(seccion2, LOW);
  pinMode(compresor1, OUTPUT);
  pinMode(compresor2, OUTPUT);
  digitalWrite(compresor1, LOW);
  digitalWrite(compresor2, LOW);
  pinMode(PIN_BUTTON_A, INPUT);
  pinMode(PIN_BUTTON_B, INPUT);
  pinMode(PIN_BUTTON_C, INPUT);
  pinMode(PIN_BUTTON_D, INPUT);
  pinMode(PIN_BUTTON_E, INPUT);
  pinMode(PIN_BUTTON_F, INPUT);
  estadoRele1 = LOW;
  estadoRele2 = LOW;
  InitWiFi();
  client.setServer( thingsboardServer, 1883 );
  client.setCallback(on_message);
  String json = "{\"gpio_Status\":";
  json += GPIO2;
  json += "}";
  timeClient.begin(); 
}
//----------------------------------------------------------MENU DE INICIO
void loop() {
  if ( !client.connected() ) {
    reconnect();
  }
  
  lcd.clear(); 
  lcd.setCursor(0,0);
  lcd.print("(A) MODO MAN.");
  lcd.setCursor(0,2);
  lcd.print("(B) MODO TB");
  
  digitalWrite(seccion1, LOW);
  digitalWrite(seccion2, LOW);
  digitalWrite(compresor1, LOW);
  digitalWrite(compresor2, LOW);
  
  if(pressed_a == digitalRead(PIN_BUTTON_A)){
    modo_manual();
  }
  
  if(pressed_b == digitalRead(PIN_BUTTON_B)){
    modotb();
  }

  client.loop();
  delay(1000);
  }


  //-----------------------------------------------------------MODO MANUAL
void modo_manual() {
  while(pressed_a != digitalRead(PIN_BUTTON_F)){
    lcd.clear(); 
    lcd.setCursor(0,0);
    lcd.print("(E) Act. MODO MANUAL");
    lcd.setCursor(0,1);
    lcd.print("(A) T.ROC.(B) T.Int.");
    lcd.setCursor(0,2);
    lcd.print("(C) ON/OFF L1");
    lcd.setCursor(0,3);
    lcd.print("(D) ON/OFF L2");

    if(pressed_a == digitalRead(PIN_BUTTON_A)){
      tiempo_rocio();
      delay(200);
    }
    if(pressed_b == digitalRead(PIN_BUTTON_B)){
      tiempo_intervalo();
      delay(200);
    }
    if(pressed_c == digitalRead(PIN_BUTTON_C)){
      linea1();
      delay(200);
    }
    if(pressed_d == digitalRead(PIN_BUTTON_D)){
      linea2();
      delay(200);
    }
    if(pressed_e == digitalRead(PIN_BUTTON_E)){
      activar_salidas();
      delay(200);
    }
    delay(1000);
  }
}
///-----------------------------------------------------------MENU DEL MODO MANUAL
void activar_salidas(){
    lcd.clear(); 
    lcd.setCursor(0,0);
    lcd.print("MODO MANUAL Act.");
    lcd.setCursor(0,1);
    lcd.print("T.ROC");
    lcd.setCursor(7,1);
    lcd.print("T.INT");
    lcd.setCursor(13,1);
    lcd.print("L1");
    lcd.setCursor(18,1);
    lcd.print("L2");
    lcd.setCursor(0,2);
    lcd.print(sta);
    lcd.setCursor(7,2);
    lcd.print(mir);
    lcd.setCursor(13,2);
    if (L1 == true){
      lcd.print("ON");
    } 
      else{
           lcd.print("OFF");
    }
    lcd.setCursor(18,2);
    if (L2 == true){
      lcd.print("ON");
    } 
      else{
          lcd.print("OFF");
    }
    lcd.setCursor(0,3);
    lcd.print("Salir boton (F)");
    count=0;
    countb=0;/////////////
    while(pressed_a != digitalRead(PIN_BUTTON_F)){
  
       if (sta > 0 && pressed_a != digitalRead(PIN_BUTTON_F)) {
          while (pressed_a != digitalRead(PIN_BUTTON_F)) {
              count++, countb++;
               delay(10); 
      
             // Serial.println(count/100);
              digitalWrite(seccion1,LOW);
              digitalWrite(seccion2,LOW);

             if (countb/100 ==0){
                 publicaintervalo();
              }
             if (countb/100 >60 ){
               countb=0;
              }


       
    l_activo = 0;
    l2_activo = 0;  
    if (count/100 >= mir*60 ) {
    if ( count/100 <=( (mir*60)+ (sta*60) )) {
    if (L1 == true && L2 == true){
    estadoRele1=HIGH;
    estadoRele2=HIGH;
    l_activo = 1;
    l2_activo = 1;
        }
    if (L1 == false && L2 == false){
    estadoRele1=LOW;
    estadoRele2=LOW;
    l_activo = 0;
    l2_activo = 0;
          }

     if (L1 == false && L2 == true){

     estadoRele1=LOW;
     estadoRele2=HIGH;
     l_activo = 0;
     l2_activo = 1; 
          }
        
      if (L1 == true && L2 == false){

      estadoRele1=HIGH;
      estadoRele2=LOW;
      l_activo = 1;
      l2_activo = 0;   
        }
      digitalWrite(seccion1,estadoRele1);
      digitalWrite(seccion2,estadoRele2);
      }
      if ( count/100 >=( (mir*60)+ (sta*60))){
      count=0;
      countb=0;
        }}
      if (pressed_a == digitalRead(PIN_BUTTON_A)){
       break;
      }}}}}
///--------------------------------------------------------TIEMPO ENCENDIDO DE ROCIADO
void tiempo_rocio(){
  
  while(pressed_a != digitalRead(PIN_BUTTON_F)){
   // publicaintervalo();
    lcd.clear(); 
    lcd.setCursor(0,0);
    lcd.print("T. ROCIADO"); 
    lcd.setCursor(0,1);
    lcd.print("(F) ATRAS");
    lcd.setCursor(0,2);
    lcd.print("(A) + ");
    lcd.print("(C) -");  
    lcd.setCursor(0,3);
    lcd.print("ACTUAL: ");  
    lcd.print(sta);
    lcd.print (" MINUTOS");
    if (pressed_a == digitalRead(PIN_BUTTON_A)){
    sta++;
    STATE = 1;
    delay(200);
    }
    if (pressed_c == digitalRead(PIN_BUTTON_C)){
    sta--;
    STATE = 1;
    delay(200);
    }
    delay(1000);
  }
}
///---------------------------------------------------------------TIEMPO APAGADO DE ROCIADO
void tiempo_intervalo(){
  while(pressed_a != digitalRead(PIN_BUTTON_F)){
  //  publicaintervalo();
    lcd.clear(); 
    lcd.setCursor(0,0);
    lcd.print("T. Intervalo"); 
    lcd.setCursor(0,1);
    lcd.print("(F) ATRAS");
    lcd.setCursor(0,2);
    lcd.print("(A) + ");
    lcd.print("(C) -");  
    lcd.setCursor(0,3);
    lcd.print("ACTUAL: ");  
    lcd.print(mir);
    lcd.print (" MINUTOS");
    if (pressed_a == digitalRead(PIN_BUTTON_A)){
    mir++;
    STATE = 1;
    delay(200);
    }
    if (pressed_c == digitalRead(PIN_BUTTON_C)){
    mir--;
    STATE = 1;
    delay(200);
    }
    delay(1000);
  }
}
///--------------------------------------------------------ACTIVACION DE LINEA 1
void linea1(){
  while(pressed_a != digitalRead(PIN_BUTTON_F)){
    lcd.clear(); 
    lcd.setCursor(0,0);
    lcd.print("LINEA 1");
    lcd.setCursor(0,1);
    lcd.print("(F) ATRAS");
    lcd.setCursor(0,2);
    lcd.print("(A) ON ");
    lcd.print("(C) OFF");
    lcd.setCursor(0,3);
    lcd.print("ACTUAL: ");
    if (L1 == true){
      lcd.print("ON");
    } 
    else{
      lcd.print("OFF");
    }
    if (pressed_a == digitalRead(PIN_BUTTON_A)){
    L1 = true;
    //digitalWrite(seccion1, HIGH);
    delay(200);
    }
      
    if (pressed_c == digitalRead(PIN_BUTTON_C)){
    L1 = false;
    //digitalWrite(seccion1, LOW);
    delay(200);
    }
    delay(1000);
  }
}
///-------------------------------------------------------ACTIVACION DE LINEA 2
void linea2(){
  while(pressed_a != digitalRead(PIN_BUTTON_F)){
    lcd.clear(); 
    lcd.setCursor(0,0);
    lcd.print("LINEA 2");
    lcd.setCursor(0,1);
    lcd.print("(F) ATRAS");
    lcd.setCursor(0,2);
    lcd.print("(A) ON ");
    lcd.print("(C) OFF");
    lcd.setCursor(0,3);
    lcd.print("ACTUAL: ");
    if (L2 == true){
      lcd.print("ON");
    } 
    else{
      lcd.print("OFF");
    }
    if (pressed_a == digitalRead(PIN_BUTTON_A)){
    L2 = true;
    //digitalWrite(seccion1, HIGH);
    delay(200);
    }
      
    if (pressed_c == digitalRead(PIN_BUTTON_C)){
    L2 = false;
    //digitalWrite(seccion1, LOW);
    delay(200);
    }
    delay(1000);
  }
}
//--------------------------------------------------MODO THINGSBOARD

void modotb(){
    lcd.clear(); 
    lcd.setCursor(0,0);
    lcd.print("MODO THINGSBOARD");
/*__________________SECCION PARA MISTRAR LAS HORAS____________________________*/
    lcd.setCursor(0,1);
    lcd.print("ON:");
    lcd.setCursor(11,1);
    lcd.print("OFF:");

/*  //Hora de encendido "EJEMPLO"
    lcd.setCursor(3,1);
    lcd.print("24:59");
    //Hora de apagado
    lcd.setCursor(15,1);
    lcd.print("24:59");
*/  
    lcd.setCursor(0,2);
    lcd.print("TR:");
    lcd.setCursor(11,2);
    lcd.print("TI:");

/*  //Minutos Activo "EJEMPLO"
    lcd.setCursor(3,2);
    lcd.print("59");
    //Minutos Apagado
    lcd.setCursor(15,2);
    lcd.print("59");
*/
    
    lcd.setCursor(0,3);
    lcd.print("REGRESA MENU (F)");
  
 while(pressed_b != digitalRead(PIN_BUTTON_F)){
  horas();
    
  tr=(hora*3600)+(minuto*60)+ segundo;//tiempo real en segundos
  ttbe=(he*3600)+(me*60);//tiempo de encendido establecido en thingsboard
  ttba=(ha*3600)+(ma*60);  // tiempo de apagado establecido en thingsboard   

 ///encendido
  if (tr >= ttbe &&  tr<= ttba && mir>0 && sta > 0 ){
  count++;
  delay(5);
  
  lcd.setCursor(17,0);
  lcd.print("OK");
/*_________TIEMPO ESTABLECIDO EN THINGSBOARD____________________*/    
  //Hora de encendido
  lcd.setCursor(3,1);
  lcd.print(String(he));
  //Hora de apagado
  lcd.setCursor(15,1);
  lcd.print(String(ha));
  
  /*__ESPACIOS_*/
  lcd.setCursor(5,1);
  lcd.print(":");
  lcd.setCursor(17,1);
  lcd.print(":");

  //Minutos de encendido
  lcd.setCursor(6,1);
  lcd.print(String(me));
  //minutos de apagado
  lcd.setCursor(18,1);
  lcd.print(String(ma));

/*__________________TIEMPO DE ROCIADO ESTABLECIDO EN THINGSBOARD_______________*/
  //Minutos Activo
  lcd.setCursor(3,2);
  lcd.print(String(mir));
  //Minutos Apagado
  lcd.setCursor(15,2);
  lcd.print(String(sta));
/*_______________________________________________________________________________*/
  digitalWrite(seccion1, LOW);
  digitalWrite(seccion2, LOW);
  digitalWrite(compresor1, LOW);
  digitalWrite(compresor2, LOW);
  if (count/100 ==0){
  publicaintervalo();
  }
  if (count/100 >60 ){
  count=0;
  }
 //Serial.println(count/100);
  mirs=(mir*60);//
  stas= (sta*60);//
  static unsigned long intertb=tr;// tiempo real estatico
  Serial.println(intertb);


      if(ca == true){
        digitalWrite(compresor1, HIGH);
      }
      else{
      digitalWrite(compresor1, LOW);
      }
      if(cb == true){
        digitalWrite(compresor2, HIGH);
      }
      else{
        digitalWrite(compresor2, LOW);
      }


  if (tr >= (intertb+mirs)){
  
  
    if (tr <= (intertb+stas + mirs)){

      


      if (cs == true && ss == false){

          digitalWrite(seccion1, HIGH);
          digitalWrite(seccion2, LOW);
          l_activo = 1;
          l2_activo = 0;
          }
  
          if (cs == true && ss == true){          
            digitalWrite(seccion1, LOW);
            digitalWrite(seccion2, HIGH);
            l_activo = 0;
            l2_activo = 1;
            }
          if (cs == false){
          
           digitalWrite(seccion1, HIGH);
           digitalWrite(seccion2, HIGH);
           l_activo = 1;
           l2_activo = 1;
           }
            }}

    if (tr >= (intertb +mirs + stas)){
    count=0;
    l_activo = 0;
    l2_activo = 0;
    intertb=tr;
    }}
   /////apagado
    if (tr>= ttba || tr < ttbe){//&&  ||
    digitalWrite(seccion1, LOW);
    digitalWrite(seccion2, LOW);
    digitalWrite(compresor1, LOW);
    digitalWrite(compresor2, LOW);
    count=0;
    l_activo = 0;
    l2_activo = 0;
    intertb=tr;
    } 
    client.loop();
    }}
///---------------------------------------------------------RECEPCION DE DATOS 
//La devolución de llamada para cuando se recibe un mensaje PUBLISH del servidor.
void on_message(const char* topic, byte* payload ,unsigned int length) 
    {
    // Serial.println("On message");
    char json[length];//+1
    strncpy (json, (char*)payload, length);
 // json[length] = '\1';
 // Serial.print("Topic: ");
 // Serial.println(topic);
//  Serial.print("Message: ");
 // Serial.println(json);
  // Decodificar solicitud JSON
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& data = jsonBuffer.parseObject((char*)json);
  if (!data.success())
  {
   // Serial.println("parseObject() failed");
    return;
  } 
// comprobacion de datos de thingsboard
  // Comprobar el método de solicitud
  String methodName = String((const char*)data["method"]);

  if (methodName.equals("GPIO_GET")) 
  {
  // Reply with GPIO status
  String responseTopic = String(topic);
  responseTopic.replace("request", "response");
  client.publish(responseTopic.c_str(), get_gpio_status().c_str());
  } 

  else if (methodName.equals("GPIO_SET")) 
  {
  // Update GPIO status and reply
  set_gpio_status(data["params"]["pin"], data["params"]["enabled"]);
  String responseTopic = String(topic);
  responseTopic.replace("request", "response");
  client.publish(responseTopic.c_str(), get_gpio_status().c_str());
  client.publish("v1/devices/me/attributes", get_gpio_status().c_str());
  }

  if (methodName){

    
  if ( gpioState[0] == true || gpioState[1] == true){
  
  if (methodName.equals("mir")){
  mir = data["params"];// intervalo
// Serial.print(mir); 
  }
if (methodName.equals("sta")){
   sta = data["params"];//activacion
//Serial.print(sta);
    
  }

if (methodName.equals("setHe")){
   he = data["params"];
 //  Serial.print(he);
    
  }
if (methodName.equals("me")){
   me = data["params"];
 //Serial.print(me);
    
  }

if (methodName.equals("ha")){
   ha = data["params"];
// Serial.print(ha);
    
  }
if (methodName.equals("ma")){
 ma = data["params"];
 //Serial.print(ma);
    
  }

if (methodName.equals("cs")){
  cs = data["params"];
// Serial.print(cs);
    
  }

if (methodName.equals("ss")){
  ss = data["params"];
// Serial.print(ss);
    
  }
  if (methodName.equals("ca")){
  ca = data["params"];
// Serial.print(ss);
    
  }
  if (methodName.equals("cb")){
  cb = data["params"];
// Serial.print(ss);
    
  }
  }
  }
  }

  String get_gpio_status() {
  // Prepare gpios JSON payload string
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& data = jsonBuffer.createObject();

  data[String(GPIO0_PIN)] = gpioState[0] ? true : false;
  data[String(GPIO2_PIN)] = gpioState[1] ? true : false;
  char payload[256];
  data.printTo(payload, sizeof(payload));
  String strPayload = String(payload);
  Serial.print("Estado del GPIO obtenido: ");
  Serial.println(strPayload);
  return strPayload;
  }

  void set_gpio_status(int pin, boolean enabled) {
  if (pin == GPIO0_PIN) {
  // Output GPIOs state
  digitalWrite(GPIO0, enabled ? HIGH : LOW);
  // Update GPIOs state
  gpioState[0] = enabled;
  } 
  else if (pin == GPIO2_PIN) {
  // Output GPIOs state
  digitalWrite(GPIO2, enabled ? HIGH : LOW);
  // Update GPIOs state
  gpioState[1] = enabled;
  }
  }
  ///--------------------------------------------------------


  
///----------------------------------------------------------
void InitWiFi() {
 // Serial.println("Connecting to AP ...");
  lcd.clear(); 
  lcd.setCursor(0,0);
  lcd.print("Conectado a WiFi...");
  // attempt to connect to WiFi network

  WiFi.begin(WIFI_AP, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  //  Serial.print(".");
  }
 // Serial.println("Connected to AP");
}

///-------------------------------------------------------
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
  status = WiFi.status();
    if ( status != WL_CONNECTED) {
      WiFi.begin(WIFI_AP, WIFI_PASSWORD);
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
      //  Serial.print(".");
      }
    //  Serial.println("Connected to AP");
    }
   // Serial.print("Connecting to ThingsBoard node ...");
    // Attempt to connect (clientId, username, password)
    if ( client.connect("ESP32 Device", TOKEN, NULL) ) {
   //   Serial.println( "[DONE]" );
      // Subscribing to receive RPC requests
      client.subscribe("v1/devices/me/rpc/request/+");
      client.publish("v1/devices/me/attributes", get_gpio_status().c_str());
      String attributeTopic = "v1/devices/me/attributes/hora_de_encendido";
      client.subscribe(attributeTopic.c_str());
      

     
    } else {
     // Serial.print( "[FAILED] [ rc = " );
    //  Serial.print( client.state() );
   //   Serial.println( " : retrying in 5 seconds]" );
      // Wait 5 seconds before retrying
     // delay( 5000 );
    }
  }
}

///---------------------------------------------------------
void horas() {
 timeClient.update(); //sincronizamos con el server NTP

//hora
 hora=(timeClient.getHours());
  // Serial.println(hora);


//minutos
minuto=((timeClient.getMinutes()));//+1
 //Serial.println(minuto);
 
 //segundo
segundo=(timeClient.getSeconds());
//Serial.println(segundo);
 
}
///------------------------------------------------------------
void publicaintervalo(){
    
    String payload = "{";                                    
    payload += "\"Intervalo\":";payload += mir; payload += ",";
    payload += "\"L1\":";payload += l_activo; payload += ",";
    payload += "\"L2\":";payload += l2_activo; payload += ",";
    payload += "\"Activo\":";payload += sta;payload += "}";
    char attributes[1000];
    payload.toCharArray( attributes, 1000 );
    client.publish( "v1/devices/me/telemetry",attributes);
    client.publish( "v1/devices/me/attributes",attributes); 
}
