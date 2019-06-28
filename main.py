import pygame
import serial
import struct
import threading
import time
import sys
#import socket

# pygame.init()
pygame.joystick.init()
pygame.font.init()
clock = pygame.time.Clock()

# UDO Setup
UDP_IP = "192.168.1.5"
UDP_PORT = 6969

#sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# Screen setup
size = 800, 480
screen = pygame.display.set_mode(size)
pygame.display.set_caption("Joystic")
run = True

# Joystick setup
joysticks = [pygame.joystick.Joystick(x)
             for x in range(pygame.joystick.get_count())]

joystick = joysticks[0]
joystick.init()

throttle = joysticks[1]
throttle.init()

print("Joystick :" + joystick.get_name() + "Throttle :" + throttle.get_name())

# Font Setup
font = pygame.font.SysFont("monospace", 18)

last_position_x = 0.00000
last_position_y = 0.00000
last_position_throttle = 0.00000

# Serial COM Setup
arduino = serial.Serial('com4', baudrate=9600)


def readSerial():
    global run
    lock = threading.Lock()

    lock.acquire()
    print("Escuchando puerto serial")
    lock.release()

    while run:
        reading = arduino.read().decode('utf-8')
        lock.acquire()
        print(reading)
        lock.release()


#thread = threading.Thread(target=readSerial)
#thread.start()


def close_read_serial():
    print("Closing Serial Connection")
    arduino.close()
    thread.join(0)


def main_loop():
    global run, last_position_x, last_position_y, last_position_throttle

    while run:
        for event in pygame.event.get():

            if event.type == pygame.QUIT:
                run = False
                close_read_serial()
                joystick.quit()
                throttle.quit()
                pygame.quit()

            x = round(joystick.get_axis(0) * -1, 5)
            y = round(joystick.get_axis(1) * -1, 5)
            z = round(throttle.get_axis(2) * -1, 5)

            if(x != last_position_x or y != last_position_y or last_position_throttle != z):
                last_position_x = x
                last_position_y = y
                last_position_throttle = z

                x_data = ((((1 + last_position_x) / 2 - 0) * (485 - 110)) / (1 - 0)) + 110
                y_data = ((((1 + last_position_y) / 2 - 0) * (180 - 0)) / (1 - 0)) + 0
                power = 143 + (440 * (1 + last_position_throttle)) + 1

                if power < 150:
                    power = 0

                data = ('H' + str(power) + '|' + str(x_data) + '|' +
                        str(y_data) + '|' + 'E').encode('utf-8')
                #sock.sendto(data, (UDP_IP, UDP_PORT))
                arduino.write(data)
                print(data)

                screen.fill(0)
                label = font.render(
                    "X: " + str(last_position_x) + " Y: " + str(y_data) + " Z: " + str(last_position_throttle), 1, (255, 250, 50))
                screen.blit(label, (65, 100))

        fps = font.render(str(int(clock.get_fps())),
                          True, pygame.Color('white'))
        screen.blit(fps, (50, 50))
        pygame.display.update()
        clock.tick(30)


main_loop()
