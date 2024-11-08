import time
import os
import sys

sys.path.append(os.path.join(os.path.dirname(__file__), '..'))
from xarm.wrapper import XArmAPI
from scripts.barista_func import *

# POSICIONES
pos_filtro = []
pos_taza = []
pos_dispensador = []
pos_tetera = [120.0, -220.0, 129.5, 90.0, 0.0, -90.0] # offset: 'tetera'
pos_chemex = [505.0, -180.0, 325.0, 90.0, 0, -45.0] # offset: 'default'

def iniciar_robot(ip, arm_name, debug):
    arm = XArmAPI(ip)
    arm.motion_enable(enable=True)
    arm.reset(wait=True)
    arm.set_mode(0)
    arm.set_state(state=0)
    arm.reset(wait=False)
    if debug: print(f"iniciar_robot({arm_name}) - Robot conectado.")
    # arm.go_home()
    # if debug: print("Go home...\n")
    return arm

def iniciar_arduino(arm, debug):
    arm.set_cgpio_digital(DO4_ENABLE, HIGH)
    if debug: print("iniciar_arduino()")

def dispensar_cafe(arm, debug):
    # enviar señal a Arduino para dispensar café
    if debug: print("dispensar_cafe()")

def colocar_chemex_sobre_balanza(arm, debug):
    # mover hacia Chemex y sujetar
    # levantar Chemex y colocar sobre balanza
    # mover brazo a posición neutral
    if debug: print("colocar_chemex_sobre_balanza() - Chemex sobre balanza")

def tarar_balanza(arm, debug):
    arm.set_cgpio_digital(DO5_TARE, HIGH)
    if debug: print("tarar_balanza()")

def colocar_filtro_sobre_chemex(arm, debug):
    # mover hacia filtro y sujetar
    # levar filtro y colocar sobre Chemex
    # mover brazo a posición neutral
    if debug: print("colocar_filtro_sobre_chemex - Filtro sobre Chemex")

def tarar_chemex(arm, debug):
    arm.set_cgpio_digital(DO5_TARE, HIGH)
    if debug: print("tarar_chemex()")

def verter_cafe_sobre_filtro(arm, debug):
    # verificar que café esté listo
    # sujetar recipiente con café
    # mover hacia Chemex e inclinar recipiente
    # regresar recipiente a posición inicial
    if debug: print("verter_cafe_sobre_filtro() - Café vertido")

def pesar_cafe_vertido(arm, debug):
    arm.set_cgpio_digital(DO5_TARE, HIGH)
    if debug: print("pesar_cafe_vertido()")

def saturar_cafe(arm, debug):
    # mover hacia tetera, acercar y sujetar
    arm.set_servo_angle(angle=[-52.8, -29.1, -8.0, -58.8, 111.4, 59.0], speed=ARM_SPEED, mvacc=ARM_ACCEL, wait=False, radius=60)
    arm.set_position(*modificar_pos(pos_tetera, y=70), is_radian=False, speed=ARM_SPEED*3, mvacc=ARM_ACCEL*3, wait=False, radius=60)
    msg = controlar_gripper(arm, ABRIR, wait_min=0.5)
    print_debug(f'saturar_cafe() - {msg}', debug)
    arm.set_position(*pos_tetera, is_radian=False, wait=True, speed=ARM_SPEED*3, mvacc=ARM_ACCEL*2, radius=0.0)
    msg = controlar_gripper(arm, CERRAR, wait_min=1)
    print_debug(f'saturar_cafe() - {msg}', debug)
    # levantar y centrar tetera sobre Chemex
    msg = cambiar_offset('tetera')
    print_debug(f'saturar_cafe() - {msg}', debug)
    arm.set_position(*modificar_pos(arm.get_position(is_radian=False)[1], x=30, y=30, z=180), is_radian=False, wait=False, speed=ARM_SPEED*3, mvacc=ARM_ACCEL*3, radius=80)
    arm.set_position(*modificar_pos(pos_chemex, roll=5), is_radian=False, wait=False, speed=ARM_SPEED*3, mvacc=ARM_ACCEL*3, radius=80)
    # verter agua sobre café en movimientos circulares - interrupción por señal
    print_debug(f'saturar_cafe() - Inclinando tetera...', debug)
    inclin_inicial = modificar_pos(pos_chemex, x=12, y=0, z=-40, roll=-35)
    arm.set_position(*inclin_inicial, is_radian=False, wait=True, speed=ARM_SPEED*2, mvacc=ARM_ACCEL*2, radius=0.0)
    
    mover_en_circulos(arm=arm, chemex_coords=inclin_inicial, chemex_radius=25, interrupt_time=25, debug=debug)
    # enderezar y elevar tetera
    arm.set_position(*modificar_pos(pos_chemex, z=15), is_radian=False, wait=False, speed=ARM_SPEED*4, mvacc=ARM_ACCEL*3, radius=80)

    print_debug(f'saturar_cafe() - Café saturado', debug)

