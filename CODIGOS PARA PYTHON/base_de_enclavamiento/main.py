arranque = 0
enclavamiento = 0
paro = 0
estado = 0

def setup():
    pinMode(2, INPUT)
    pinMode(0, INPUT)
    pinMode(13, OUTPUT)
    pinMode(5, OUTPUT)

def loop():
    global arranque, enclavamiento, paro, estado
    arranque = digitalRead(2)
    paro = digitalRead(0)
    enclavamiento = estado
    if arranque == HIGH:
        estado = 1
    if estado == 1:
        digitalWrite(13, LOW)
        digitalWrite(5, HIGH)
        print("encendido el 5")
    else:
        digitalWrite(13, HIGH)
        digitalWrite(5, LOW)
        print("encendido el 13")
    if paro == HIGH:
        estado = 0