import os
import sys
import time
import math
import numpy
sys.path.append(os.path.join(os.path.dirname(__file__), '..'))
from xarm.wrapper import XArmAPI

from scripts.barista_move import *

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
control_gripper(arm, 1)
control_gripper(arm, 0)
get_digio_value(arm)

estado = ST_ON
arm.set_cgpio_digital(DOUT4_ENABLE, 1)
print("> Estado: ST_ON")
time.sleep(1)

estado = ST_CHMX
print("> Estado: ST_CHMX (2)")
time.sleep(1)
tarar(0)
# colocar Chemex y esperar a que llegue a peso
print("Esperando GRAMAJE. ", end="")
if get_digio_value(arm, DIN1_BAL) == LOW:
    time.sleep(0.5)
    print(". ", end="")
print("Chemex pesado!")


estado = ST_CAFE
print("> Estado: ST_CAFE (3)")
time.sleep(1)
tarar(0)
# colocar café y esperar a que llegue a peso
print("Esperando GRAMAJE. ", end="")
if get_digio_value(arm, DIN1_BAL) == LOW:
    time.sleep(0.5)
    print(". ", end="")
print("Cafe pesado!")


estado = ST_AFLORAM
print("> Estado: ST_AFLORAM (4)")
time.sleep(1)
tarar(0)
# verter agua y esperar a que llegue a peso:
# mover a hervidor + abrir gripper
print_msg("a hervidor...")
arm.set_servo_angle(angle=[-45.0, -16.9, -13.1, 0.0, 30.0, -45.0], speed=ARM_SPEED, mvacc=ARM_ACCEL, wait=True, radius=0.0)
arm.set_servo_angle(angle=[-61.4, 22.6, -56.0, -65.6, 105.3, 59.9], speed=ARM_SPEED, mvacc=ARM_ACCEL, wait=True, radius=0.0)
# agarrar hervidor (cerrar gripper)

# mover a Chemex (centro)
print_msg("a chemex...")
joint_motion_cont(arm, [-20.6, 6.3, -38.5, 72.4, 100.8, -59.5])

# movimiento circular - verter agua
print("TARAR - DO5 = 1")
arm.set_cgpio_digital(DOUT5_TARE, 1)
print("ESPERAR a TARE", end=" ")
if get_digio_value(arm, DIN1_BAL) == 0:
    time.sleep(1)
    print(".", end=" ")
print("\nTARE listo")

arm_speed = 20
r_chemex = 40
pos_x = 350.0
pos_y = -50.0
pos_z = 250.0
ang_servido = -120.0
circ_repetidos = 2

print("agua en circulos...")
inc = 0
while inc < circ_repetidos : # and get_digio_value(arm, DIN_BAL) == 0:
    print(f"{inc} de {circ_repetidos}")
    arm.set_position(*[pos_x, pos_y, pos_z, ang_servido, 0.0, 0.0], speed=arm_speed, mvacc=ARM_ACCEL/2, radius=0.0, wait=True)
    arm.move_circle([pos_x + r_chemex, pos_y + r_chemex, pos_z, ang_servido, 0.0, 0.0],
                    [pos_x + r_chemex, pos_y - r_chemex, pos_z, ang_servido, 0.0, 0.0],
                    float(100), speed=arm_speed, mvacc=ARM_ACCEL/2, wait=False)
    inc += 1
print("Afloramiento pesado!")


estado = ST_BLOOM
print("> Estado: ST_BLOOM (5)")
time.sleep(1)
tarar(0)
# colocar café y esperar a que llegue a peso
print("Esperando GRAMAJE. ", end="")
if get_digio_value(arm, DIN1_BAL) == 0:
    time.sleep(0.5)
    print(". ", end="")
print("Blooming pesado!")


## END

print("a posición inicial")
arm.move_gohome(wait=True)
arm.reset(wait=True)

arm.disconnect()
print("Done.")