-----------------------------------------------
Author Information
-----------------------------------------------
Name: Caroline Requierme
Email: crequie@clemson.edu
Date produced: December 4, 2017
Version: 1

-----------------------------------------------
Included files:
-----------------------------------------------
morpher.cpp
Pixel.h
Pixel.cpp
Pixmap.h
Pixmap.cpp
Segment.h
Segment.cpp
segments.txt *used to store segment coordinate information
-----------------------------------------------
Description
-----------------------------------------------
morpher.cpp is a c++/GLUT program which performs a morph 
between two imagessuch that the subject of one image is transformed 
smoothly into the subject of another image over time, using Beier 
and Neely's segment-based morphing approach. 
The program's morphing process consists of four major steps:
1) Create line segments to drive the morph. These segments are used to
   indicate distinct features in both the source and destination images.
   The program allows the user to read in the data for these segments
   through a text file and/or draw the segments dynamically in the 
   display window using a mouse.
2) Interpolate between the segments of the source image and the 
   destination image over time to drive a gradual warp/morph between
   the images. 
3) Use Beier and Neely's morphing algorithm to warp the source and 
   destination images according to the interpolated segments.If the 
   warp is successful, the image subjects will loosely hold the 
   shape of the source image subject in the beginning of the warp,
   the shape of the destination image subject at the end of the warp,
   and a shared shape in the middle of the warp.
4) Cross dissolve the warped images using alpha as a function of time
   such that in the beginning of the morph, only the source image is 
   visible, in the middle both are equally visible, and only the
   destination image is visible at the end.

-----------------------------------------------
Instructions for Use
-----------------------------------------------

Initializing morpher in the Command Line:
morpher is initialized using the following format...

	morpher imgA.jpg imgB.jpg

where imgA amd imgB are any valid image file name with valid
file type. imgA be the source image in the warp while imgB will
be the destination.
  
NOTE: For the program to morph, both images MUST be the same size.
************************************************
Using keys and mouse in the display window:

Use LEFT and RIGHT ARROW keys to scroll through images in
the display window. This will work with the original images,
the interpolated segment images, and the final sequence images

'q' or 'Q' or ESC
Quit the program

'w' or 'W' will prompt the user for a filename and write out distinct
files for each displayed image. For instance, if the user clicks 'w'
immediately after reading in the images, it will only write out the
two original images. If called after the morph sequence, it will write
out the full morph sequence.

**It is IMPORTANT to note that the following
keypresses need to happen IN THIS CERTAIN ORDER:

(optional) 'r' or 'R'
reads segment data in from a file and displays the
segments over the image. NOTE: you need to toggle between
images once before the segment will display.
Instructions for formatting the text file are below.

(optional) LEFT MOUSE CLICK
Click twice (not double-click) anywhere in the display
window to draw a segment. The first click will register
the start point and the second an end point. If you choose
this method primarily, every time a segment is added, the program 
will print the id and coordinates so that they may be copied and
stored in the text file.

'i' or 'I'
Before interpolating segments, there MUST be a one-to-one 
relationship between image segments such that for every 
one segment in the source image, there is a segment with 
the same id/name in the destination image. The program will 
check if they have the same number of segments, but users must 
keep track of id’s in order for the morph to work.

'm' or 'M'
Perform the warp and cross-dissolve to complete
the morph and update display. This will work slowly
but once completed, the user can scroll through
the frames of the sequence and write out files from
the sequence. 

-----------------------------------------------
Segment Text File Format
-----------------------------------------------
imgA.jpg             // source image filename with valid file type
n                    // number of segments to be read in the image
Feature_label        // an identifier for segment 0 (i.e nose, l_eye, etc.)
0.0 0.50 200.0 400.0 //  coordinates in order x, y, x, y for segment 0 
…
Feature_label         // an identifier for segment n - 1
10.0 20.0 300.0 600.0 //  coordinates in order x, y, x, y for segment 0 

imgB.jpg             // destination image filename with valid file type
n                    // number of segments to be read in the image
Feature_label        // an identifier for segment 0 (i.e nose, l_eye, etc.)
0.0 0.50 200.0 400.0 //  coordinates in order x, y, x, y for segment 0 
…
Feature_label        // an identifier for segment n - 1
10.0 20.0 300.0 600.0 //  coordinates in order x, y, x, y for segment 0 

-----------------------------------------------
Known Issues
-----------------------------------------------
While these have been previously mentioned in this
document, the summary of known issues is as follows:
-The program takes in two images only

-Before interpolating segments, there MUST be a one-to-one 
 relationship between image segments such that for every one 
 segment in the source image, there is a segment with the 
 same id/name in the destination image. The program will check 
 if they have the same number of segments, but users must keep 
 track of id’s in order for the morph to work.

-Both images MUST be the same size.
 
-While the program does not require the user to read segment 
 coordinates from a text file, if the user chooses to do so, 
 the text file must have the name “segments.txt”


