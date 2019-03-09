// pixmap.cpp
// CPSC 6040        Caroline Requierme (crequie@clemson.edu)        9/19/2017
// 
// Class Pixmap is used to store image pixel data into a 1-Dimensional array of unsigned char
// so that the image may be displayed, written, and/or manipulated. In this 
// format data is stored one pixel at a time and one channel at a time. For example,
// the first four indeces of a pixmap storing a four channel image contain 1) first pixel 
// R value 2) first pixel G value 3) first pixel B value and 4) first pixel A value
//
// Members of the class include:
//  int width - the x-resolution of the stored image
//  int height - the y-resolution of the stored image
//  int numChannels - the number of channels in the ORIGINAL image (note: this class makes 
//                    space assuming FOUR channels and has the ability to adjust 1 or 3 channel images
//					  to display as RGBA images)
//  unsigned char* pmPointer - pointer to the array of unsigned char which stores pixel data
//

#include <iostream>
#include <vector>
#include "Pixmap.h"
#include "Pixel.h"
#include "Segment.h"
using namespace std;


//================================================
/* 
Pixmap()

* PURPOSE: default constructor, Pixmap has no size and pointers don't point to anything
* INPUTS: none 
* OUTPUTS : none
*/
//================================================
Pixmap::Pixmap(void){
	width = 0;
	height = 0;
	pmPointer = new Pixel*[height];
	dataPointer = new Pixel[height * width];
	filename = "";
}

//================================================
/* 
Pixmap(int w, int h)

* PURPOSE: variable constructor
* INPUTS: param -- int w-- xresolution of the image
*	  param -- int h-- yresolution of the image
* OUTPUTS : none
*/
//================================================
Pixmap::Pixmap(int w, int h){
	width = w;
	height = h;
	pmPointer = new Pixel*[height];
	dataPointer = new Pixel[height * width];
	filename = "";


	// construct 2D array for convenient [x][y] indexing of pixels
	pmPointer[0] = dataPointer;
	for (int i=1; i < height; i++){    //index begins at 1
		pmPointer[i] = pmPointer[i-1] + width;
	} 
}

//================================================
/* 
fillPixmap(unsigned char[] channelVals, int numChannels)

* PURPOSE: fill the Pixmap 2D array with pixel color/alpha values
*          if the image data does not have 4 channels (Pixel object holds 
*          4 channel values), adjust to make a 4-channel image
* INPUTS: param -- unsigned char[] channelVals -- 1D array containing unsigned
*		   chars, representing a single channel value of a single pixel.
*		   These values will be in RGBA order. So that the first two
*                  elements will contain the image's first pixel's R and G values,
*		   respectively. If the image does not have four channels, elements
*		   will contain RGB values (for 3 channels) or C values(for 1 channel).
*         param -- int channels-- number of channels the original image contains
*		   NOTE: upon completion of this function, Pixmap object will store
*		   data for a FOUR channel image.
* OUTPUTS : none
*/
//================================================

