slavik smart socket
============

This is a IoT electricity socket for Slavik Smart Home based on ESP8266 (ARDUINO).  

## Functions are implemented:

* Connection and authorization with backend via WebSocket protocol, secured by JWT 
* Sending current status
* Getting Commands from backend
* Initial setup via wifi

## Components

* x1 - 5v power supply
* x1 - positive voltage regulator 3v3
* x1 - ESP8266
* x5 - relay "srd-05vdc-sl-c"
* x5 - npn transistor "SS8050"
* x5 - diode "1N4007"
* x5 - resistor 1 kOhm
* x5 - resistor 10 kOhm
* x5 - resistor 250 Ohm
* x5 - momentary button
* x5 - LED

## Сircuit diagram

In developing