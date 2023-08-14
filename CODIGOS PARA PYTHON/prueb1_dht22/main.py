from machine import Pin               # importa la clase Pin del módulo machine
import dht                            #importa el módulo dht
import time 


pin_04 = dht.DHT22(Pin(4))            # crea el objeto pin_04 para un módulo DHT22 en el pin GPIO 04

while True:
    try:
        pin_04.measure()                      # realiza la lectura de la temperatura y humedad
        temperatura = pin_04.temperature()    # asigna a la variable "temperatura" la temperatura 
        humedad = pin_04.humidity()           # asigna a la variable "humedad" la humedad
        print ("La temperatura es de " + str(temperatura) + "ºC y la humedad del " + str(humedad) + "%.")
    
    except OSError as e:
        print("Error al leer los datos del sensor", e)
        
    time.sleep(2)