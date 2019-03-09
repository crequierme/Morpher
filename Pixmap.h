// Pixmap.h
// CPSC 6040        Caroline Requierme (crequie@clemson.edu)        10/3/2017
// 
// Class Pixmap is used to store image pixel data into a 1-Dimensional array 
//
// NOTE: This class depends on class Pixel
//
// Members of the class include:

//
#include <iostream>
#include <vector>
#include "Pixel.h"
#include "Segment.h"
using namespace std;

#ifndef PIXMAP
#define PIXMAP

class Pixmap{
	private:
		int width;
		int height;
		Pixel** pmPointer; // 2D array pointer, points to column of pointers to rows of Pixel objects
				   // (i.e. pmPointer[1] points to the FIRST row of Pixels that form the image
		Pixel* dataPointer; // 1D array pointer, points to array of Pixels of length width * height
		vector<Segment> segmentList; // vector of segment objects, identify distinct features to be morphed
		string filename; // filename of read image
	public:
		// constructors -- default and variable
		Pixmap(void);
		Pixmap(int w, int h);
        	
		// fill pixmap with pixel data and adjust so that the pixmap has four channels
		void fillPixmap(unsigned char* channelVals, int numChannels);
		void fillSolidColor(unsigned char rVal, unsigned char gVal, unsigned char bVal, unsigned char aVal);
		
       		// getters and setters to members of the class
		string getFilename(void);
		void setFilename(string fn);
		int getWidth(void);
		int getHeight(void);
		Pixel** getPmPointer(void);
		Pixel* getDataPointer(void);
		
		// functions to access and modify feature segments
		int getNumSegments(void);
		void addSegment(Segment seg);
		int getSegmentById(string id);
		vector<Segment> getSegmentList(void);

};

#endif