void Pixmap::fillPixmap(unsigned char* channelVals, int numChannels){
	int channelValsSize = width*height*numChannels;
	//======================================
	// 4 channel (RGBA) image
	if (numChannels == 4){
		int channelIndex = 0; // tells us what channel we're looking at (R0,G1,B2,A3)
		int pixelIndex = 0; //index in dataPointer array of Pixels
		for (int i=0; i < channelValsSize; i++){ // loop through spaces in unsigned char array
			//++++++++++++++++++++++++++++
			if (channelIndex == 0){ // if you've reached an alpha value
				dataPointer[pixelIndex].setRVal(channelVals[i]);
				channelIndex = channelIndex + 1; // tell program to expect G val next
			}
			//++++++++++++++++++++++++++++
			else if (channelIndex == 1){ // if you've reached an alpha value
				dataPointer[pixelIndex].setGVal(channelVals[i]);
				channelIndex = channelIndex + 1; // tell program to expect B val next
			}
			//++++++++++++++++++++++++++++
			else if (channelIndex == 2){ // if you've reached an alpha value
				dataPointer[pixelIndex].setBVal(channelVals[i]);
				channelIndex = channelIndex + 1; // tell program to expect A val next
			}
			//++++++++++++++++++++++++++++
			else if (channelIndex == 3){ // if you've reached an alpha value
				dataPointer[pixelIndex].setAVal(channelVals[i]);
				channelIndex = 0; // tell program to expect R val next
				pixelIndex = pixelIndex + 1; // move to the next pixel
			}
		}
	}
        //======================================
	// 3 channel (RGB) image
	if (numChannels == 3){
		int channelIndex = 0; // tells us what channel we're looking at (R0,G1,B2,A3)
		int pixelIndex = 0; //index in dataPointer array of Pixels
		for (int i=0; i < channelValsSize; i++){ // loop through spaces in unsigned char array
			//++++++++++++++++++++++++++++
			if (channelIndex == 0){ // if you've reached an R value
				dataPointer[pixelIndex].setRVal(channelVals[i]);
				channelIndex = channelIndex + 1; // tell program to expect G val next
			}
			//++++++++++++++++++++++++++++
			else if (channelIndex == 1){ // if you've reached an G value
				dataPointer[pixelIndex].setGVal(channelVals[i]);
				channelIndex = channelIndex + 1; // tell program to expect B val next
			}
			//++++++++++++++++++++++++++++
			else if (channelIndex == 2){ // if you've reached a B value
				dataPointer[pixelIndex].setBVal(channelVals[i]);
				dataPointer[pixelIndex].setAVal(255); // no alpha information, set to opaque
				channelIndex = 0; // go back to R val
				pixelIndex = pixelIndex + 1;
			}		
		}
	}
	//======================================	
	// 1 channel (greyscale) image
	if (numChannels == 1){
		int pixelIndex = 0;
		for (int i=0; i < channelValsSize; i++){ // loop through spaces in unsigned char array
			unsigned char greyVal = channelVals[i];
			dataPointer[pixelIndex].setRVal(greyVal); // RGB values are equal in greyscale images
			dataPointer[pixelIndex].setGVal(greyVal);
			dataPointer[pixelIndex].setBVal(greyVal);
			dataPointer[pixelIndex].setAVal(255);
			pixelIndex = pixelIndex + 1;
		}	
	}
	
}

//================================================
void Pixmap::fillSolidColor(unsigned char rVal, unsigned char gVal, unsigned char bVal, unsigned char aVal){
	for (int row = 0; row < height; row++){
		for (int col = 0; col < width; col++){
			pmPointer[row][col].setAllVals(rVal, gVal, bVal, aVal);
		}
	}
}

//================================================
/*
Getter functions for class Pixmap

* PURPOSE: allow access to Pixmap specs by returning members
* 		   of the class
* INPUTS: none
* OUTPUTS: each function returns one member of the class-- width,
*          height, pmPointer, and dataPointer respectively
*/
//================================================

string Pixmap::getFilename(void){
	return filename;
}

int Pixmap::getWidth(void){
	return width;
}

int Pixmap::getHeight(void){
	return height;
}

Pixel** Pixmap::getPmPointer(void){
	return pmPointer;

}

Pixel* Pixmap::getDataPointer(void){
	return dataPointer;
}

void Pixmap::setFilename(string fn){
	filename = fn;
}

//================================================
int Pixmap::getNumSegments(void){
	return segmentList.size();
}

void Pixmap::addSegment(Segment seg){
	segmentList.push_back(seg);
}


vector<Segment> Pixmap::getSegmentList(void){
	return segmentList;
}

// search for a segment based on it's identifying name
// returns an index of the found segment in the segment list
int Pixmap::getSegmentById(string id){
	int foundPos;
	bool found = false;
	int i = 0;
	while (found == false && i < segmentList.size()){
		if (segmentList[i].getId() == id){
			foundPos = i;
			found = true;
		}
		else {
			i = i + 1;
		}
	}
	
	if (found == true){
		return foundPos;
	}
}


