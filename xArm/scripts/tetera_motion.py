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

## START
arm.set_servo_angle(angle=[-90.0, 0.3, 1.1, 0.0, -1.4, 0.0], speed=ARM_SPEED, mvacc=ARM_ACCEL, wait=False, radius=0.0)
arm.set_servo_angle(angle=[-90.0, -36.8, -9.6, 0.0, 46.4, 0.0], speed=ARM_SPEED, mvacc=ARM_ACCEL, wait=False, radius=0.0)
arm.set_servo_angle(angle=[-49.5, 4.2, -20.0, -50.6, 100.2, 77.9], speed=ARM_SPEED, mvacc=ARM_ACCEL, wait=True, radius=0.0)
control_gripper(arm, 1)
time.sleep(1)
arm.set_servo_angle(angle=[-55.7, 9.3, -27.4, -57.0, 100.0, 74.9], speed=ARM_SPEED, mvacc=ARM_ACCEL, wait=True, radius=0.0)
time.sleep(1)
control_gripper(arm, 0)
time.sleep(1)
arm.set_servo_angle(angle=[-52.4, -17.7, -26.1, -60.9, 114.9, 52.8], speed=ARM_SPEED, mvacc=ARM_ACCEL, wait=True, radius=0.0)
