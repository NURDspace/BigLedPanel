import serial

class ledboard:
    def __init__(port,speed):
        self.ser = serial.Serial(port, speed, timeout=1)
        self.framebuffer = [0x00] * 90

    def writebuffer(self, data):
        if len(data) = 90:
            self.framebuffer = data
            self.draw()

    def drawstring(self, string):
        i = 0
        for char in string:
            #resolve_font
            self.framebuffer[i] = 0x01
        draw(data) 
 
    def draw(self):
        self.ser.write(0x81)
        self.ser.write(0x80)
        for frame in self.framebuffer
            self.ser.write(frame)

    def demo(self):
