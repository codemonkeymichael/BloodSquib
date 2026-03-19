# DMX Trigger Output #

Reads DMX frames from port A, channel 1-4 drives digital output 1-4

Build 

D:\repos\BloodSquib> cmake --build build

No problem at all—rest up! Troubleshooting custom timing on a PIO (Programmable I/O) can be a real headache.
Before you go, here is a quick recap of where we stand so you can jump back in tomorrow:

* The Problem: The DMX Core Pico 2 works with a cable but "ignores" the Donner wireless signal, even though your other lights respond to it.
* Failed Fixes: We tried lowering the "Break" detection threshold and the baud rate, but it didn't trigger the signal.
* The "Tomorrow" Plan: We should look at Signal Polarity and Electrical Levels. If the Donner receiver is outputting an inverted signal (Idling Low instead of High) or using 5V logic that the Pico 2 is struggling to read, that would explain why the software changes didn't help. [1] 

Quick check for tomorrow morning: When you have a chance, see if your Donner receiver has a polarity switch or if you have a multimeter to check the resting voltage on the data pin.
Talk tomorrow!

[1] [https://support.enttec.com](https://support.enttec.com/support/solutions/articles/101000396247-dmx-basics-troubleshooting)
