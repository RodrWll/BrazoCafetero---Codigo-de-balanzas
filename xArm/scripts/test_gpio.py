import os
import sys
import time
import math
import numpy
sys.path.append(os.path.join(os.path.dirname(__file__), '..'))
from xarm.wrapper import XArmAPI

from scripts.barista_func import *
import select
import keyboard

ip = "192.168.1.203" # xArm tetera
arm = XArmAPI(ip)
ARM_SPEED = 20

arm.motion_enable(enable=True)
arm.reset(wait=True)
arm.set_mode(0)
arm.set_state(state=0)
arm.reset(wait=False)
# arm.move_gohome()

def space_pressed():
    return keyboard.is_pressed('space')

def get_gpio_value(arm, output=True, pin=None):
    code, states = arm.get_cgpio_state()
    idx = 5 if output else 4
    di_states = [states[idx] >> i & 0x0001 for i in range(16)]
    
    return di_states if pin is None else di_states[pin]

## IMPRIMIR ESTADOS (test con gripper)
# previous_states = None
# while True:
#     if space_pressed():
#         break

#     code, states = arm.get_cgpio_state()
#     if previous_states is None or states[2:6] != previous_states[2:6]:

#         print(f'get_cgpio_state, code={code}')
#         # print(f'GPIO state: {states[0]}')
#         # print(f'GPIO error code: {states[1]}')
#         do_states = [states[5] >> i & 0x0001 for i in range(16)]
#         print(f'Digital->Output->ConfiguringIO: {do_states[:8]}')
#         print(f'Digital->Output->FunctionalIO: {do_states[8:]}')
#         di_states = [states[4] >> i & 0x0001 for i in range(16)]
#         print(f'Digital->Input->ConfiguringIO: {di_states[:8]}')
#         print(f'Digital->Input->FunctionalIO: {di_states[8:]}')
#         print(f'Analog->Input: {states[6:8]}')
#         print(f'Analog->Output: {states[8:10]}')
#         # print()
#         # print(f'All: {states}')
#         print()
    
#     previous_states = states
#     time.sleep(0.05)


## CONTROLLER GPIO
# value = 0
# for i in range(15+1):
#     code = arm.set_cgpio_digital(i, value)
#     print(f'set_cgpio_digital({i}, {value}), code={code}')
#     states = get_gpio_value(arm, output=True)
#     print(f'get_digio_value(), state={states}')
#     time.sleep(0.5)


## TEST BALANZA
# print()
# din = get_gpio_value(arm, output=False)
# dout = get_gpio_value(arm, output=True)
# print(f"Estados iniciales - DI: {din} | DO: {dout}")
# print("ENABLE_LOW=0 | ENABLE_HIGH=1 | TARE_LOW=10 | TARE_HIGH=11")
# print("q: Salir | s: Mostrar estados")
# while True:
#     user_input = input("State: ")
    
#     din = get_gpio_value(arm, output=False)
#     dout = get_gpio_value(arm, output=True)
#     if user_input == 's':
#         print(f"DI: {din} | DO: {dout}")
#     elif user_input in ['0', '1']:
#         state = 0 if int(user_input) == 0 else 1
#         arm.set_cgpio_digital(DO4_ENABLE, state)
#         print(f"DO4_ENABLE = {state} >> DO: {dout}")
#     elif user_input in ['10', '11']:
#         state = 0 if int(user_input) == 10 else 1
#         arm.set_cgpio_digital(DO5_TARE, state)
#         print(f"DO5_TARE = {state} >> DO: {dout}")
#     elif user_input not in ['0', '1', '10', '11']:
#         print("ENABLE_LOW=0 | ENABLE_HIGH=1 | TARE_LOW=10 | TARE_HIGH=11")
#         continue
#     elif user_input == 'q':
#         break


arm.disconnect()
print('Disconnected')
