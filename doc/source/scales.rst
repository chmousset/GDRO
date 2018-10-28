===================================
Digital Gauges, Calipers and Scales
===================================

GDRO supports various type of Gauges, Calipers and Scales.
In the GDRO documentation, we use the term `scale` interchangably for those three types.

Gauge
=====
Sometimes called 'Comparator', these are mainly used to check run-out or a flat surface irregularities rather than indicating an absolute position.
The most common form of Gauge had a round body, and a cylindrical plunger with an harden ball-shaped tip at it's measuring end.
I believe most of them use capacitive sensing technology.


Calipers
========
Calipers have two opposing jaws you clamp on each side of an object to measure its dimentions. Contrary to Gauges, their goal is to measure an absolute distance.
Most of these use capacitive sensing technology.

Scales
======
Scales are meant to be fixed on a machine. Usually, there is a reading head that slides over a (static) scale.
They can use various technologies, the cheapest being the capacitive ones, while the most extreme precision ones can use laser interferometry. On machine tool, magnetic and optical technology is the most widespread.

The physics behind the measure
==============================

Mainly 4 technologies cover >90% of what is present in machine tools:

Capacitive
----------
These sensors use two opposing comb-like patterns, usually drawn in copper on a PCB.
Each of the two opposing patterns elements create a small capacitor between each other. Since the value of this capacitor depends on the surface shared between those opposing elements, the alternating conductor/slot patter of the scale will create wave of change of capacitance when it moves.
A specialized IC (ASIC) reads the capacitance of different reading elements to sense the capacitance wave and interpolate the precision. 
it will depend on the position of the reading head vs the scale

Thanks to massive scale of the PCB technology, these type of sensor are extremely economical to manufacture: you can pickup a 600mm 0.01mm scale for less than 30€. On the other hand, their performance is quite average, with resolution usually in the 0.01mm range, average absolute precision (usually it's not even specified!), and very poor refresh rate (~5Hz max).
These scales are usually incremental, which means they loose absolute position upon power loss.

Optical, digital
----------------
These sensor usually have a transparent glass scale on which opaque stripes are printed onto at a regular pitch `p`. A reading head travels along it, with an LED on one side and 2 photo sensors on the other side to detect the stripes blocking the light, creating a square wave on both channels when the sensor travels along the scale.
Those two sensors are 1/4 the pitch `p` apart, in such a way that their signals are dephased by 90°, creating a `quadrature` pattern which is easy to read with simple logic.
This kind of sensor produce a resolution that is `p/4` (1µm glass scales have a 4µm pitch).
These scales are usually incremental, which means they loose absolute position upon power loss.


Optical, analog
---------------
Similar than the digital counterpart, but this time instead of hard opaque/transparent 'digital' transitions, the sharp stripes are replaced by smoother lines, and the sensors are also optimised to give a linear 'analog' output.
As a result, the sharp wave shape of the digital scale is replaced by a smooth sine/cosine wave.
The sine/cosine pair is digitalized by a specialized IC (ASIC) to interpolate the position *between* two consecutive optical stripes, increasing the resolution way over `p/4`. On high quality scales the resolution can reach 0.1µm or better.
Some scales use fancy patterns to make an absolute reading of the position despite power loss, but most are incremental.

Magnetic
--------
The principle is more or less the same as the optical analog sensors, except that the scale is composed of an alternating pattern of N/S permanent magnets and the sensors read the magnetic field or it's orientation (typically using the Hall effect).
Resolution of 1µm is reachable, but with the added benefit over optical sensors that it's usually insensitive to water or oil, and quite robust to dust. Cost will typically be higher than an optical digital scale, but can be more economic than a optical analog scale for the same kind of performance.
Some scales use fancy patterns to make an absolute reading of the position despite power loss, but most are incremental.
