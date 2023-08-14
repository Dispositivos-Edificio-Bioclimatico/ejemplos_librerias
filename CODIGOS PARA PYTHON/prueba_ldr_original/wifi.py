import network
import time

def do_connect(red,clave):

    sta_if = network.WLAN(network.STA_IF)
    if not sta_if.isconnected():
        print('connecting to network...')
        sta_if.active(True)
	sta_if.connect(red,clave)
        sta_if.connect(red, clave)
        while not sta_if.isconnected():
            pass
    else:
      print("Already connected")
    print('network config:', sta_if.ifconfig())
    time.sleep(5)

def activate_wifi(red,clave):
    sta_if = network.WLAN(network.STA_IF)
    sta_if.active(True)
    ap_if = network.WLAN(network.AP_IF)
    sta_if.connect(red,clave)
