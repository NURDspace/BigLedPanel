import serial
import sys
import termios, fcntl, sys, os, select
import time
import urllib2
import json
from optparse import OptionParser
from lib.ledboard import Ledboard
from lib.font1 import font1 as font
from mpd import MPDClient, CommandError
from socket import error as SocketError


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

last_b_usd = 0
last_l_usd = 0

while 1:
    #Koers
    _b_usd = urllib2.urlopen('https://btc-e.com/api/2/btc_usd/ticker')
    _l_usd = urllib2.urlopen('https://btc-e.com/api/2/ltc_usd/ticker')
    b_usd = json.load(_b_usd)
    l_usd = json.load(_l_usd)

    if last_b_usd < b_usd['ticker']['last']:
        updwn_b_usd = chr(0x80) 
    else:
        updwn_b_usd = chr(0x81)

    if last_l_usd < l_usd['ticker']['last']:
        updwn_l_usd = chr(0x80)
    else:
        updwn_l_usd = chr(0x81)

    last_b_usd = b_usd['ticker']['last']
    last_l_usd = l_usd['ticker']['last']

    _buffer="B/U %s %s | L/U %s %s" % (b_usd['ticker']['last'],updwn_b_usd
        ,l_usd['ticker']['last'],updwn_l_usd) 
    my_long_string = (" " * 18) + _buffer 

    for i in range (0, len(my_long_string)):
        _buffer = my_long_string[i:(i+18)]
        ledboard.drawstring(_buffer,font())
