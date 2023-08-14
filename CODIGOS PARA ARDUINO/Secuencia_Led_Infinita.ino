int i;
int recibido;

void setup() {
    // put your setup code here, to run once:
  Serial.begin(9600);
  recibido=Serial.read();
if(recibido=='0'){
  for(i=2;i<7;i++){
  pinMode(i,OUTPUT);
  digitalWrite(i,LOW);
}
}
}
void loop() {
  // put your main code here, to run repeatedly:
  recibido=Serial.read();
if (recibido=='1'){
  while(true){
    for(i=2;i<7;i++){
      digitalWrite(i,HIGH);
      delay(100);
      digitalWrite(i,LOW);
    }
    for(i=7;i>2;i--){
      digitalWrite(i,HIGH);
      delay(100);
      digitalWrite(i,LOW);
}
}
}
}
