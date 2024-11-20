import time
import os
import sys

sys.path.append(os.path.join(os.path.dirname(__file__), '..'))
from xarm.wrapper import XArmAPI

# barista_func.py

"""
Variables
"""
## BALANZA 
# pinout
DI1_WEIGHT = 9      # DO1
DO4_ENABLE = 12     # DI4
DO5_TARE = 13       # DI5
# estados para INPUT
HIGH = 1
LOW = 0
# estados para Gripper
ABRIR = 1
CERRAR = 0

## xARM
ARM_ACCEL = 200
ARM_SPEED_J = 30
ARM_SPEED_P = 50
# estados
ST_ON = 0
ST_TARE = 1
ST_CHMX = 2
ST_CAFE = 3
ST_BLOOM = 4
ST_SATUR = 5
ST_OFF = 6

estado = 0
"""
Funciones
"""
## GENERALES
def space_pressed():
    import keyboard
    return keyboard.is_pressed('space')

# def tarar(arm, value):
#     arm.set_cgpio_digital(DO5_TARE, value)
#     time.sleep(0.5)
#     print(f"TARE listo: {value}")

def print_msg(msg):
    estados = ['ST_ON', 'ST_TARE', 'ST_CHMX', 'ST_CAFE', 'ST_BLOOM', 'ST_SATUR', 'ST_OFF']
    print(f"{estados[estado]}: {msg}")

def print_debug(msg, debug):
    if debug == True:
        print_msg(msg)

## MOVIMIENTO
def cambiar_offset(arm, option):
    """
    option: 'tetera' o 'default'
    """
    code, last_pos = arm.get_position(is_radian=False)
    if option == 'tetera':
        # arm.set_tcp_offset([130, 30, -200, 0, 0, 0])
        arm.set_tcp_offset([127.2, 43.38, -149.88, 0, 0, 0])
    elif option == 'default':
        arm.set_tcp_offset([0, 0, 0, 0, 0, 0])
    code, current_pos = arm.get_position(is_radian=False)
    arm.set_state(state=0)
    return f'cambiar_offset("{option}") - Last pos = {last_pos}, Current pos = {current_pos}'

def modificar_pos(pos, x=0, y=0, z=0, roll=0, pitch=0, yaw=0):
    return [pos[0] + x, pos[1] + y, pos[2] + z, pos[3] + roll, pos[4] + pitch, pos[5] + yaw]

def mover_en_circulos(arm, chemex_coords, radius, interrupt_time, debug):
    start_pos = modificar_pos(chemex_coords, x=-radius)
    arm.set_position(*start_pos, is_radian=False, wait=False, speed=ARM_SPEED_P, mvacc=ARM_ACCEL, radius=0.0)
    [initial_x, initial_y, initial_z, initial_roll, initial_pitch, initial_yaw] = start_pos
    print_debug(f"mover_en_circulos() - Initial pos: {start_pos}", debug)
    roll_inc = 0
    start_time = time.time()
    toggle_pos = True
    while True:
        # interrupción
        elapsed_time = time.time() - start_time
        interrupt_signal = leer_gpio(arm, pin=DO5_TARE)
        if elapsed_time >= interrupt_time or interrupt_signal == HIGH:
            interrupt_mode = "señal" if interrupt_signal == HIGH else "tiempo"
            msg = f'{elapsed_time} / {interrupt_time} s' if interrupt_mode == "tiempo" else f'DO5_TARE == HIGH'
            print_debug(f"mover_en_circulos() - Interrupción por {interrupt_mode}: {msg}", debug)
            break
        # nuevas coordenadas
        y_offset = radius if toggle_pos else -radius
        pos1 = [initial_x+radius, initial_y-y_offset, initial_z, initial_roll-roll_inc, initial_pitch, initial_yaw]
        pos2 = [initial_x+radius, initial_y+y_offset, initial_z, initial_roll-roll_inc, initial_pitch, initial_yaw]
        toggle_pos = not toggle_pos
        roll_inc += 3.5 if elapsed_time < 6 else 2.5
        # mover a las nuevas coordenadas
        c, pos0 = arm.get_position(is_radian=False)
        print_debug(f"mover_en_circulos() - Pos1: {pos1[:3]}, Central: {pos0[:3]}, Pos2: {pos2[:3]}")
        arm.move_circle(pos1, pos2, 50, speed=ARM_SPEED_P, mvacc=ARM_ACCEL, wait=True, is_radian=False)

## GPIO
def leer_gpio(arm, pin=None, output=False):
    code, states = arm.get_cgpio_state()
    idx = 5 if output else 4
    dig_states = [states[idx] >> i & 0x0001 for i in range(16)]
    
    return dig_states if pin is None else dig_states[pin]

def controlar_gripper(arm, comm, wait=0):
    """
    comm: ABRIR (1) o CERRAR (0)
    """
    msg = "CERRAR" if comm == CERRAR else "ABRIR"
    arm.set_tgpio_digital(ionum=0, value=comm)
    time.sleep(wait)
    return f'controlar_gripper({comm}) - {msg}'