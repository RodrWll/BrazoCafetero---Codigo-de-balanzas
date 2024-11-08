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
arm.move_gohome()

def tarar(value):
    arm.set_cgpio_digital(DOUT5_TARE, value)
    time.sleep(0.5)
    print(f"TARE listo: {value}")
def print_msg(msg):
    estados = ['ST_ON', 'ST_TARE', 'ST_CHMX', 'ST_CAFE', 'ST_AFLORAM', 'ST_BLOOM', 'ST_OFF']
    print(f"{estados[estado]}: {msg}")

## START
print(get_digio_value(arm))

estado = ST_ON
arm.set_cgpio_digital(DOUT4_ENABLE, HIGH)
print("> Estado: ST_ON")
time.sleep(2)
print(get_digio_value(arm))

estado = ST_CHMX
print("> Estado: ST_CHMX (2)")
time.sleep(2)
tarar(HIGH)
print(get_digio_value(arm))
# colocar Chemex y esperar a que llegue a peso
print("Esperando GRAMAJE - entrando a loop")
while get_digio_value(arm, DIN1_BAL) == LOW:
    time.sleep(0.5)
print("Chemex pesado!")

estado = ST_CAFE
print("> Estado: ST_CAFE (3)")
time.sleep(2)
tarar(HIGH)
# colocar café y esperar a que llegue a peso
print("Esperando GRAMAJE. ", end="")
while get_digio_value(arm, DIN1_BAL) == LOW:
    time.sleep(0.4)
    print(". ", end="")
print("Cafe pesado!")


estado = ST_AFLORAM
print("> Estado: ST_AFLORAM (4)")
time.sleep(2)
tarar(HIGH)
# verter agua y esperar a que llegue a peso:
# mover a hervidor + abrir gripper
print_msg("a hervidor...")

# mover a Chemex (centro)
print_msg("a chemex...")

# movimiento circular - verter agua
print("TARAR - DO5 = 1")
time.sleep(1)
tarar(HIGH)

print("agua en circulos...")
time.sleep(2)
print("Afloramiento pesado!")

estado = ST_BLOOM
print("> Estado: ST_BLOOM (5)")
time.sleep(1)
tarar(0)
# colocar café y esperar a que llegue a peso
print("Esperando GRAMAJE. ", end="")
if get_digio_value(arm, DIN1_BAL) == 0:
    time.sleep(0.4)
    print(". ", end="")
print("Blooming pesado!")

## END

print("a posición inicial")
arm.move_gohome(wait=True)
arm.reset(wait=True)

arm.disconnect()
print("Done.")