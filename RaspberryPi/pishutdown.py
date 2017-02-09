#!/usr/bin/python

# Import the modules to send commands to the system and access GPIO pins
import RPi.GPIO as gpio
import syslog
import os
import sys
import signal

def cleanup(signum, frame):
   print 'pishutdown: Caught a signal, shutting down.\n'
   gpio.cleanup()
   sys.exit(1)

signal.signal(signal.SIGINT, cleanup)

#Set pin numbering to board numbering
gpio.setmode(gpio.BOARD)

gpio.setup(38, gpio.OUT)
gpio.output(38, gpio.HIGH)
gpio.setup(40, gpio.IN)

while True:
  try:
    gpio.wait_for_edge(40, gpio.RISING)
    # Shutdown && log
    syslog.syslog('pishutdown: Soft shutdown');
    gpio.cleanup();
    os.system('shutdown now -h')
  except KeyboardInterrupt:  
    print 'Keyboard interrupt'
    gpio.cleanup();
    sys.exit(1)    
  except:
    syslog.syslog('pishutdown: Exception');

