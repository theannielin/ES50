'''
Created on 2011-12-02

@author: Bobby Wood
'''

## import the serial library
import serial

## Boolean variable that will represent 
## whether or not the arduino is connected
connected = False

## open the serial port that your ardiono 
## is connected to.
# Serial port parameters
serial_speed = 115200
serial_port = '/dev/cu.usbmodem1411'

# Test with USB-Serial connection
# serial_port = '/dev/tty.usbmodem1421'

ser = serial.Serial(serial_port, serial_speed, timeout=1)

print ser.write('7')
if ser.read() == 1:
	print "got number back"

# ## loop until the arduino tells us it is ready
# while not connected:
#     serin = ser.read()
#     connected = True

# ## Tell the arduino to blink!
# ser.write("1")

# ## Wait until the arduino tells us it 
# ## is finished blinking
# while ser.read() == '1':
#     ser.read()
#     print "connected"

## close the port and end the program
# ser.close()