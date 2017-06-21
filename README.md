# README #

This is a Linux kernel driver module to be used to test High Resolution Timers precision.

### How do I could you it? ###

1 - Install kernel sources for your distribution.
2 - Customize values for START_TIME (nanoseconds), END_TIME, STEP_LOOPS and LOOP_MULTIPLIER.
3 - Compile by the command "make".
4 - In a secondary terminal window issue the command "make log-tail".
5 - In the primary terminal window issue the command "make install".
6 - Inspect the output at #4 to see how HR timer performs on your system
7 - In the primary terminal window issue the command "make uninstall".