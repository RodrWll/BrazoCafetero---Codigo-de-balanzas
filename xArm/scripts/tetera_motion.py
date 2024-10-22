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
ARM_SPEED = 15

def wait_for_enter():
    """
    Espera a que usuario presione enter.
    """
    while True:
        input("Press Enter to continue...")
        break

def offset_tcp(option):
    """
    Ajusta el offset del xArm6 para que el gripper se posicione correctamente.
    """
    code, last_pos = arm.get_position(is_radian=False)
    if option == 'tetera':
        arm.set_tcp_offset([130, 0, -200, 0, 0, 0])
    elif option == 'default':
        arm.set_tcp_offset([0, 0, 0, 0, 0, 0])
    time.sleep(1)
    code, current_pos = arm.get_position(is_radian=False)
    print(f'Offset: "{option}". Last pos = {last_pos}, Current pos = {current_pos}')

def move_circle(radius, interrupt_time):
    """
    Mueve el xArm6 en un círculo.
    """
    print("Moviendo en círculo...")
    code, initial_pos = arm.get_position(is_radian=False)
    arm.set_position(*modify_position(initial_pos, x=-10), is_radian=False, wait=True, speed=ARM_SPEED*3, mvacc=ARM_ACCEL, radius=0.0)
    code, [initial_x, initial_y, initial_z, initial_roll, initial_pitch, initial_yaw] = arm.get_position(is_radian=False)
    print(f"Initial pos: {initial_x, initial_y, initial_z, initial_roll, initial_pitch, initial_yaw} - Code: {code}")
    roll_inc = 0
    z_offset = 0
    start_time = time.time()
    toggle_pos = True
    while True:
        # interrupción
        elapsed_time = time.time() - start_time
        if elapsed_time >= interrupt_time:
            print(f"Interrupción detectada (s): {elapsed_time}")
            break
        # nuevas coordenadas
        y_offset = radius if toggle_pos else -radius
        # z_offset = 1 if toggle_pos else -1
        pos1 = [initial_x+radius, initial_y-y_offset, initial_z+z_offset, initial_roll-roll_inc, initial_pitch, initial_yaw]
        pos2 = [initial_x+radius, initial_y+y_offset, initial_z+z_offset, initial_roll-roll_inc, initial_pitch, initial_yaw]
        toggle_pos = not toggle_pos
        roll_inc += 7.5
        z_offset += 4
        # mover a las nuevas coordenadas
        arm.move_circle(pos1, pos2, 50, speed=ARM_SPEED*1.5, mvacc=ARM_ACCEL, wait=True, is_radian=False)
        # esperar un pequeño intervalo para suavizar el movimiento
        time.sleep(0.005)

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
arm.move_gohome()

## TETERA
pos_tetera = [120.0, -220.0, 129.5, 90.0, 0.0, -90.0]
# 1) orientar a tetera
# 1a) elevar, orientar gripper y rotar
arm.set_servo_angle(angle=[-52.8, -29.1, -8.0, -58.8, 111.4, 59.0], speed=ARM_SPEED, mvacc=ARM_ACCEL, wait=False, radius=30)
# 1c) descender
# arm.set_servo_angle(angle=[-33.3, 14.6, -24.7, -33.7, 98.4, 84.4], wait=True, radius=30)
arm.set_position(*modify_position(pos_tetera, y=70), is_radian=False, wait=True, speed=ARM_SPEED*3, mvacc=ARM_ACCEL*3, radius=30)

# 2) acercar a tetera + abrir gripper
control_gripper(arm, 1)
# arm.set_servo_angle(angl  e=[-43.7, 19.0, -32.8, -44.5, 99.9, 80.4], speed=ARM_SPEED, mvacc=ARM_ACCEL, wait=True, radius=0.0)
arm.set_position(*pos_tetera, is_radian=False, wait=True, speed=ARM_SPEED*3, mvacc=ARM_ACCEL*2, radius=0.0)
time.sleep(0.5)
# 3) cerrar gripper + levantar tetera
control_gripper(arm, 0)
time.sleep(0.5)
# arm.set_servo_angle(angle=[-52.5, -29.8, -30.8, -60.3, 115.9, 39.8], speed=ARM_SPEED, mvacc=ARM_ACCEL, wait=False, radius=60)
arm.set_position(*modify_position(pos_tetera, z=250, roll=10), is_radian=False, wait=False, speed=ARM_SPEED*3, mvacc=ARM_ACCEL*3, radius=60)

