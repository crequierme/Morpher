// Pixel.cpp
// CPSC 6040        Caroline Requierme (crequie@clemson.edu)        10/3/2017
// 
// Class Pixel is used to store channel values (Red, Green, Blue, and Alpha) for an individual
// pixel of an image. This class will be referenced by class Pixmap.
//
// Members of the class include:
// unsigned char r,g,b,a - channel values of a pixel from 0-255
//
#include <iostream>
#include "Pixel.h"
using namespace std;

//================================================
/* 
Pixel()

* PURPOSE: default constructor, Pixel color values are set to solid black (0,0,0,1)
* INPUTS: none 
* OUTPUTS : none
*/
//================================================
Pixel::Pixel(void){
	r = 0;
	g = 0;
	b = 0;
	a = 255;
}

//================================================
/* 
Pixel()

* PURPOSE: variable constructor, allow user to set channel values in the pixel's construction
* INPUTS: unsigned char rVal, gVal, bVal, aVal
* OUTPUTS : none
*/
//================================================
Pixel::Pixel(unsigned char rVal, unsigned char gVal, unsigned char bVal, unsigned char aVal){
	r = rVal;
	g = gVal;
	b = bVal;
	a = aVal;
}

//================================================
/* 
Getter functions for members of class Pixel (channel values)

* PURPOSE: allow access to Pixel's color and alpha values by returning class members
* INPUTS: none
* OUTPUTS : unsigned char r, g, b, a (one for each function)
*/
//================================================
unsigned char Pixel::getRVal(void){
	return r;
}

unsigned char Pixel::getGVal(void){
	return g;
}

unsigned char Pixel::getBVal(void){
	return b;
}

unsigned char Pixel::getAVal(void){
	return a;
}

//================================================
/* 
Setter functions for members of class Pixel (channel values)

* PURPOSE: set a channel's value for a Pixel object
* INPUTS: unsigned char rVal, gVal, bVal, aVal (one for each function)
* OUTPUTS : none
*/
//================================================
void Pixel::setRVal(unsigned char rVal){
	r = rVal;
}

void Pixel::setGVal(unsigned char gVal){
	g = gVal;
}

void Pixel::setBVal(unsigned char bVal){
	b = bVal;
}

void Pixel::setAVal(unsigned char aVal){
	a = aVal;
}

void Pixel::setAllVals(unsigned char rVal, unsigned char gVal, unsigned char bVal, unsigned char aVal){
	r = rVal;
	g = gVal;
	b = bVal;
	a = aVal; 
}

