====
GDRO
====

What?
=====
GDRO stands for Graphical Digital ReadOut. It's a display for digital Scales for machine tools

Why?
====
I have some conventional machines in my garage and I'm too lazy to count wheel turns, and I hate trying to squeeze my head around to read my gauge's reading.
Also, the mill/lathe I use does not have a threading appartus, so I had to find a solution (which indeed involves servomotors)

I did not find anything that was really what I wanted. Of course there is commercial solutions such as Shumatec's or touchDRO but neither did really sound appealing to me as neither were really customizable (like: act as a GUI for CAN-bus modules), or compact.

Althought I had been working on dsDRO in the past, the idea of dedicating a laptop wasn't appealing either. And dsPIC are junk, I learnt to hate them the more I gained experience in the Embedded world.

The availability of cheap & powerful STM32 boards with LCD, plus awesome RTOS ChibiOS and uGFX graphical lib enabled a much better, cleaner solution... so I gave it a shot!

How?
====
The base of the system is a STM32F746G-DISO board. It's a demoboard from ST, leveraging their 200MHz+ ARM Cortex-M7 MCU and featuring a nice LCD.
It's IOs are somewhat limited, but sufficient to handle 3 digital scales + a quadrature encoder + a digital gauge.
And if it's lacking any IOs, there is a CAN controller we can use to communicate with other modules.

The basic configuration is the STM32F746G-DISO + an 'Arduino-style' shield to fanout the connections to the different scales. iGaging-style scales can even be connected directly to the STM32 IOs so there is no need for specific electronics.

Who?
====
I'm Charles-Henri Mousset, and I'm a Mechatronics Engineer. I've spent now close to 15 years doing electronics, mechanical and informatics stuff, of which 8 as a professionnal. But always as a passionate!
Before I started my professionnal carreer, I gave birth to some projects like YAPSC (a servomotor controller), and participated on dsDRO and PAPSI projects. Unfortunately, with most of my time dedicated to work I had very little time left for personal project, and even less for sharing them.
Since I've got some more free time nowadays, a bigger proportion can be spent in my garage :)