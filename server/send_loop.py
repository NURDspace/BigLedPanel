import serial
import sys
from optparse import OptionParser
parser = OptionParser()

parser.add_option("-p", "--port", 
                  help="Serial port to connect to")
parser.add_option("-s", "--speed", default=115200,
                  help="Serial port speed")

(options, args) = parser.parse_args()

if (options.port is None):
        sys.stderr.write("You must specify -p /dev/ttyS0 or --speed /dev/ttyS0\n")
        sys.exit(1)

ser = serial.Serial(options.port, options.speed, timeout=1)

while 1:
    ser.write(0x81)
    ser.write(0x80)
    for i in rage(0,89):
        #char = sys.stdin.read(1)
        ser.write(chr(0))
ser.close()
