# barista_move.py

"""
Variables
"""
## BALANZA 
# pinout
DIN1_BAL = 9      # DI1
DOUT4_ENABLE = 13  # DO4
DOUT5_TARE = 12   # DO5
# estados (invertidos)
HIGH = 0
LOW = 1

## xARM
ARM_ACCEL = 200
ARM_SPEED = 20
# estados
ST_ON = 0
ST_TARE = 1
ST_CHMX = 2
ST_CAFE = 3
ST_AFLORAM = 4
ST_BLOOM = 5
ST_OFF = 6

estado = 0
"""
Funciones
"""
## GPIO
def joint_motion_cont(arm, angle=None, speed=ARM_SPEED):
    arm.set_servo_angle(angle=angle, speed=speed, mvacc=ARM_ACCEL, wait=False, radius=0.0)

def get_digio_value(arm, pin=None):
    all_gpio = arm.get_cgpio_digital()[1]
    if pin is None:
        return all_gpio
    return all_gpio[pin]

def control_gripper(arm, comm):
    # 0 = cerrar / 1 = abrir
    msg = "cerrar" if comm == 0 else "abrir"
    arm.set_tgpio_digital(ionum=0, value=comm, delay_sec=0)
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
