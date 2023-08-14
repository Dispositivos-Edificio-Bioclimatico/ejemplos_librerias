import machine
import time

# Configuración de pines
servo_pin = machine.Pin(15, machine.Pin.OUT)

# Función para mover el servomotor a una posición en grados
def mover_servomotor(grados):
    duty_cycle = (grados / 18) + 2
    servo_pin.duty(int(duty_cycle * 1023 / 20))

print("Control de servomotor desde el REPL")
print("Escribe 'mover_servomotor(grados)' para mover el servomotor a la posición deseada")

# Bucle REPL
while True:
    try:
        comando = input(">>> ")
        exec(comando)

    except KeyboardInterrupt:
        print("\nPrograma interrumpido por el usuario")
        break

    except Exception as e:
        print("Error: " + str(e))
