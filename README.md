# Space shooter

I was tasked with implementing and creating the classic game "Spaceshooter". Fun exercise that demanded I'd be able to get an overview of an existing codebase in which I was to implement gamelogic and programdecisions based on what I learned in the course "systems programming".
The program runs on a RPi 5.

#### More technical, please.

This program was all about POSIX I/O and concurrency. The goal was to implement a game imitating the iconic "Spaceshooter" in a Raspberry Pi 5. The project was given as an assignment in the course "Systemprogramming". 

## My focus was on
- Working with multiple threads.
- Thread synchronization with mutex and conditional variables.
- GPIO access using POSIX I/O.

## What did I learn?
A lot about how data flow in Linux, threads and concurrency. Especially how Linux sees everything as a file: Even hardware. This meant, that in order to access hardware, I had to get a filedescriptor for the GPIO chip and further access each pin that I needed to use to control the ship and shoot.

Especially interesting was how I primarily used ioctl() to communicate with hardware. Open was used to gain access to the hardware and from here ioctl was used.

Moreover, threads were absolutely necessary for this game to function and was kind of an eye-opener. If threads weren't used, everything from button presses to enemy spawning would happen sequentially instead of concurrently. Sequential game logic would in this instance cause the game to feel unresponsive and blocking calls would freeze the whole game.