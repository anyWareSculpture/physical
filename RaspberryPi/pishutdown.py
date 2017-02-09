#!/usr/bin/python

# Import the modules to send commands to the system and access GPIO pins
import RPi.GPIO as gpio
import syslog
import os
import sys
import signal
from time import sleep

def cleanup(signum, frame):
   syslog.syslog('pishutdown: Caught a signal, shutting down')
   gpio.cleanup()
   sys.exit(1)

signal.signal(signal.SIGINT, cleanup)

#Set pin numbering to board numbering
gpio.setmode(gpio.BOARD)

gpio.setup(38, gpio.OUT)
gpio.output(38, gpio.HIGH)
# Enable pull-down for safety
gpio.setup(40, gpio.IN, pull_up_down=gpio.PUD_DOWN)

while True:
  try:
    gpio.wait_for_edge(40, gpio.RISING)
    # Debounce
    for i in range(10):
	sleep(0.010)
	if not gpio.input(40): break
    if i == 9:
        # Shutdown && log
	syslog.syslog('pishutdown: Soft shutdown')
	gpio.cleanup()
    	os.system('shutdown now -h')
    	sys.exit(0)
  except KeyboardInterrupt:  
    print 'Keyboard interrupt'
    gpio.cleanup()
    sys.exit(1)    
  except Exception as e:
    syslog.syslog('pishutdown: Exception: ' + str(e))

