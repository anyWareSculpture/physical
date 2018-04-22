#!/usr/bin/python

# Import the modules to send commands to the system and access GPIO pins
import RPi.GPIO as gpio
import syslog
import os
import sys
import signal
from time import sleep

def shutdown():
    # Shutdown && log
    syslog.syslog('pishutdown: Soft shutdown')
    gpio.cleanup()
    os.system('shutdown now -h')
    sys.exit(0)

def reboot():
    # Reboot && log
    syslog.syslog('pishutdown: Soft reboot')
    gpio.cleanup()
    os.system('reboot')
    sys.exit(0)

def cleanup(signum, frame):
   syslog.syslog('pishutdown: Caught a signal, shutting down')
   gpio.cleanup()
   sys.exit(1)

def wait_for(value, timeout=-1):
  while True:
    channel = gpio.wait_for_edge(40, gpio.RISING if value else gpio.FALLING, timeout=timeout)
    if channel == None: return None
    # Debounce
    for i in range(10):
      sleep(0.001)
      curr_value = gpio.input(40) == gpio.HIGH
      if curr_value != value: break
    if i == 9: return channel
      
   
signal.signal(signal.SIGINT, cleanup)

#Set pin numbering to board numbering
gpio.setmode(gpio.BOARD)

gpio.setup(38, gpio.OUT)
gpio.output(38, gpio.HIGH)
# Enable pull-down for safety
gpio.setup(40, gpio.IN, pull_up_down=gpio.PUD_DOWN)
try:
    wait_for(True)
    if wait_for(False, 500) != None:
       if wait_for(True, 500) != None:
          reboot()
    shutdown()
except KeyboardInterrupt:  
  print 'Keyboard interrupt'
  gpio.cleanup()
  sys.exit(1)    
except Exception as e:
  syslog.syslog('pishutdown: Exception: ' + str(e))

