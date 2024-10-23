import os
import sys
import time
import math
import numpy
sys.path.append(os.path.join(os.path.dirname(__file__), '..'))
from xarm.wrapper import XArmAPI

from scripts.barista_move import *
import select

ip = "192.168.1.203"
arm = XArmAPI(ip)
ARM_SPEED = 20

def wait_for_enter():
    print("Presione ENTER para continuar...")
    input()

def offset_tcp(option):
    """
    Ajusta el offset del xArm6 para que el gripper se posicione correctamente.
    """
    code, last_pos = arm.get_position(is_radian=False)
    if option == 'tetera':
        arm.set_tcp_offset([130, 30, -200, 0, 0, 0])
    elif option == 'default':
        arm.set_tcp_offset([0, 0, 0, 0, 0, 0])
    code, current_pos = arm.get_position(is_radian=False)
    print(f'Offset: "{option}". Last pos = {last_pos}, Current pos = {current_pos}')
    arm.set_state(state=0)

def modify_position(pos, x=0, y=0, z=0, roll=0, pitch=0, yaw=0):
    """
    Modifica la posición de un punto en el espacio.
    """
    new_pos = [pos[0] + x, pos[1] + y, pos[2] + z, pos[3] + roll, pos[4] + pitch, pos[5] + yaw]
    return new_pos

offset_tcp('default')
arm.motion_enable(enable=True)
arm.reset(wait=True)
arm.set_mode(0)
arm.set_state(state=0)
arm.reset(wait=False)
# arm.move_gohome()

## TETERA
pos_tetera = [120.0, -220.0, 129.5, 90.0, 0.0, -90.0]
# 1) orientar a tetera
arm.set_servo_angle(angle=[-52.8, -29.1, -8.0, -58.8, 111.4, 59.0], speed=ARM_SPEED, mvacc=ARM_ACCEL, wait=False, radius=60)
# 1c) descender
arm.set_position(*modify_position(pos_tetera, y=70), is_radian=False, speed=ARM_SPEED*3, mvacc=ARM_ACCEL*3, wait=False, radius=60)

offset_tcp('tetera')
print("Fin del script.")