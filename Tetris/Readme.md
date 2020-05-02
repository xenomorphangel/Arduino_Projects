# Tetris Project 

This project came about as a university group project (2 students) for a course on Microcontrollers. We were allowed to choose our own projects and hardware required to build this hand-held portable Tetris game.

Main Hardware components:

- 2x 8x8 LED Matrix display (common cathode)
- 1x 4 digit 7-segment display (common anode)
- 3x MAX7219 driver chips
- 4x Normally Opened (NO) buttons
- 1x Arduino Nano

The Tetris Game was adapted from:

* [Jae Yeong Bae](jocker.tistory.com)  who is the original developer of the Tetris game logic for the Arduino.
* [Anne Jan Brouwer](https://github.com/IJHack/tetris) who adapted Jae Yeong Bae's codes for to be able to use the MAX7219 driver chip
* [Eberhard Fahle](https://www.arduinolibraries.info/libraries/led-control) who is a contributor for Arduino Library LedControl for the MAX7219 and the MAX7221 driver chips.

Further editions were made by me to adapt the codes and drivers to fit the additional components and differences in LED polarity.

