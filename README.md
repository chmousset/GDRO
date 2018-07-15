# DRO Protocols
## Chinese linear scales
This uses SPI. The scale does not generate it's own clock, you have to generate it

## 'Castorama' caliper
Sends the data roughly 5 times per second, at 150 to 200kHz data rate.
Data bits are received LSB first. It sends 24 bits in 6 4-bits nibbles, LSB first.
MSB indicates the sign (bit 0), or mm/inch (bit 4)

# DRO connection
## Generic cable

Using a USB-type 4 wires / 2p 2c wire connect the 


(Back-facing female connector, clips up&down	)
| Function | Color  | Pin | Pin | Color | Function |
|----------|--------|-----|-----|-------|----------|
| 3V       | Red    | 1   | 3   | White | Clk      |
| 0V       | Black  | 2   | 4   | Green | Data     |


(Front-facing female connector)
| Function | Color  | Pin | Pin | Color | Function |
|----------|--------|-----|-----|-------|----------|
| 3V       | Red    | 3   | 1   | White | Clk      |
| 0V       | Black  | 4   | 2   | Green | Data     |

The pin1 marker are on the same plane, but obviously not at the same location.

## STM32F429-DISCO pinout

| Function | STM32 Pin |
|----------|-----------|
| CLK X    | PB4       |
| DATA X   | PB7       |
| CLK Y    | PA7       |
| DATA Y   | PA5       |
| CLK Z    | PC3       |
| DATA Z   | PC8       |
| free     | PC11      |
| free     | PC12      |
| free     | PC13      |
| free     | PC14      |
| free     | PC15      |
