import os
import sys
import time
import math
import numpy
sys.path.append(os.path.join(os.path.dirname(__file__), '..'))
from xarm.wrapper import XArmAPI

from scripts.barista_move import *

ip = "192.168.1.203"
arm = XArmAPI(ip)
arm.motion_enable(enable=True)
arm.reset(wait=True)
arm.set_mode(0)
arm.set_state(state=0)
arm.reset(wait=False)
arm.move_gohome()

def wait_for_enter():
    while True:
        input("Press Enter to continue...")
        break

ARM_SPEED = 12
## TETERA
# wait_for_enter()
# 1) orientar a tetera
# 1a) elevar y rotar 45°
arm.set_servo_angle(angle=[-40.0, -48.9, -5.3, 0.0, 54.2, 0.0], speed=ARM_SPEED, mvacc=ARM_ACCEL, wait=False, radius=0.0)
# 1b) rotar otros 45°
arm.set_servo_angle(angle=[-90.0, -54.9, -2.8, 0.0, 57.7, 0.0], speed=ARM_SPEED, mvacc=ARM_ACCEL, wait=False, radius=0.0)
# 1c) orientar gripper
arm.set_servo_angle(angle=[-47.0, -18.0, -18.8, -53.3, 114.2, 61.2], speed=ARM_SPEED, mvacc=ARM_ACCEL, wait=False, radius=0.0)
# 1c) descender
arm.set_servo_angle(angle=[-51.2, 13.6, -22.9, -51.6, 95.8, 82.7], speed=ARM_SPEED, mvacc=ARM_ACCEL, wait=False, radius=0.0)

# 2) acercar a tetera + abrir gripper
# wait_for_enter()
control_gripper(arm, 1)
arm.set_servo_angle(angle=[-56.9, 17.6, -30.2, -57.6, 96.9, 79.4], speed=ARM_SPEED, mvacc=ARM_ACCEL, wait=True, radius=0.0)
time.sleep(1)
# 3) cerrar gripper + levantar tetera
# wait_for_enter()
control_gripper(arm, 0)
time.sleep(1)
arm.set_servo_angle(angle=[-52.2, -27.5, -31.1, -63.5, 118.4, 39.9], speed=ARM_SPEED, mvacc=ARM_ACCEL, wait=True, radius=0.0)

## CHEMEX
# 1) orientar a Chemex
arm.set_servo_angle(angle=[-3.0, 10.3, -74.6, -5.8, 149.2, 84.8], speed=ARM_SPEED, mvacc=ARM_ACCEL, wait=True, radius=0.0)
# 2) inclinar tetera
arm.set_servo_angle(angle=[-2.1, 18.8, -78.4, -10.6, 169.4, 80.3], speed=ARM_SPEED, mvacc=ARM_ACCEL, wait=True, radius=0.0)
