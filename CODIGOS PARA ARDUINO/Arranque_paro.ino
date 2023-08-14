//Este codigo es de un sistema de arranque y paro con enclavamiento digital

//se tienen dos entradas que son dos botones y dos salidas que pueden ser dos leds o se pueden conectar unos reles para controlar motores

//el enclavamiento se refiere a que solo se debera presionar una sola vez para encender o apagar el sistema 

//NO se necesitan librerias para poder correr el código


int arranque=0;
int enclavamiento=0;
int paro=0;
int estado=0;


void setup() {
  pinMode(2,INPUT);
  pinMode(0,INPUT);
  pinMode(13,OUTPUT);
  pinMode(5,OUTPUT);

}

void loop() {
  arranque = digitalRead(2);
  paro = digitalRead(0);
  enclavamiento = estado;

  if (arranque == HIGH) {
    estado = 1;
  }
  if (estado == 1){
    digitalWrite(13,LOW);
    digitalWrite(5,HIGH);
  }
  else 
  {
    digitalWrite(13,HIGH);
    digitalWrite(5,LOW);
  }
  if (paro == HIGH){
    estado = 0;
  }
}
