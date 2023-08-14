from gpiozero import *
from time import sleep

# Define los pines GPIO para el servo y crea el objeto Servo
pin_servo = 15  # Cambia el número del pin según tu configuración
servo = Servo(pin_servo)

# Función para mover el servo a un ángulo específico
def move_servo(angle):
    # Rango de ángulo: -90 a 90 grados
    if angle < -90:
        angle = -90
    elif angle > 90:
        angle = 90
    # Convierte el ángulo al rango de -1 a 1 requerido por el Servo
    position = angle / 90.0
    servo.value = position
    sleep(0.3)  # Espera 0.3 segundos para que el servo se mueva

# Mueve el servo a diferentes ángulos según los botones presionados
while True:
    choice = input("Ingrese el ángulo (-90 a 90) o 'q' para salir: ")
    if choice == 'q':
        break
    try:
        angle = int(choice)
        move_servo(angle)
    except ValueError:
        print("Entrada no válida. Intente nuevamente.")

# Detiene el servo y libera los recursos GPIO antes de salir
servo.value = None
servo.close()
