import serial
import time

class Ledboard:
    def __init__(self,port,speed):
        self.ser = serial.Serial(port, speed, timeout=1)
        self.framebuffer = [0x00] * 90
        time.sleep(5)

    def writebuffer(self, data):
        if len(data) == 90:
            self.framebuffer = data
            self.draw()

    def drawstring(self, string, font):
        i = 0
        self.framebuffer = [0x00] * 90
        for char in string:
            self.framebuffer[i] = font[ord(char)-32][0]
            i += 1
            self.framebuffer[i] = font[ord(char)-32][1]
            i += 1
            self.framebuffer[i] = font[ord(char)-32][2]
            i += 1
            self.framebuffer[i] = font[ord(char)-32][3]
            i += 1 
            self.framebuffer[i] = font[ord(char)-32][4]
            i += 1 
        self.draw() 
 
    def draw(self):
        self.ser.write(chr(0x81))
        self.ser.write(chr(0x80))
        for frame in self.framebuffer:
            self.ser.write(chr(frame))

    def demo(self):
        for i in range(0,89):
            self.framebuffer[i]=i
        self.draw()
