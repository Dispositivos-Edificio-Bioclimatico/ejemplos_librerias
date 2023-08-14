
from umqtt.simple import MQTTClient
import gc
import json
import machine
import time
from wifi import do_connect
import math

def settimeout(duration):
  pass
def t3_publication(topic, msg):
  print (topic, ';', msg)
  pycom.rgbled(0xff00)


def publish_thingsboard(red, clave,token,UNIQUE_ID,data):
  # from umqtt.simple import MQTTClient
  # import gc
  # import json
  # import machine
  # import utime

  contador = 0
  condicion = True
  while condicion:
      try:
          client = MQTTClient(UNIQUE_ID, "tb.ier.unam.mx", port = 1883, user=token, password='')
          client.settimeout = settimeout
          client.connect()
          print(json.dumps(data))
          client.publish('v1/devices/me/telemetry', json.dumps(data) )
          client.disconnect()
          condicion = False
      except Exception as inst:
          do_connect(red,clave);
          time.sleep(10)
          contador += 1
          print("Falla ",contador)
          if contador >= 10:
              machine.reset()


def sinusoidal(periodo=300.,amplitud=10.):
    # """" periodo en segundos """"

    f = 1/periodo
    omega = 2.*math.pi*f
    tiempo = time.ticks_ms()/1000.
    valor = amplitud*math.sin(omega*tiempo)
    # print(tiempo)
    return {"Litros":valor}


def datos_thingsboard():

    red = 'WIFI-CIE-UNAM-101'
    clave = ''

    unique_id = 'd8b0f2a0-00c0-11ee-a64c-031ad2021d0e'
    token = 'S6AtZAlYfH6mkhTABdoI'

    return red,clave, unique_id, token
