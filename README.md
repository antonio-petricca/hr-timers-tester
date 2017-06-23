# README #

This is a Linux kernel driver module to be used to test High Resolution Timers precision.

### How do I could you it? ###

 - Compile by the command "make".
 - In a secondary terminal window issue the command "make log-tail".
 - In the primary terminal window issue the command "make install".
 - Inspect the output got by the kernel log tail to see how HR timer performs on your system.
 - In the primary terminal window issue the command "make uninstall".
