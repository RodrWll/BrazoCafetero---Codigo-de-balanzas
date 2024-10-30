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
        # arm.set_tcp_offset([130, 30, -200, 0, 0, 0])
        arm.set_tcp_offset([130, 0, -200, 0, 0, 0])
    elif option == 'default':
        arm.set_tcp_offset([0, 0, 0, 0, 0, 0])
    code, current_pos = arm.get_position(is_radian=False)
    print(f'Offset: "{option}". Last pos = {last_pos}, Current pos = {current_pos}')
    arm.set_state(state=0)

def move_circle(chemex_radius, interrupt_time):
    """
    Mueve el xArm6 en un círculo.
    """
    print("Moviendo en círculo...")
    code, initial_pos = arm.get_position(is_radian=False)
    print(f"Initial pos: {initial_pos}")
    arm.set_position(*modify_position(initial_pos, x=-chemex_radius), is_radian=False, wait=True, speed=ARM_SPEED, mvacc=ARM_ACCEL, radius=0.0)

    code, [initial_x, initial_y, initial_z, initial_roll, initial_pitch, initial_yaw] = arm.get_position(is_radian=False)
    print(f"Initial pos: {initial_x, initial_y, initial_z, initial_roll, initial_pitch, initial_yaw} - Code: {code}")
    roll_inc = 0
    start_time = time.time()
    toggle_pos = True
    while True:
        # interrupción
        elapsed_time = time.time() - start_time
        interrupt_signal = get_gpio_value(arm, pin=DI5_TARE)
        if elapsed_time >= interrupt_time or interrupt_signal == HIGH:
            print(f"Interrupción: {elapsed_time} / {interrupt_time} segundos. | DI5_TARE: {'HIGH' if not interrupt_signal else 'LOW'}")
            break
        # nuevas coordenadas
        y_offset = chemex_radius if toggle_pos else -chemex_radius
        pos1 = [initial_x+chemex_radius, initial_y-y_offset, initial_z, initial_roll-roll_inc, initial_pitch, initial_yaw]
        pos2 = [initial_x+chemex_radius, initial_y+y_offset, initial_z-1, initial_roll-roll_inc, initial_pitch, initial_yaw]
        toggle_pos = not toggle_pos
        roll_inc += 6
        # mover a las nuevas coordenadas
        c, pos0 = arm.get_position(is_radian=False)
        print(f"Pos1: {pos1[:3]}, Central: {pos0[:3]}, Pos2: {pos2[:3]}")
        arm.move_circle(pos1, pos2, 50, speed=ARM_SPEED, mvacc=ARM_ACCEL, wait=True, is_radian=False)
        # esperar
        # time.sleep(tolerance)

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
arm.set_servo_angle(angle=[-52.8, -29.1, -8.0, -58.8, 111.4, 59.0], speed=ARM_SPEED, mvacc=ARM_ACCEL, wait=False, radius=60)
# 1c) descender
# arm.set_servo_angle(angle=[-33.3, 14.6, -24.7, -33.7, 98.4, 84.4], wait=True, radius=30)
arm.set_position(*modify_position(pos_tetera, y=70), is_radian=False, speed=ARM_SPEED*3, mvacc=ARM_ACCEL*3, wait=False, radius=60)
# wait_for_enter()

# 2) acercar a tetera + abrir gripper
control_gripper(arm, ABRIR)
time.sleep(0.5)
# arm.set_servo_angle(angle=[-43.7, 19.0, -32.8, -44.5, 99.9, 80.4], speed=ARM_SPEED, mvacc=ARM_ACCEL, wait=True, radius=0.0)
arm.set_position(*pos_tetera, is_radian=False, wait=True, speed=ARM_SPEED*3, mvacc=ARM_ACCEL*2, radius=0.0)
# 3) cerrar gripper + levantar tetera
control_gripper(arm, CERRAR)
# wait_for_enter()
time.sleep(1)
# arm.set_servo_angle(angle=[-52.5, -29.8, -30.8, -60.3, 115.9, 39.8], speed=ARM_SPEED, mvacc=ARM_ACCEL, wait=False, radius=60)
arm.set_position(*modify_position(pos_tetera, z=200, roll=20), is_radian=False, wait=False, speed=ARM_SPEED*3, mvacc=ARM_ACCEL*3, radius=60)

