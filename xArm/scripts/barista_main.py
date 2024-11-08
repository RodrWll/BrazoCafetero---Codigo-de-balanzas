import os
import sys
import time
import math
import numpy
sys.path.append(os.path.join(os.path.dirname(__file__), '..'))
from xarm.wrapper import XArmAPI

from scripts.barista_func import *

ip = "192.168.1.203"
arm = XArmAPI(ip)
arm.motion_enable(enable=True)
arm.reset(wait=True)
arm.set_mode(0)
arm.set_state(state=0)
arm.reset(wait=False)
print("Conectado al xArm6.")
initial_gpio = get_gpio_value(arm)
print("GPIOs iniciales: ", initial_gpio)
# arm.move_gohome()
# print("Go home...\n")

## RUTINA
print("Iniciando rutina...\n")

# print("Probando gripper...\n")
# control_gripper(arm, ABRIR)
# control_gripper(arm, CERRAR)

estado = ST_ON
arm.set_cgpio_digital(DO4_ENABLE, HIGH)
print("DO4_ENABLE=1 - Estado: ST_ON \n")
time.sleep(1)

estado = ST_CHMX
tarar(HIGH)
print("DO5_TARE=1 - Estado: ST_CHMX \n")
time.sleep(1)
print("_Colocar Chemex en balanza_") # <código>
print("Esperando DI1_WEIGHT", end="")
while get_gpio_value(arm, DI1_WEIGHT) == LOW:
    print(".", end=" ")
    time.sleep(0.75)
print(" >> DI1_WEIGHT=1 - Chemex en balanza! \n")

estado = ST_CAFE
tarar(HIGH)
print("DO5_TARE=1 - Estado: ST_CAFE \n")
time.sleep(1)
print("_Pesar café_") # <código>
print("Esperando DI1_WEIGHT", end="")
while get_gpio_value(arm, DI1_WEIGHT) == LOW:
    print(".", end=" ")
    time.sleep(0.75)
print(" >> DI1_WEIGHT=1 - Café pesado! \n")

estado = ST_BLOOM
tarar(HIGH)
print("DO5_TARE=1 - Estado: ST_BLOOM \n")
time.sleep(1)
print("_Tetera y vertido de agua_") # <código>
pos_x = 350.0
pos_y = -50.0
pos_z = 250.0
print("Esperando DI1_WEIGHT", end="")
while True:
    # vertido de agua
    if get_gpio_value(arm, DI1_WEIGHT) == HIGH or space_pressed():
        break
    print(".", end=" ")
    time.sleep(1)
print(" >> DI1_WEIGHT=1 - Pre-infusión (bloom) lista! \n")

estado = ST_SATUR
print("> Estado: ST_SATUR (5)")
tarar(HIGH)
time.sleep(1)
print("_Vertido de agua_") # <código>
print("Esperando DI1_WEIGHT", end="")
while True:
    # vertido de agua
    if get_gpio_value(arm, DI1_WEIGHT) == HIGH or space_pressed():
        break
    print(".", end=" ")
    time.sleep(1)
print(" >> DI1_WEIGHT=1 - Infusión lista! \n")

print("Esperando filtro de café")

## END
print("go home...")
arm.move_gohome(wait=True)
arm.reset(wait=True)

arm.disconnect()
print("Fin de la rutina.")

