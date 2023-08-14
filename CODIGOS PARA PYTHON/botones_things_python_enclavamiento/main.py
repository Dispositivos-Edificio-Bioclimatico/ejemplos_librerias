import machine
import time
import network
import ujson
from umqtt.simple import MQTTClient

# Configuración de pines GPIO
led1_pin = machine.Pin(3, machine.Pin.OUT)
led2_pin = machine.Pin(2, machine.Pin.OUT)
button1_pin = machine.Pin(4, machine.Pin.IN, machine.Pin.PULL_UP)
button2_pin = machine.Pin(5, machine.Pin.IN, machine.Pin.PULL_UP)


# define enclavamiento, arranque, paro, y estado 
arranque = 0
enclavamiento = 0
paro = 0
estado = 0


# Configuración de la conexión Wi-Fi
wifi_ssid = "WIFI-CIE-UNAM-101"  # Reemplaza con el nombre de tu red Wi-Fi
wifi_password = ""  # Reemplaza con la contraseña de tu red Wi-Fi

# Configuración de la conexión MQTT con ThingsBoard
thingsboard_server = "tb.ier.unam.mx"  # Reemplaza con la dirección de tu servidor ThingsBoard
access_token = "S6AtZAlYfH6mkhTABdoI"  # Reemplaza con el token de acceso de tu dispositivo

# Conexión Wi-Fi
def connect_wifi():
    sta_if = network.WLAN(network.STA_IF)
    if not sta_if.isconnected():
        print("Conectando a Wi-Fi...")
        sta_if.active(True)
        sta_if.connect(wifi_ssid, wifi_password)
        while not sta_if.isconnected():
            pass
    print("Conectado a Wi-Fi:", sta_if.ifconfig())

# Callback ejecutado al recibir un mensaje MQTT
def on_message(topic, msg):
    print("Mensaje recibido: ", topic, msg)
    # Parsear el mensaje JSON recibido
    payload = ujson.loads(msg)
    # Verificar si es un mensaje para controlar los LEDs virtuales
    if "method" in payload and payload["method"] == "control_leds":
        if "params" in payload and "led1" in payload["params"] and "led2" in payload["params"]:
            led1_state = payload["params"]["led1"]
            led2_state = payload["params"]["led2"]
            # Controlar los LEDs virtuales
            led1_pin.value(led1_state)
            led2_pin.value(led2_state)
            # Enviar el estado actual de los LEDs a ThingsBoard
            client.publish("v1/devices/me/telemetry", ujson.dumps({"led1": led1_state, "led2": led2_state}))

# Conexión MQTT
def connect_mqtt():
    client = MQTTClient("esp8266", thingsboard_server, user=access_token, password="")
    client.set_callback(on_message)
    client.connect()
    client.subscribe("v1/devices/me/rpc/request/+")
    print("Conectado a ThingsBoard MQTT")
    return client



# Bucle principal
def main():
    connect_wifi()
    client = connect_mqtt()
    try:
        while True:
            client.check_msg()
            # Verificar el estado de los botones físicos
            button1_state = button1_pin.value()
            button2_state = button2_pin.value()
            # Enviar el estado de los botones a ThingsBoard
            client.publish("v1/devices/me/telemetry", ujson.dumps({"abierto": button1_state, "cerrado": button2_state}))
            time.sleep(0.1)
    except KeyboardInterrupt:
        client.disconnect()

# Ejecutar el programa principal
if __name__ == '__main__':
    main()