## CHEMEX
pos_chemex = [300.0, -210.0, 300.0, 90.0, 0, -45.0]
# 1) orientar a Chemex
# arm.set_servo_angle(angle=[-19.3, -2.3, -44.0, -65.1, 100.5, 45.7], speed=ARM_SPEED, mvacc=ARM_ACCEL, wait=False, radius=60)
arm.set_position(*modify_position(pos_chemex, roll=10), is_radian=False, wait=True, speed=ARM_SPEED*3, mvacc=ARM_ACCEL*3, radius=60)
# 2) inclinar tetera
print("Inclinando tetera...")
chemex_radius = 20
# arm.set_servo_angle(angle=[-13.4, 5.3, -46.5, -80.6, 124.9, 46.3], speed=(ARM_SPEED*0.3), mvacc=ARM_ACCEL, wait=True, radius=0.0)
arm.set_position(*modify_position(pos_chemex, z=20, roll=-30), is_radian=False, wait=True, speed=ARM_SPEED*2, mvacc=ARM_ACCEL*2, radius=0.0)
# 3) mover en círculo
offset_tcp('tetera')
interrupt_time = 15 # segundos
move_circle(chemex_radius, interrupt_time)
offset_tcp('default')

## REGRESAR TETERA
# 1) enderezar tetera
arm.set_servo_angle(angle=[-19.3, -2.3, -44.0, -65.1, 100.5, 45.7], speed=ARM_SPEED*2, mvacc=ARM_ACCEL, wait=False, radius=30)
# arm.set_position(*modify_position(pos_chemex, z=20, roll=10), is_radian=False, wait=False, speed=ARM_SPEED*3, mvacc=ARM_ACCEL*3, radius=0.0)

# 2) regresar tetera a posición inicial (elevada)
print("Regresando tetera...")
# arm.set_servo_angle(angle=[-52.5, -29.8, -30.8, -60.3, 115.9, 39.8], speed=ARM_SPEED*0.75, mvacc=ARM_ACCEL, wait=False, radius=60)
arm.set_position(*modify_position(pos_tetera, z=250, roll=5), is_radian=False, wait=False, speed=ARM_SPEED*3, mvacc=ARM_ACCEL*2, radius=60)

# 3) descender tetera + abrir gripper
print("Descendiendo tetera...")
# arm.set_servo_angle(angle=[-57.0, 17.7, -30.4, -57.7, 96.9, 79.3], speed=ARM_SPEED, mvacc=ARM_ACCEL, wait=True, radius=60)
arm.set_position(*pos_tetera, is_radian=False, wait=True, speed=ARM_SPEED*3, mvacc=ARM_ACCEL*2, radius=0.0)
time.sleep(2)
control_gripper(arm, ABRIR)

# 4) alejarse de tetera + cerrar gripper
time.sleep(1)
print("Alejándose de tetera...")
# arm.set_servo_angle(angle=[-49.2, 12.5, -20.8, -49.5, 95.4, 83.7], speed=ARM_SPEED, mvacc=ARM_ACCEL, wait=True, radius=0.0)
arm.set_position(*modify_position(pos_tetera, y=70), is_radian=False, wait=True, speed=ARM_SPEED*3, mvacc=ARM_ACCEL*2, radius=30)
control_gripper(arm, CERRAR)

## REGRESAR A HOME
arm.set_servo_angle(angle=[-52.8, -29.1, -8.0, -58.8, 111.4, 59.0], speed=ARM_SPEED, mvacc=ARM_ACCEL, wait=False, radius=30)
arm.move_gohome()
# desconectar
arm.disconnect()
print("Fin de la ejecución.")