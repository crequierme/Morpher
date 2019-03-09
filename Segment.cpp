// Segment.cpp
// CPSC 6040        Caroline Requierme (crequie@clemson.edu)        11/22/2017
// 

#include <iostream>
#include "Segment.h"
using namespace std;


//================================================
/* 
Segment()

* PURPOSE: default constructor,both start and end are set at the origin and ID is blank.
* INPUTS: none 
* OUTPUTS : none
*/
//================================================
Segment::Segment(void){
	start.x = 0;
	start.y = 0;
	end.x = 0;
	end.y = 0;
	id = "";
}

//================================================
/* 
Segment(Vector2D startCoords, Vector2D endCoords, string featureID)

* PURPOSE: 
* INPUTS: 
* OUTPUTS: 
*/
//================================================
Segment::Segment(Vector2D startCoords, Vector2D endCoords, string featureID){
	start = startCoords;
	end = endCoords;
	id = featureID;
}

//================================================
/* 
Segment(float startX, float startY, float endX, float endY, String featureID)

* PURPOSE: 
* INPUTS: 
* OUTPUTS: 
*/
//================================================
Segment::Segment(float startX, float startY, float endX, float endY, string featureID){
	start.x = startX;
	start.y = startY;
	end.x = endX;
	end.y = endY;
	id = featureID;
}

//================================================
/*
Getter functions for class Segment

* PURPOSE: allow access to Segment specs by returning members
* 		   of the class
* INPUTS: none
* OUTPUTS: each function returns one member of the class-- start, end,
*          and ID, respectively.
*/
//================================================

Vector2D Segment::getStartVect(void){
   	return start;
}

Vector2D Segment::getEndVect(void){
	return end;
}

string Segment::getId(void){
	return id;
} 	 

//================================================
/*
Setter functions for class Segment

* PURPOSE: allow access to Segment specs by returning members
* 		   of the class
* INPUTS: none
* OUTPUTS: each function returns one member of the class-- start, end,
*          and ID, respectively.
*/
//================================================

void Segment::setStartVect (float startX, float startY){
	start.x = startX;
	start.y = startY;
}

void Segment::setStartVect (Vector2D startCoords){
	start = startCoords;
}

void Segment::setEndVect (float endX, float endY){
	end.x = endX;
	end.y = endY;
}

void Segment::setEndVect (Vector2D endCoords){
	end = endCoords;
}

void Segment::setId(string featureID){
	id = featureID;
}
