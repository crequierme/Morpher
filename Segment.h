// Segment.h
// CPSC 6040        Caroline Requierme (crequie@clemson.edu)        11/22/2017
// 
// Class Segment stores information on individual line segments for the purposes
// of segment-based morphing.
//
// NOTE: This class makes use of Vector2D objects defined in matrix.h
//
#include <iostream>

using namespace std;

#ifndef SEGMENT
#define SEGMENT

struct Vector2D{  // store vector coordinates
float x, y;
};


class Segment{
	private:
		Vector2D start; // starting point (x,y) of line segment
		Vector2D end; // ending point (x,y) of line segment
		string id; // identifier for feature indicated by segment (i.e. "nose", "l_eye", etc.)
	public:
		// constructors -- default and variable
		Segment(void);		
		Segment(Vector2D startCoords, Vector2D endCoords, string featureID);
		Segment(float startX, float startY, float endX, float endY, string featureID);

       		// getters and setters to members of the class
		void setStartVect (float startX, float startY);
		void setStartVect (Vector2D startCoords);
		Vector2D getStartVect(void);
		
		void setEndVect (float endX, float endY);
		void setEndVect (Vector2D endCoords);
		Vector2D getEndVect(void);
		
		void setId(string featureID);
		string getId(void); 	 

};

#endif
