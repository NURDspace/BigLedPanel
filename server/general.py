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
parser.add_option("", "--mpdport", default="6600", 
                  help="MPD port")
parser.add_option("", "--mpdhost", default="localhost",
                  help="MPD Host")
(options, args) = parser.parse_args()

if (options.port is None):
        sys.stderr.write("You must specify -p /dev/ttyS0 or --port /dev/ttyS0\n")
        sys.exit(1)

ledboard = Ledboard(options.port,options.speed)
_buffer = " " * 18

PASSWORD = False
CON_ID = {'host':options.mpdhost, 'port':options.mpdport}

def mpdConnect(client, con_id):
    """
    Simple wrapper to connect MPD.
    """
    try:
        client.connect(**con_id)
    except SocketError:
        return False
    return True 

client = MPDClient()
if mpdConnect(client, CON_ID):
    print 'Got connected!'
else:
    print 'fail to connect MPD server.'
    sys.exit(1)

while 1:
    _buffer = client.currentsong()['file']
    str_pad = (" " * (18-13)) + "Now playing: "
    my_long_string = _buffer 
    my_long_string = str_pad + my_long_string  
    for i in range (0, len(my_long_string)):  
        _buffer = my_long_string[i:(i+18)]  
        ledboard.drawstring(_buffer,font())
        time.sleep(.1)
    _buffer = "The time is: "
    ledboard.drawstring(_buffer,font())
    time.sleep(1.5)
    for i in range (0, 10):
        current = time.strftime("%H.%M.%S")
        _buffer = "%s%s%s"%(" " * 5,current," " * 5)
        ledboard.drawstring(_buffer,font())
        time.sleep(1)

    #temp
    j = urllib2.urlopen('http://space.nurdspace.nl/spaceapi/status.json')
    j_obj = json.load(j)
    _buffer="De temperatuur in de space is: %s" %(j_obj['sensors'][0]['temp']['space'])
    my_long_string = _buffer
    my_long_string = (" " * 18) + my_long_string
    for i in range (0, len(my_long_string)):
        _buffer = my_long_string[i:(i+18)]
        ledboard.drawstring(_buffer,font())
        time.sleep(.1)
