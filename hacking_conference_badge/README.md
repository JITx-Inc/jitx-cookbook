# Hacking Conference Badge

This recipe serves as a playground for you to learn JITX while working on a functional PCB design. You’ll use the electronic conference badge template design in this folder to design your own electronic badge.

This template is a fully functioning board with an RP2040 microcontroller, battery and USB power solution, and a number of peripheral input/output devices that you can use as part of your badge design. We can roughly separate the template badge into 2 parts - the subsystems that you won’t change, and those that you (likely) will change.

##### Won't Change

- MCU (RP2040)
- MCU supporting circuitry (flash, crystal, etc.)
- power system (USB, battery charger, battery port, power switch)

##### Will Change

- RGB LED array
- solid color LED array
- potentiometers (dials/knobs)
- key-matrix array (buttons)
- Speaker
- TOF sensor (distance sensor)
- WS2816 array (addressable RGB LEDs)

### How The Badge Works

At this point, you should have forked/cloned this repo board, updated/inited submodules, and opened this folder/project in JITX VSCode. Make sure you're running the latest JITX version in VSCode.

The main design of the board is in `main.stanza`, open that in JITX VSCode and press "Ctrl+Enter" to build the board.

##### The Code

Briefly, let's go through the code.

The top level module is `public pcb-module hacking_conference_badge_module :`

Here, we instantiate and connect everything that will be part of our board.

`usb` - the USB connector
`battery` - the JST connector for the LiPo battery
`sw` - the power switch in series with the BQ2407 battery charger
`ld` - the LDO power/voltage regulator that converts voltage from the battery charger output to 2.8V for the RP2040 and other peripherals to run off of
`rpi` - the RP2040 microcontroller -> the brains/compute of the board
`led-drive` - the LED driver -> this accepts commands over I2C and turns the attached LEDs on/off. Notice that the RGB LEDs on the top left of the board are connected to this with 3 connections each - that is 1 for Red, 1 for green, and 1 for blue
`led-driver-shifter` - logic level shifter for the LED-driver. The LED-driver logic is running at its input power (~4.2V) whereas the RP2040 logic is running at 2.8V. This driver shifter shifts between those voltage levels so the LED-driver and RP2040 can talk to each other
`en-sw` - there are two of these which turn on and off a bank of single-color LEDs
`rgb-grid` - grid of RGB LEDs connected to the LED-driver
`amp` - this is the audio amplifier and speaker submodule
`knobs` - knobs/dials/potentiometers you can read directly with the RP2040 and use to do arbitrary things
`tof` - time of flight distance sensor, talks to RP2040 with I2C
`indicator-led-array` - array of WS2816 RGB addressable LEDs
`geom(NET)` - these are ground and power plane/pours
`power-net-class` - setting the trace width for various nets in the design
`run-design` - this compiles the board, or it runs checks on the board (depending on arguments)

##### Happy Hacking!
