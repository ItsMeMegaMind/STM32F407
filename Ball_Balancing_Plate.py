import cv2
import serial
import struct
import numpy as np
from decimal import *

Kp_X = 30.0/560.0
Ki_X = 0
Kd_X = 0

Kp_Y = 30.0/480.0
Ki_Y = 0
Kd_Y = 0

Error_X = 0
Error_Y = 0
Error_Sum_X = 0
Error_Sum_Y = 0
Error_Last_X = 0
Error_Last_Y = 0

Set_Point_X = 320
Set_Point_Y = 240

Center_X = 0
Center_Y = 0

Servo_X = 90
Servo_Y = 90

Capture = cv2.VideoCapture(1)
Blur_Kernel = np.ones((5,5),np.uint8)

STM32F4 = serial.Serial('COM3', 9600)

while(1):

    _, Frame = Capture.read()

    HSV = cv2.cvtColor(Frame, cv2.COLOR_BGR2HSV)

    Lower_Orange = np.array([4,100,100])
    Upper_Orange = np.array([24,255,255])

    Mask = cv2.inRange(HSV, Lower_Orange, Upper_Orange)
    Blur = cv2.medianBlur(Mask,5)
    Edges = cv2.Canny(Blur,150,200)

    Derp,Contours,Hierarchy = cv2.findContours(Edges, 1, 2)

    Moments = cv2.moments(Contours[0])

    if Moments['m00'] == 0:
        Center_X = int(Moments['m10']/0.0001)
        Center_Y = int(Moments['m01']/0.0001)

    else :
        Center_X = int(Moments['m10']/Moments['m00'])
        Center_Y = int(Moments['m01']/Moments['m00'])


    Error_X = Set_Point_X - Center_X
    Error_Y = Set_Point_Y - Center_Y

    Servo_X = Kp_X*Error_X
    Servo_Y = Kp_Y*Error_Y

    Servo_X = Servo_X + 90
    Servo_Y = Servo_Y + 90
    
    print int(Servo_X)

    STM32F4.write(struct.pack('B',int(Servo_X)))
    STM32F4.write(struct.pack('B',int(Servo_Y)))
    
    cv2.imshow('BLUR',Edges)

    k = cv2.waitKey(100) & 0xFF
    if k == 27 :
        break


cv2.destroyAllWindows()
