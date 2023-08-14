const int pinSensor = A0;
const int pin13 = 10;
const int pin8 = 9;
int valsensor = 0;
int temperatura = 0;

void setup() {
  // put your setup code here, to run once:
pinMode(pin13,OUTPUT);
pinMode(pin8,OUTPUT);
Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
valsensor=analogRead(pinSensor);
int milivolts=(valsensor/1023.0)*5000;
temperatura=milivolts/10;
Serial.println(temperatura);

if (temperatura>=29){
  digitalWrite(pin13,HIGH);
  digitalWrite(pin8,LOW);
}
else {
  digitalWrite(pin13,LOW);
  digitalWrite(pin8,HIGH);
}
delay(1000);
}
