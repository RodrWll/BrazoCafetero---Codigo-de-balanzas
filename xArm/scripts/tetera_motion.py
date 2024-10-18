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
ARM_SPEED = 12

def wait_for_enter():
    """
    Espera a que usuario presione enter.
    """
    while True:
        input("Press Enter to continue...")
        break

def set_position(coords, speed=ARM_SPEED, mvacc=ARM_ACCEL, radius=0.0, wait=True):
    """
    Mueve el xArm6 a las coordenadas especificadas.
    """
    x, y, z, roll, pitch, yaw = coords
    arm.set_position(x, y, z, roll, pitch, yaw, speed=speed, mvacc=mvacc, radius=radius, wait=wait)

def move_circle(radius, initial_pos, interrupt_time):
    """
    Mueve el xArm6 en un círculo.
    """
    print("Moviendo en círculo...")
    initial_x, initial_y, initial_z, initial_roll, initial_pitch, initial_yaw = initial_pos
    roll_inc = 0
    diff_roll = 1
    start_time = time.time()
    toggle_pos = True
    while True:
        # interrupción
        elapsed_time = time.time() - start_time
        if elapsed_time >= interrupt_time:
            print(f"Interrupción detectada (s): {elapsed_time}")
            break
        # nuevas coordenadas
        offset = radius if toggle_pos else -radius
        if roll_inc % 2 == 0:
            diff_roll -= 1
        
        pos1 = [initial_x+radius, initial_y-offset, initial_z, initial_roll+diff_roll, initial_pitch, initial_yaw]
        pos2 = [initial_x+radius, initial_y+offset, initial_z, initial_roll+diff_roll, initial_pitch, initial_yaw]
        toggle_pos = not toggle_pos
        roll_inc += 1
        # mover a las nuevas coordenadas
        print(f"Pos1: {pos1}, Pos2: {pos2}")
        arm.move_circle(pos1, pos2, 50, speed=ARM_SPEED*1.5, mvacc=ARM_ACCEL, wait=True)
        # esperar un pequeño intervalo para suavizar el movimiento
        time.sleep(0.1)

def get_current_pos(print=False):
    """
    Obtiene la posición actual del xArm6.
    """
    code, current_pos = arm.get_position(is_radian=False)
    current_pos = [round(val, 1) for val in current_pos]
    if print: print(f"Current pos: {current_pos}")
    return current_pos

def offset_tcp(option):
    """
    Ajusta el offset del xArm6 para que el gripper se posicione correctamente.
    """
    last_pos = get_current_pos(False)
    if option == 'tetera':
        arm.set_tcp_offset([130, 0, -200, 0, 0, 0])
    elif option == 'default':
        arm.set_tcp_offset([0, 0, 0, 0, 0, 0])
    current_pos = get_current_pos(False)
    print(f'Last pos: {last_pos}, Current pos: {current_pos}')
    return current_pos

offset_tcp('default')
arm.motion_enable(enable=True)
arm.reset(wait=True)
arm.set_mode(0)
arm.set_state(state=0)
arm.reset(wait=False)
arm.move_gohome()

## TETERA
# 1) orientar a tetera
# 1a) elevar, orientar gripper y rotar
arm.set_servo_angle(angle=[-52.4, -38.0, -10.7, -63.1, 117.3, 47.9], speed=ARM_SPEED, mvacc=ARM_ACCEL, wait=False, radius=30)
# 1c) descender
arm.set_servo_angle(angle=[-49.2, 12.5, -20.8, -49.5, 95.4, 83.7], speed=ARM_SPEED, mvacc=ARM_ACCEL, wait=True, radius=30)

# 2) acercar a tetera + abrir gripper
# wait_for_enter()
control_gripper(arm, 1)
arm.set_servo_angle(angle=[-57.0, 17.7, -30.4, -57.7, 96.9, 79.3], speed=ARM_SPEED, mvacc=ARM_ACCEL, wait=True, radius=0.0)
time.sleep(1)
# 3) cerrar gripper + levantar tetera
# wait_for_enter()
control_gripper(arm, 0)
time.sleep(1)
arm.set_servo_angle(angle=[-52.5, -29.8, -30.8, -60.3, 115.9, 39.8], speed=ARM_SPEED, mvacc=ARM_ACCEL, wait=False, radius=60)

## CHEMEX
# 1) orientar a Chemex
arm.set_servo_angle(angle=[-19.3, -2.3, -44.0, -65.1, 100.5, 45.7], speed=ARM_SPEED, mvacc=ARM_ACCEL, wait=False, radius=60)
# 2) inclinar tetera
arm.set_servo_angle(angle=[-13.4, 5.3, -46.5, -80.6, 124.9, 46.3], speed=(ARM_SPEED*0.3), mvacc=ARM_ACCEL, wait=True, radius=0.0)

center_pos = offset_tcp('tetera')
radius = 15
interrupt_time = 20 # segundos
move_circle(radius, center_pos, interrupt_time)

## REGRESAR TETERA
# 1) enderezar tetera
current_pos = offset_tcp('default')
set_position(current_pos)
# 2) regresar tetera
arm.set_servo_angle(angle=[-52.5, -29.8, -30.8, -60.3, 115.9, 39.8], speed=ARM_SPEED*0.75, mvacc=ARM_ACCEL, wait=False, radius=60)
# 3) descender tetera + abrir gripper
arm.set_servo_angle(angle=[-57.0, 17.7, -30.4, -57.7, 96.9, 79.3], speed=ARM_SPEED, mvacc=ARM_ACCEL, wait=True, radius=60)
time.sleep(2)
control_gripper(arm, 1)
# 4) alejarse de tetera + cerrar gripper
time.sleep(1)
arm.set_servo_angle(angle=[-49.2, 12.5, -20.8, -49.5, 95.4, 83.7], speed=ARM_SPEED, mvacc=ARM_ACCEL, wait=True, radius=0.0)
control_gripper(arm, 0)

## REGRESAR A HOME
arm.set_servo_angle(angle=[-52.4, -38.0, -10.7, -63.1, 117.3, 47.9], speed=ARM_SPEED, mvacc=ARM_ACCEL, wait=False, radius=0.0)
arm.move_gohome()
# desconectar
arm.disconnect()
print("Fin de la ejecución.")