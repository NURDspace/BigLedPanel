#!/usr/bin/python
import serial
import sys
import termios, fcntl, sys, os, select, string
import time
import urllib2
import json
from optparse import OptionParser
from lib.ledboard import Ledboard
from lib.font1 import font1 as font
from socket import error as SocketError
from mpd import MPDClient, CommandError
from socket import error as SocketError

from twisted.protocols.basic import LineReceiver
from twisted.internet.protocol import Factory
from twisted.internet.task import LoopingCall
from twisted.internet import reactor

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

#CON_ID = {'host':options.mpdhost, 'port':options.mpdport}
ledboard = Ledboard(options.port,options.speed)

def sendLine(line,speed=0.25,hold=0):
    buffer = " " * 18
    if len(line) <= len(buffer):
        buffer=string.center(line,len(buffer))
        ledboard.drawstring(buffer,font())
    else:
        buffer=line[:len(buffer)]
        ledboard.drawstring(buffer,font())
        time.sleep(speed)
        for x in line[len(buffer):]:
            buffer = buffer[1:]+x
            ledboard.drawstring(buffer,font())
            time.sleep(speed)
    time.sleep(hold)

def showTime():
    sendLine(time.strftime("%H.%M.%S"),0.33)

def showMPD():
    client = MPDClient()
    try:
        client.connect(options.mpdhost,options.mpdport)
    except SocketError:
        print 'fail to connect MPD server.'
        return
    try:
        try:
            song=client.currentsong()['title']
        except:
            song=client.currentsong()['file']
        if song != '?':
            try:
                song=client.currentsong()['artist']+': '+song
            except:
                pass
        else:
            song=client.currentsong()['file']
        sendLine("Now playing: "+song,hold=1)
    except:
        pass
#        sendLine("Not Playing.")

def showBitcoin():
  #Koers
  _b_usd = urllib2.urlopen('https://btc-e.com/api/2/btc_usd/ticker')
  _l_usd = urllib2.urlopen('https://btc-e.com/api/2/ltc_usd/ticker')
  b_usd = json.load(_b_usd)
  l_usd = json.load(_l_usd)

  value="B/U %s | L/U %s" % (b_usd['ticker']['last'],l_usd['ticker']['last'])
  sendLine(value,speed=0.01,hold=1)

class GetLine(LineReceiver):
    delimiter='\n'

    def connectionLost(self,reason):
        self.lineReceived(self._LineReceiver__buffer)

    def lineReceived(self, line):
        line = line.strip('\r')
        speed=0.1
        if len(line)>50:
            speed=0.33
        sendLine(line,speed,hold=1)

class LineFact(Factory):
    maxDelay=30

    def buildProtocol(self, addr):
        return GetLine()

timr=LoopingCall(showTime)
timr.start(0.333, now=True)
mpdr=LoopingCall(showMPD)
mpdr.start(60, now=True)
bitr=LoopingCall(showBitcoin)
bitr.start(91, now=True)
kilr=LoopingCall(ledboard.disconnect)
kilr.start(360)
reactor.listenTCP(55555, LineFact())
reactor.run()
