# barista_move.py

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
ARM_SPEED = 20
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

## GPIO
def joint_motion_cont(arm, angle=None, speed=ARM_SPEED):
    arm.set_servo_angle(angle=angle, speed=speed, mvacc=ARM_ACCEL, wait=False, radius=0.0)

def get_gpio_value(arm, pin=None, output=False):
    code, states = arm.get_cgpio_state()
    idx = 5 if output else 4
    dig_states = [states[idx] >> i & 0x0001 for i in range(16)]
    
    return dig_states if pin is None else dig_states[pin]

def control_gripper(arm, comm):
    # 0 = cerrar / 1 = abrir
    msg = "cerrar" if comm == 0 else "abrir"
    arm.set_tgpio_digital(ionum=0, value=comm)
    return print(f'Gripper = {comm} ({msg})')

## TÉCNICA
def segmentar_circulo(radio, centro, num_seg):
    """
    Dividir movimiento circular.
    
    Args:
        radio (float): radio de la circunferencia
        centro (tuple): coordenadas (x, y) del centro de la circunferencia
        num_seg (int): n° deseado de segmentos de la circunferencia

    Returns:
        positions ()
    """
    centro_x, centro_y = centro
    positions = []
    for i in range(num_seg):
        angle = (2 * math.pi / num_seg) * i
        x = centro_x + radio * math.cos(angle)
        y = centro_y + radio * math.sin(angle)
        positions.append((x, y))
    return positions




# arm_speed = 60
# vueltas = 2
# r_chemex = 80
# pos_init = 350.0

# positions = [
#     [pos_init, 0.0, 200.0, -90.0, 0.0, 0.0],
#     [pos_init + r_chemex, r_chemex, 200.0, -90.0, 0.0, 0.0],
#     [pos_init + r_chemex, -r_chemex, 200.0, -90.0, 0.0, 0.0],
#     [pos_init + 2*r_chemex, 0.0, 200.0, -90.0, 0.0, 0.0]
# ]
# inc = 0
# while inc < 20:
#     idx = inc % 4
#     circ_coords = [positions[idx], positions[(idx + 1) % 4], positions[idx-1]]
#     print(circ_coords)
#     inc += 1
