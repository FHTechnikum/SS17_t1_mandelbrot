# t1_mandelbrot
MANDELBROT GENERATOR @ v1.0  
Created by Sebastian Dichler, 2017  
  
The pixelgen program is generating the mandelbrot picture and writes the pixels into a local memory. This memory is moved into a global, shared memory.  
The writepic program is reading the pixels from the shared memory and writes them into a ppm P3 picture.  

The whole process is running in a while loop and zooms out of the standard zoom.  
  
### Please run writepic before pixelgen and stop it before pixelgen due to possible data loss!  
  
Debug messages appear read and time calculation messages appear in yellow background  
Format input:
<pre>
./writepic  
./pixelgen -w 3840 -h 2160 -i 30000 -c 1  
</pre>

Useable parameters
<pre>
-w    to change Image width  
-h    to change Image height  
-i    to change iterations  
-t    to change zoom  
  0   the default template  
  1   for 1st template  
  2   for 2nd template  
  3   for 3rd template  
  4   for 4th template  
  5   for 5th template  
  6   for 6th template  
  7   for 7th template  
  8   for 8th template  
  9   for 9th template  
  10  for 10th template  
-c    change color type  
  1   for sinus cyan  
  2   for sinus red  
  3   for sinus green  
  4   for sinus blue  
  5   for PT1 cyan  
  6   for PT1 red  
  7   for PT1 green  
  8   for PT1 blue  
  9   for LOG LOG colorfull (use less iterations)  
  0   for LOG LOG all channels  
-?    to print help message  
</pre>
