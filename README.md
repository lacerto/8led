# 8led

This little program controls a configurable amount of LEDs using a Raspberry Pi.
Basically it is a pimped up version of the C code provided for Sunfounder's Super Kit V2.0 for RaspberryPi [Lesson 3 Flowing LED Lights](http://www.sunfounder.com/learn/Super_Kit_V2_for_RaspberryPi/lesson-3-flowing-led-lights-super-kit-for-raspberrypi.html).

Tested with a Raspberry Pi 3 and Raspbian.

## Setup

Connect the circuit as shown on the [breadboard diagram](breadboard_diagram/8Led_breadboard.png). See also [Lesson 3](http://www.sunfounder.com/learn/Super_Kit_V2_for_RaspberryPi/lesson-3-flowing-led-lights-super-kit-for-raspberrypi.html) on Sunfounder's home page.

## Usage

To compile the program you will need the [wiringPi](http://wiringpi.com/) library which comes pre-installed if you are using Raspbian.

You can compile either by using the Code::Blocks IDE or using the Makefile provided.
* Code::Blocks

  Open the project file in Code::Blocs and press the Build button on the toolbar.
* Makefile

  Run the following command in a terminal:
  
  `make`

Either way you should now have an executable file called `8led`. It is located either in the root dierctory of the project if you compiled using the Makefile or in the bin/Debug or bin/Release directory if you are using Code::Blocks.

Run the code by typing:

  `sudo ./8led`

Running directly out of Code::Blocks is tricky as wiringPi requires root permissions.

The program has three modes:

1. Binary counter: the LEDs represent a binary up counter.
2. Flowing lights: the LEDs light up in turn from left to right and vice versa.
3. Breathing LED: gradually changes the brightness of a single LED.

For the first two modes you can specify the delay between each LED lighting up.
The third mode is actually [Lesson 4](http://www.sunfounder.com/learn/Super_Kit_V2_for_RaspberryPi/lesson-4-breathing-led-super-kit-for-raspberrypi.html) from Sunfounder. It demonstrates the use of the hardware PWM feature. It only uses one LED as the RPi has hardware PWM output only on Pin 1.

## Customizing

The first thing defined in the main function is an array containing the pin numbers the LEDs are connected to.

You can look up the pin numbers using the following command:

  `gpio readall`
  
This program uses the wiringPi pin numbers that can be found in the wPi columns of the table output by the previous command.

Not only 8 LEDs can be controlled if you modify `PIN_NUMBER` and the `pins` array. Of course the circuit must also be set up accordingly.

