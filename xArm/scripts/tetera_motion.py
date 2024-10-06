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
arm.motion_enable(enable=True)
arm.reset(wait=True)
arm.set_mode(0)
arm.set_state(state=0)
arm.reset(wait=False)
arm.move_gohome()

def wait_for_enter():
    """
    Espera a que usuario presione enter.
    """
    while True:
        input("Press Enter to continue...")
        break

def move_circle(radius, initial_pos, interrupt_time):
    """
    Mueve el xArm6 en un círculo.
    """
    print("Moviendo en círculo...")
    initial_x, initial_y, initial_z, initial_roll, initial_pitch, initial_yaw = initial_pos
    roll_inc = 0
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
        pos1 = [initial_x+radius, initial_y-offset, initial_z, initial_roll+roll_inc, initial_pitch, initial_yaw]
        pos2 = [initial_x+radius, initial_y+offset, initial_z, initial_roll+roll_inc, initial_pitch, initial_yaw]
        toggle_pos = not toggle_pos
        roll_inc -= 1.2
        # mover a las nuevas coordenadas
        arm.move_circle(pos1, pos2, 50, speed=ARM_SPEED*1.5, mvacc=ARM_ACCEL, wait=True)
        # esperar un pequeño intervalo para suavizar el movimiento
        time.sleep(0.01)

ARM_SPEED = 12
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
arm.set_servo_angle(angle=[-17.8, -7.8, -50.6, -66.1, 107.4, 35.2], speed=ARM_SPEED, mvacc=ARM_ACCEL, wait=False, radius=60)
# 2) inclinar tetera
arm.set_servo_angle(angle=[-14.9, -2.9, -47.1, -87.8, 124.8, 32.8], speed=(ARM_SPEED*0.3), mvacc=ARM_ACCEL, wait=True, radius=0.0)

code, center_pos = arm.get_position(is_radian=False)
center_pos = [round(val, 1) for val in center_pos]
print(f"Center pos: {center_pos}")
radius = 25
interrupt_time = 20 # segundos
move_circle(radius, center_pos, interrupt_time)

## REGRESAR TETERA
# 1) enderezar tetera
code, current_pos = arm.get_position(is_radian=False)
arm.set_position(current_pos[0], current_pos[1], current_pos[2]+50, 99, current_pos[4], current_pos[5], wait=False, speed=ARM_SPEED*2, mvacc=ARM_ACCEL*3, radius=0.0)
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