def filtrar_cafe(arm, debug):
    # verter agua sobre café en movimientos circulares - interrupción por tiempo
    print_debug(f'filtrar_cafe() - Inclinando tetera...', debug)
    inclin_inicial = modificar_pos(pos_chemex, x=12, y=0, z=-40, roll=-50) # MODIFICAR PARA AGUA RESTANTE
    arm.set_position(*inclin_inicial, is_radian=False, wait=True, speed=ARM_SPEED*2, mvacc=ARM_ACCEL*2, radius=0.0)
    mover_en_circulos(arm=arm, chemex_coords=inclin_inicial, chemex_radius=25, interrupt_time=25, debug=debug)
    # enderezar y elevar tetera
    arm.set_position(*modificar_pos(pos_chemex, z=15), is_radian=False, wait=False, speed=ARM_SPEED*4, mvacc=ARM_ACCEL*3, radius=80)
    # regresar tetera a posición inicial
    arm.set_position(*modificar_pos(pos_chemex, z=15), is_radian=False, wait=False, speed=ARM_SPEED*4, mvacc=ARM_ACCEL*3, radius=80)
    msg = cambiar_offset('default')
    print_debug(f'filtrar_cafe() - {msg}', debug)
    arm.set_position(*modificar_pos(pos_tetera, z=200), is_radian=False, wait=False, speed=ARM_SPEED*3, mvacc=ARM_ACCEL*2, radius=60)
    arm.set_position(*pos_tetera, is_radian=False, wait=True, speed=ARM_SPEED*3, mvacc=ARM_ACCEL*2, radius=0.0)
    time.sleep(2)
    msg = controlar_gripper(arm, ABRIR)
    print_debug(f'filtrar_cafe() - {msg}', debug)
    print("Alejándose de tetera...")
    arm.set_position(*modificar_pos(pos_tetera, y=70), is_radian=False, wait=True, speed=ARM_SPEED*3, mvacc=ARM_ACCEL*2, radius=30)
    msg = controlar_gripper(arm, CERRAR)
    print_debug(f'filtrar_cafe() - {msg}', debug)

    print_debug(f'filtrar_cafe() - Café saturado', debug)

def retirar_filtro(arm, debug):
    # sujetar filtro
    # retirar filtro de Chemex
    # regresar filtro a posición inicial
    # regresar brazo a posición neutral
    print_debug("retirar_filtro() - Filtro retirado", debug)

def servir_cafe(arm, debug):
    # sujetar, levantar y remover Chemex
    # servidor café en taza
    # regresar Chemex a balanza
    if debug: print("Café servido.")

def preparar_cafe(debug):
    ip_chemex = '192.168.1.196'
    arm_chemex = iniciar_robot(ip_chemex, 'arm_chemex', debug)
    ip_tetera = '192.168.1.203'
    arm_tetera = iniciar_robot(ip_tetera, 'arm_tetera', debug)

    print("Iniciando rutina...\n")
    print_debug('1. Preparación', debug)
    iniciar_arduino(arm_tetera, debug)
    dispensar_cafe(arm_tetera, debug)
    colocar_chemex_sobre_balanza(arm_chemex, debug)
    tarar_balanza(arm_tetera, debug)
    colocar_filtro_sobre_chemex(arm_tetera, debug)
    tarar_chemex(arm_tetera, debug)
    verter_cafe_sobre_filtro(arm_chemex, debug)
    pesar_cafe_vertido(arm_tetera, debug)

    print_debug('2. Filtrado de café', debug)
    saturar_cafe(arm_tetera, debug)
    time.sleep(60 * 0.5)
    filtrar_cafe(arm_tetera, debug)
    time.sleep(60 * 3)
    
    print_debug('3. Servido de café', debug)
    retirar_filtro(arm_tetera, debug)
    servir_cafe(arm_chemex, debug)
    
    print("Rutina finalizada.")

if __name__ == "__main__":
    preparar_cafe(debug=True)