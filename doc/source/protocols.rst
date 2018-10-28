================
Scales Protocols
================

Quadrature
==========
Applies for Optical Digital scales as well as for some magnetic and Optical Analog scales.
In GDRO, the STM32 timer is configured to count up/down the quadrature signal and give the result directly into the timer register. Since it's handled in hardware, it allows to track very fast and precise moves without any loss of 'steps' (ie, more than 1m/s at 1µm resolution).

It is advised to use a differential pair connection (RS422 / RS485) to transmit the quadrature signal from the Scale to GDRO: since Quadrature Scales are solely incremental, glitches in the transmissions of the signal cause reading error that tend to accumulate over time. This is particularly true when using noisy power electronics around (Stepper motors, VFD, DC motors etc).


"Slave" protocols
==================
For some chinese digital Scales, Calipers and Gauges, the signal is generated from the Scale itself, making the Scale the Master driving the clock, and GDRO the Slave.
On these, you have no control over the refresh rate, as it is controlled by the Scale itself (with some exception)

"6x4" protocol
--------------
The data is transmitted, LSB first, on each clock rising edge, 4 bits at a time, for a total of 24 bits.
Between each transmissions, both the clock and the data lines are tied high.

Usually, the digital value transmitted is simply the displayed value in the displayed resolution.
For metric, the resulution can be 0.01 or 0.001mm; and in imperial it can be 0.001" or 0.00005" (maybe other configurations?).

 * Neoteck 0.001mm Gauge : metric resolution 0.0001mm, 0.00005" for imperial
 * Castorama Caliper : metric resolution 0.001mm, imperial ???

48 bits protocol
----------------
Originally documented by www.shumatech.com.
Two 24 bits words are transmitted, LSB first, transmitting the absolute, then the relative position. Clock is around 90kHz and refresh rate is around 3Hz.
Units are 20480 th of an inch (25.4/20480 = 0.001240234375mm)
GDRO only reads the first 24bits word, as it has it's own logic for absolute/incremental position.

'BCD' protocol
--------------
Documented both by shumatech and http://www.pcbheaven.com
The characters of the LCD display are simply sent as they are. The MSB holds info about the sign and metric/imperial units being used.


"Master" protocols
==================


iGaging 21 bits protocol
------------------------
It looks to be more recent flavour of scales, and it is also very economical.
Data is sent LSB first, on rising edge of the clok signal. GDRO then reads its value while sending the falling edge of the clock signal.
It seems like these Scales can only output 0.01mm resolution, but shumatech states a resolution of 2560 CPI
GDRO drives the clock at 10kHz, and updates the position at ~250Hz.
Also, it is powered from the 3.3V rail, directly.

These come in three categories:

'Master' 