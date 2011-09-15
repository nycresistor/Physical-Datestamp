#!/usr/bin/python

import serial
import re
import time
import sys
import simplejson
import popen2
import socket

def msleep(millis):
    time.sleep(millis*0.001)


offTable = [
    310,
    400,
    490,
    588,
    678,
    762,
    860,
    958,
    1060,
    1150


]
# 8    1050
# 9    1150


class Stamper:
    def __init__(self):
        self.serialPort = None

    def say(self,message):
        self.serialPort.write(message + "\n\r")

    def open(self,portName,baud=38400):
        self.serialPort = serial.Serial(portName, baud, timeout=3.5)
        try:
            self.serialPort.open()
        except serial.SerialException, e:
            sys.stderr.write("Could not open serial port %s: %s\n" % (self.serialPort.portstr, e)
)
        time.sleep(1.1)
        data = self.serialPort.read(1)  # read one, blocking
        n = self.serialPort.inWaiting() # look if there is more
        if n:
            data = data + self.serialPort.read(n)
        print "DATA: ", data

    def raiseHead(self):
        "Raise the printhead."
        self.say("T100");

    def lowerHead(self):
        "Lower the printhead."
        self.say("T1000");

    def setDigit(self,digit):
        "Turn the printhead to the given digit."
        if digit > 9 or digit < 0:
            print "ERROR: digit "+str(digit)+" out of range!"
        self.say("N"+str(offTable[digit]))

    def setPosition(self,pos):
        "Move the head horizontally to the given position."
        steps = pos * 210
        if pos > 9 or pos < 0:
            print "ERROR: position "+str(pos)+" out of range!"
        self.say("S"+str(steps))
    
    def setInkPosition(self):
        "Move the head over the inkpad"
        self.say("S2600")
        
    def home(self):
        "Home the head."
        self.say("H")
    
    def startSequence(self):
        "Startup sequence of motions."
        self.raiseHead()
        msleep(200)
        self.home()
        msleep(1500)

    def stampDigit(self,digit,pos):
        "Stamp the given number at the specified position."
        self.raiseHead()
        msleep(600)
        # DELAYS BUILT IN TO CALLS?
        self.setInkPosition()
        msleep(1500)
        self.setDigit(digit)
        msleep(200)
        self.lowerHead()
        msleep(800)
        self.raiseHead()
        msleep(800)
        self.setPosition(pos)
        msleep(1500)
        self.lowerHead()
        msleep(800)
        self.raiseHead()

s = Stamper()
s.open(sys.argv[1])
s.startSequence()
t = int(time.time())
d = 9
while t >= 1:
    s.stampDigit(t%10,d)
    d = d - 1
    t = t / 10

