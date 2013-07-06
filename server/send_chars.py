import serial
import sys
import termios, fcntl, sys, os, select
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
fd = sys.stdin.fileno()

oldterm = termios.tcgetattr(fd)
newattr = oldterm[:]
newattr[3] = newattr[3] & ~termios.ICANON & ~termios.ECHO
termios.tcsetattr(fd, termios.TCSANOW, newattr)

oldflags = fcntl.fcntl(fd, fcntl.F_GETFL)
fcntl.fcntl(fd, fcntl.F_SETFL, oldflags | os.O_NONBLOCK)

try:
    while 1:
        r, w, e = select.select([fd], [], [])
        if r:
            c = sys.stdin.read(1)
            _buffer = _buffer[1:] + c
            ledboard.drawstring(_buffer,font())
finally:
    termios.tcsetattr(fd, termios.TCSAFLUSH, oldterm)
    fcntl.fcntl(fd, fcntl.F_SETFL, oldflags)
