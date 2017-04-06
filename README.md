# t1_mandelbrot
MANDELBROT GENERATOR @ v1.0  
Created by Sebastian Dichler, 2017  
  
The pixelgen program is generating the mandelbrot picture and writes the pixels into a local memory. This memory is moved into a global, shared memory.  
The writepic program is reading the pixels from the shared memory and writes them into a ppm P3 picture.  

The whole process is running in a while loop and zooms out of the standard zoom.  
  
Debug messages appear read and time calculation messages appear in yellow background  
Format input:
<pre>
./writepic  
./pixelgen -w 3840 -h 2160 -i 30000  
</pre>

Useable parameters
<pre>
-w    to change Image width  
-h    to change Image height  
-i    to change iterations  
-t    to change zoom  
 -0   the default template  
 -1   for 1st template  
 -2   for 2nd template  
 -3   for 3rd template  
 -4   for 4th template  
 -5   for 5th template  
 -6   for 6th template  
 -7   for 7th template  
 -8   for 8th template  
 -9   for 9th template  
 -10  for 10th template  
-b    to change blue color output  
-?    to print help message  
</pre>