## CHEMEX
pos_chemex = [300.0, -210.0, 300.0, 90.0, 0, -45.0]
# 1) orientar a Chemex
# arm.set_servo_angle(angle=[-19.3, -2.3, -44.0, -65.1, 100.5, 45.7], speed=ARM_SPEED, mvacc=ARM_ACCEL, wait=False, radius=60)
arm.set_position(*modify_position(pos_chemex, roll=10), is_radian=False, wait=True, speed=ARM_SPEED*3, mvacc=ARM_ACCEL*3, radius=60)
# 2) inclinar tetera
# arm.set_servo_angle(angle=[-13.4, 5.3, -46.5, -80.6, 124.9, 46.3], speed=(ARM_SPEED*0.3), mvacc=ARM_ACCEL, wait=True, radius=0.0)
chemex_radius = 20
arm.set_position(*modify_position(pos_chemex, x=chemex_radius*1.75, y=chemex_radius*1.75, z=30, roll=-25), is_radian=False, wait=True, speed=ARM_SPEED*2, mvacc=ARM_ACCEL*2, radius=0.0)
# 3) mover en círculo
interrupt_time = 30 # segundos
move_circle(chemex_radius, interrupt_time)

## REGRESAR TETERA
# 1) enderezar tetera
code, current_pos = arm.get_position(is_radian=False)
# arm.set_position(current_pos[0], current_pos[1], current_pos[2]+20, 99, current_pos[4], current_pos[5], is_radian=False, wait=False, speed=ARM_SPEED*2, mvacc=ARM_ACCEL*3, radius=0.0)
arm.set_position(*modify_position(pos_chemex, z=20, roll=10), is_radian=False, wait=False, speed=ARM_SPEED*3, mvacc=ARM_ACCEL*3, radius=0.0)

# 2) regresar tetera a posición inicial (elevada)
print("Regresando tetera...")
# arm.set_servo_angle(angle=[-52.5, -29.8, -30.8, -60.3, 115.9, 39.8], speed=ARM_SPEED*0.75, mvacc=ARM_ACCEL, wait=False, radius=60)
arm.set_position(*modify_position(pos_tetera, z=250, roll=5), is_radian=False, wait=False, speed=ARM_SPEED*3, mvacc=ARM_ACCEL*2, radius=60)

# 3) descender tetera + abrir gripper
print("Descendiendo tetera...")
# arm.set_servo_angle(angle=[-57.0, 17.7, -30.4, -57.7, 96.9, 79.3], speed=ARM_SPEED, mvacc=ARM_ACCEL, wait=True, radius=60)
arm.set_position(*pos_tetera, is_radian=False, wait=True, speed=ARM_SPEED*3, mvacc=ARM_ACCEL*2, radius=0.0)
time.sleep(2)
control_gripper(arm, 1)

# 4) alejarse de tetera + cerrar gripper
time.sleep(1)
print("Alejándose de tetera...")
# arm.set_servo_angle(angle=[-49.2, 12.5, -20.8, -49.5, 95.4, 83.7], speed=ARM_SPEED, mvacc=ARM_ACCEL, wait=True, radius=0.0)
arm.set_position(*modify_position(pos_tetera, y=70), is_radian=False, wait=True, speed=ARM_SPEED*3, mvacc=ARM_ACCEL*2, radius=30)
control_gripper(arm, 0)

## REGRESAR A HOME
arm.set_servo_angle(angle=[-52.8, -29.1, -8.0, -58.8, 111.4, 59.0], speed=ARM_SPEED, mvacc=ARM_ACCEL, wait=False, radius=30)
arm.move_gohome()
# desconectar
arm.disconnect()
print("Fin de la ejecución.")