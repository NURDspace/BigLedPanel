import serial
import sys
import termios, fcntl, sys, os, select
import time
from optparse import OptionParser
from lib.ledboard import Ledboard
from lib.font1 import font1 as font

parser = OptionParser()
parser.add_option("-p", "--port", 
                  help="Serial port to connect to")
parser.add_option("-s", "--speed", default=115200,
                  help="Serial port speed")
(options, args) = parser.parse_args()

if (options.port is None):
        sys.stderr.write("You must specify -p /dev/ttyS0 or --port /dev/ttyS0\n")
        sys.exit(1)

ledboard = Ledboard(options.port,options.speed)
_buffer = " " * 18 

while 1:
    if (int(time.strftime("%S")) % 2):
        current = time.strftime("%H:%M:%S")
    else:
        current = time.strftime("%H.%M.%S")
    _buffer = "%s%s%s"%(" " * 5,current," " * 5) 
    ledboard.drawstring(_buffer,font())
    time.sleep(1)
