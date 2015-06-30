# vd5m_failsafe
a simple avr attiny based addon to add a failsafe function to a vd5m receiver

# NOTE

avr gcc support for the tiny10 is not in the original gcc repositories.
you will have to download it here:
http://www.atmel.com/tools/atmelavrtoolchainforlinux.aspx

extract it to a folder and set the variable AVRGCC_TOOLCHAIN in the Makefile to your path.

Alternatively you can simply flash my compiled main.hex file.

# CONNECTIONS


```
ATTINY pin 1 (PB0) ---> VD5M jtag header pin ? (reset)
ATTINY pin 3 (PB1) ---> VD5M green led signal before resistor (todo: which R? add pic)
ATTINY pin 2 (GND) ---> VD5M jtag header pin ? (gnd)
ATTINY pin 5 (VCC) ---> VD5M jtag header pin ? (3.3V)
ATTINY pin 4 (PB2) ---> piezo buzzer (second pin to gnd) [optional, beeps on signal lost]
```

# Programming

NOTE: the tiny can only be programmed at VCC=5V ! program the device before mounting it on the vd5m.

# how does it work?

Unfortuantely the VD5M lacks failsafe support. This device adds this feature
in a convenient and minimal way.

I use this receiver with a tiny 10cm Quadcopter in combination with a CC3D atom.
The described circuit monitors the green packet rx led on the VD5M, if there is
not activity for 1.5s the attiny resets the VD5M.

During reset state, before receiving any packets the VD5M does not output any
PPM signals. This is detected as "no signal" by the cc3d and the motors are switched off.

* CAUTION:
this does not set/send any given configurable throttle position, this addon
simply causes the VD5M to outpu _NO_ signal at all during connection loss for >1.5s.
Be sure to test if your controler can cope with this (props off!).

* CAUTION2:
Be aware that the Openpilot software 15.02 had a bug that could not 
detect "no signal" on pwm receivers. Please use at least 15.05



