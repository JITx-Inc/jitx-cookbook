# Design Notes for USB-C Cable Tester

Working notes for the design of the USB-C cable tester board.

## Function

We want to create a board that can test to make sure that every single connection in a USB-C cable is working well.

Sometimes a USB-C cable powers your device but flashing and data transfer fails because the USB cable is disfunctional. This is a waste of time, not to mention very annoying, because it takes some time to realize that it's the cable at fault, and not your device or design.

This board fixes that issue by checking every connection in a USB cable.

Importantly, it also breaks out every single connection into a test pad, so you can manually test yourself for continuity, voltage, signal, etc.


## Approach

Since we're making a status LED for every single pin, let's write a generator that takes in a list of pins and generates all of those status LEDs for us.

Since we want test points on every pin, let's add test points while we generate the LEDs.

Since we want the names of our test points to match the net they're attached to, and we want this to work generically, let's write a function which introspects our design, finds all of the testpoints, and sets their value to the net they're connected to.
