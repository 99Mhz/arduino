### Arduino language reference
https://www.arduino.cc/reference/en/ :sparkles:<br />

### ATmega328P Datasheet
 http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-42735-8-bit-AVR-Microcontroller-ATmega328-328P_Datasheet.pdf :octocat:<br />

### MsTimer2
http://playground.arduino.cc/Main/MsTimer2<br />

### Other great evaluation boards
https://www.pjrc.com/teensy/ :tada: <br />
https://www.adafruit.com/category/943<br />

### Alternate designs to manual multiplexing
http://www.holtek.com/productdetail/-/vg/16K33<br />
https://www.adafruit.com/product/880 :metal: <br />
https://www.maximintegrated.com/en/app-notes/index.mvp/id/3210<br />
https://www.sparkfun.com/datasheets/Components/General/COM-09622-MAX7219-MAX7221.pdf<br />
https://www.sparkfun.com/products/9622 :rocket:<br />

### Alternative Arduino IDE
https://atom.io/<br />
install as additional package:
https://platformio.org/<br />
get started video:
https://www.youtube.com/watch?v=EIkGTwLOD7o :metal:

### Bitwise cheat sheet
Set bit i in PORTB with OR
```C
PORTB |= (1 << i)
```
Toggle bit i in PORTB with XOR
```C
PORTB ^= (1 << i)
```
Clear bit i in PORTB with AND
```C
PORTB &= (1 << i)
```
Reverse bit i in PORTB with AND NOT
```C
PORTB &= ~(1 << i)
```
