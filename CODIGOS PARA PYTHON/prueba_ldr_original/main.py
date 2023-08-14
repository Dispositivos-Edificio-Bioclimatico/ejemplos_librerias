# from wifi import do_connect
import time
from funcion import *
from wifi import activate_wifi

red,clave, unique_id, token = datos_thingsboard()
activate_wifi(red,clave)

while True:
    data   = sinusoidal()
    publish_thingsboard(red,clave,token, unique_id,data)
    time.sleep(5)
