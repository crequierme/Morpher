// Pixel.h
// CPSC 6040        Caroline Requierme (crequie@clemson.edu)        10/3/2017
// 
// Class Pixel is used to store channel values (Red, Green, Blue, and Alpha) for an individual
// pixel of an image. This class will be referenced by class Pixmap.
//
// Members of the class include:
// unsigned char r,g,b,a - channel values of a pixel from 0-255
//
#include <iostream>
using namespace std;

#ifndef PIXEL
#define PIXEL

class Pixel{
	private:
		// pixel has space allocated for four channel values
		unsigned char r;
		unsigned char g;
		unsigned char b;
		unsigned char a;
		
	public:
		// constructors -- default and variable
		Pixel(void);
		// the param constructor will be used for testing
		Pixel(unsigned char rVal, unsigned char gVal, unsigned char bVal, unsigned char aVal);
        
        	// getters and setters to members of the class
		unsigned char getRVal(void);
		unsigned char getGVal(void);
		unsigned char getBVal(void);
		unsigned char getAVal(void);	

		void setRVal(unsigned char rVal);
		void setGVal(unsigned char gVal);
		void setBVal(unsigned char bVal);
		void setAVal(unsigned char aVal);

		void setAllVals(unsigned char rVal, unsigned char gVal, unsigned char bVal, unsigned char aVal);

};

#endif
