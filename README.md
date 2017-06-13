Advent Mood Ring
================

The [Advent School](http://adventschool.org/) is a small independent PreK-6th grade school in downtown Boston. Our graduates go onto many
different middle schools across the Boston area and sometimes even further away, but they remember Advent forever and they all leave their mark on the school and future graduates.

This year, we decided to build some electronic art that's true to our mission and represents that link between excitement for the future and
recognition of the growth at Advent. Each student has taken home a small "internet enabled" knob with a single color-changing light. At Advent, we have installed an mixed-media painting that contains 23 lights integrated into a framed painting which mirror the color motions of the graduates' devices. The current system allows a simple
color choice that is faded in and out over time on the master painting, but the system is capable of representing color patterns, color wipes and other expressions of color.

How To Setup a "Mood Ring"
==========================

The student devices consist of an [ESP8266 dev board](http://www.gearbest.com/transmitters-receivers-module/pp_366523.html), a [Neopixel](https://www.adafruit.com/?q=neopixel&gclid=CK_QwaCHu9QCFc1LDQod08wDfA) and a [simple potentiometer](https://www.adafruit.com/product/562) with a [knob](https://www.adafruit.com/product/2047). Total cost is around $10 if you're willing to solder, or a little more if you want jumper wires. Note that I bought the ESP boards with headers, which made soldering a pain but made it easy to attach jumpers.

Each unit is powered with a MicroUSB cable which should be connected to a transformer, battery pack, or computer. I didn't include
these in the cost above because if you're reading this article you probably have 10 around the house.

When you plugin a programmed Mood Ring, the light should glow a white-ish color. if it can't find a saved WiFi network it will advertise itself as an access point called "AdventMoodRing". You should connect to this network with a phone or computer and it should present a "configure wifi screen" like the left screen below. Click on "Configure WiFi" and you'll see a screen like the one on the right (without the lower three fields) where you can choose your WiFi network and enter the WiFi password.

<center>
  <img alt="ESP8266 WiFi Captive Portal Homepage" src="http://i.imgur.com/YPvW9eql.png" width="300"/>
  <img alt="ESP8266 WiFi Captive Portal Configuration" src="http://i.imgur.com/oicWJ4gl.png" width="300"/>
</center>

After that's done, the microcrontroller will restart and the light will go off. Now, when you turn the knob it should change colors
in a spectrum that looks something like this:

![HCL color spectrum](https://raw.githubusercontent.com/d3/d3-interpolate/master/img/hclLong.png)

Whenever you change the color, the knob will send its state to the server and it will send down changes to the displays.

How To Wire the Mood Ring
=========================

You need six connections to make a Mood Ring work. The NeoPixel has three pins you need: G, + and "In". You will not use "O" for the mood ring - that would only be used if you had multiple lights. The potentiometer has three connections to be made to the microcontroller. Connect:

* The **OUTSIDE pins** of the potentiometer should connect to **3.3V** and **GND** of the microcontroller. Any pin labeled GND (there are multiple) will be fine.
* The **MIDDLE pin** of the potentiometer should connect to ADC of the microcontroller (it's on the top right when looking at the underside of the microcontroller)
* The + pin of the NeoPixel should connect to **VIN** of the microcontroller (on the bottom right of the microcontroller)
* The G pin of the NeoPixel should connect to **GND** of the microcontroller. Any pin labeled GND (there are multiple) will be fine.
* The "In" pin of the NeoPixel should connect to **GPIO 12** on the microcontroller. On some versions of the ESP8266 this may be labeled **D6** or **HMISO**

Writing Your Own Firmware
=========================

All the software you need to build this system is available on GitHub (where this document lives too). You will need the [Arduino IDE](https://www.arduino.cc/en/Main/Software) and have it configured for the NodeMCU microcontroller. There are [various guides](http://www.instructables.com/id/Quick-Start-to-Nodemcu-ESP8266-on-Arduino-IDE/) to help you get setup. There are two big parts
of the device software: the [WifiManager](https://github.com/tzapu/WiFiManager) which manages the configuration and connection, and a [WebSockets](https://github.com/morrissinger/ESP8266-Websocket) connection to the
server bits. The goal was to put as much of the logic on the server, which we can change whenever, as opposed to in the hardware
which we would have to rewrite to each device whenever it changed.

More information to come. Enjoy!