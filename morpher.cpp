//   morpher.cpp
//   CPSC 6040        Caroline Requierme (crequie@clemson.edu)        9/19/2017
//
/*
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
*/
//=======================================================================================

#include <OpenImageIO/imageio.h>
#include <iostream>
#include <cstring>
#include <fstream>
#include <vector>
#include <math.h>
#include "Segment.h"
#include "Pixel.h"
#include "Pixmap.h"

#ifdef __APPLE__
#  pragma clang diagnostic ignored "-Wdeprecated-declarations"
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

using namespace std;
OIIO_NAMESPACE_USING

// window dimensions
#define WIDTH 600	
#define HEIGHT 600


bool hasReadImage = false; // defines whether an image has been successfully read and stored
int numPixmaps = 0; // in cases of multiple images, defines the number of images stored
int currentIndex = 0; // in cases of multiple images index of the currently displayed pixmap
Pixmap currentPm; // current pixmap being displayed, set when pixmap(s) is read and stored
Pixmap* pmArray; // in cases of multiple images, pointer to array which contains all pixmaps
vector<float> newSeg; // holds coordinates of new segment when user clicks to draw segment
//===============================================================================================
/*
readMultiImages(int argc, char* argv[])

* PURPOSE : This function should be called when the user inputs multiple arguments into the comm-
*           and line. The arguments succeeding the program name should be file names to be read.
*           For example, if the user enters "imgview cube.ppm teapot.jpg" the program will read
*           the images at the point of initialization. Read the image files as in readImage() but
*           store the pixmaps into global array 'pmArray'. Images will be displayed one at a time,
*           beginning with the first image given in the command line.
* INPUTS :  
*           param -- int arc, number of arguments given in command line
*           param -- char* argv[] arguments given in command line
*           global -- Pixmap currentPm
*           global -- bool hasReadImage, set to true when image(s) has been successfully stored
*           global -- int numPixmaps, set to the number of images read
            global -- Pixmap* pmArray, points to the array of image pixmaps
*           global -- int pmIndex, set to default 0 to mark the current image's place in pmArray
*
* OUTPUTS : no returns, but sets currentPm, hasReadImage, pmArray, numPixmaps, pmIndex
*/
//===============================================================================================
void readMultiImages(int argc, char* argv[]){

  // set numPixmaps to the number of arguments - 1 to account for filenames and ignore the
  // program call
  // allocate space in array to store pixmaps 
  numPixmaps = argc - 1;
  pmArray = new Pixmap[numPixmaps];
  int pmIndex = 0;  

  // loop through the filenames given
  for (int i = 1; i < argc; i ++){
  	string infilename = argv[i];

    // open the image using OIIO
	  ImageInput *infile = ImageInput::open (infilename);
    // error check
    if(!infile){
      cerr << "Could not open image " << infilename << ", error = " << geterror() << endl;}
	
    // get image information using OIIO class ImageSpec
	  const ImageSpec &spec = infile->spec();
  	int xres = spec.width;
  	int yres = spec.height;
  	int numChannels = spec.nchannels;
	
    // declare memory in a 1D array using class Pixmap
  	unsigned char* pointer = new unsigned char [xres*yres*numChannels]; // pointer to the array of image data

    // read image pixels into 8-bit integer unsigned char values and store into pixmap
  	infile->read_image(TypeDesc::UINT8, &pointer[0]);
  
  	Pixmap pm = Pixmap(xres,yres);
  	pm.fillPixmap(pointer, numChannels); // transfer image color data into 4-channel pixmap
	pm.setFilename(infilename);

  
	  // store pixmaps into array
	  *(pmArray + pmIndex) = pm;
	  pmIndex = pmIndex + 1;	
	
  	infile->close();
  	delete infile;
  }
  
// set hasReadImage to tell display and write functions that an image has been read
// set currentPm to the first image that is given in the command line arguments, this will also
// be the first to be displayed
hasReadImage = true;
currentPm = *(pmArray + 0);
currentIndex = 0;

// reshape window to fit first image
int xres = currentPm.getWidth();
int yres = currentPm.getHeight();
glutReshapeWindow(xres,yres);

}

//===============================================================================================
/*
writeMultiImages()

* PURPOSE : Writes out the images in pmArray to separate files 
* INPUTS :  param -- outfilename, name of the file before sequence added (ex. if outfilename = "img", prog
*	    ram will write to "img0.png")
* OUTPUTS : no returns, but downloads the written files to the current folder
*/
//===============================================================================================

void writeMultiImages(string outfilename){

  for (int i = 0; i < numPixmaps; i++){

  // get size specs of the image to be written
  int w = pmArray[i].getWidth();
  int h = pmArray[i].getHeight();
  std::ostringstream sin; // convert sequence number to a string
  sin << i;
  std::string val = sin.str();
  string filename = outfilename.append(val); // append to base name given
  filename = filename.append(".png");

  // create the oiio file handler for the image
  ImageOutput *outfile = ImageOutput::create(filename);
  if(!outfile){
    cerr << "Could not create output image for " << filename << ", error = " << geterror() << endl;
    return;
  }
  #include <stdlib.h>
  // open a file for writing the image. The file header will indicate an image of
  // width w, height h, and 4 channels per pixel (RGBA). All channels will be of
  // type unsigned char
  ImageSpec spec(w, h, 4, TypeDesc::UINT8);
  if(!outfile->open(filename, spec)){
    cerr << "Could not open " << outfilename << ", error = " << geterror() << endl;
    delete outfile;
    return;
  }

  // write the image to the file. All channel values in the pixmap are taken to be
  // unsigned chars

  Pixel* pointer = pmArray[i].getDataPointer();
  if(!outfile->write_image(TypeDesc::UINT8, &pointer[0])){
  	cerr << "Could not write image to " << filename << ", error = " << geterror() << endl;
    	delete outfile;	
    	return;
  }
  else{
    // give notification to user if the image was output
    cout << "Image "<<filename<<", was successfully stored" << endl;}
  
  // close the image file after the image is written
  if(!outfile->close()){
    cerr << "Could not close " << filename << ", error = " << geterror() << endl;
    delete outfile;
    return;
  }


  // free up space associated with the oiio file handler
  delete outfile;
}
}

//===============================================================================================
/*
selectCurrentImage(int direction)

* PURPOSE : Manipulate the global varable currentIndex--the index of the current pixmap displayed
*           in the array of multiple pixmaps-- to allow the user to continuously scroll through multiple 
*           images in the display window.
* INPUTS :  param -- int direction, LOOKS FOR VALUES 0 OR 1, tells the function which direction to go 
*                    through the index value of 0 sets the index backwards and value of 1 sets the index forwards
*           global -- currentIndex, indicates index of the current displayed pixmap in the array of 
*                     multiple pixmaps
*           global -- currentPm, current pixmap displayed
*
* OUTPUTS : no returns, but sets currentPm and currentIndex
*/
//===============================================================================================

void selectCurrentImage(int direction){

// if value given is 0, go backwards in single increments in the array
if (direction == 0){
  // if current pixmap is the first in the array, set the index to the
  // last in the array for continuous scrolling
	if (currentIndex == 0){
	   currentIndex = numPixmaps - 1;
	}
  // if not, set the index to that of the pixmap previous to the current
	else{
	   currentIndex = currentIndex - 1;
	}
}

// if value given is 1, go forwards in single increments in the array
if (direction == 1){
  // if current pixmap is the last in the array, set the index to the
  // first in the array for continuous scrolling
	if (currentIndex == numPixmaps - 1){
	   currentIndex = 0;	
	}
  // if not, set the index to that of the pixmap succeeding the current
	else{
	   currentIndex = currentIndex + 1;
	}
}

// set the current pixmap based on the current index
// reshape window to fit the pixmap
currentPm = *(pmArray + currentIndex);
int xres = currentPm.getWidth();
int yres = currentPm.getHeight();
glutReshapeWindow(xres,yres);

}

//===============================================================================================
/*
getPmByName(string filename)

* PURPOSE : Search through pmArray to find a Pixmap with a given filename
* INPUTS : param -- string filename, name of file to search for (i.e. "img.jpg")
           global-- pmArray list of Pixmaps on rotational display in window 
* OUTPUTS : int imgIndex, index of the found file in the pmArray. If there is no
	    such file, error prints out in terminal. 
*/
//===============================================================================================

int getPmByName(string filename){

   int imgIndex = -1; //index of found image is -1 until found
   for (int img = 0; img < numPixmaps; img++){ // loop through pixmaps
      if (pmArray[img].getFilename() == filename){ // if filenames match
         imgIndex = img; //get index
      }
   }

   if (imgIndex == -1){ // if not found
      cerr << "No matching filename found in collection." << endl;
   } 
   else{
      return imgIndex;
}
}
//===============================================================================================
/*
void readTextFile()
* PURPOSE : Read in segment information from a text file
* INPUTS :  none, function reads in hard-coded filename for a ".txt" file
* OUTPUTS : none, adds segments to appropriate pixmap
*/
//===============================================================================================

void readTextFile(){
   int numSegments;
   string filename;
   string id;
   float startX;
   float startY;
   float endX;
   float endY;
   float startOffset;
   float startYCoord;
   float endOffset;
   float endYCoord;

   ifstream textFile;
   textFile.open("segments.txt");

   if (textFile.fail()){
      cerr << "Failed to open text file." << endl;
      exit(1);
   }
   for (int i = 0; i < numPixmaps; i++){ // loop through pixmaps
	textFile >> filename;   // get filename for next segments
	int imgIndex = getPmByName(filename);// get pixmap for those segments
	textFile >> numSegments;
	for (int j = 0; j < numSegments; j++){ // loop through number of segments in pixmap
		textFile >> id;		       // get segment data
		textFile >> startX;
		textFile >> startY;
		textFile >> endX;
		textFile >> endY;
		startOffset = (pmArray[0].getHeight()/2) - startY;
		startYCoord = (pmArray[0].getHeight()/2)+ startOffset;
		endOffset = (pmArray[0].getHeight()/2) - endY;
		endYCoord = (pmArray[0].getHeight()/2)+ endOffset;
		Segment seg = Segment( startX, startYCoord, endX, endYCoord, id); //create segment
		pmArray[imgIndex].addSegment(seg); // add segment to pixmap	
	}
   }
}
//===============================================================================================
/*
void clickAddSegment(int button, int state, int x, int y)

* PURPOSE : Mouse callback function, Create a segment for the currently displayed image via a sequence of mouse clicks
*           from the user. The user should be able to click twice anywhere within the window and 
*	    create a segment between the two click locations. After the user clicks, they will be
*	    prompted for an identifying name for the segment.
* INPUTS :  param -- int button, defines which button on the mouse is being used, comes in as a 
*                  GLUT enum
*	    param -- int state, defines state of given button, GLUT enum GLUT_UP or GLUT_DOWN
*	    param -- int x, y position of mouse
*	    global -- newSeg, vector that holds segment coordinates when drawing segment
* OUTPUTS : none, adds segments to appropriate pixmap
*/
//===============================================================================================
void clickAddSegment(int button, int state, int x, int y){

   bool hasCoords = false; // not enough coordinates for 

   if (button==GLUT_LEFT_BUTTON && state == GLUT_DOWN && hasCoords == false){ // if left mouse click
	newSeg.push_back(x); // give coordinates to newSeg
	newSeg.push_back(y);
	if (newSeg.size() == 4){ // if you have all the coordinates to make a segment
		hasCoords = true;
        }
   }

   if (hasCoords==true){
      string id; // prompt user for segment id
      cout << "Please provide new segment identifier: ";
      cin >> id;   

      Segment s = Segment(newSeg[0], newSeg[1], newSeg[2], newSeg[3], id); // create a segment
      pmArray[currentIndex].addSegment(s); // add segment to the currently displayed pixmap
	
      float startYOffset = (pmArray[0].getHeight()/2) - s.getStartVect().y;
      float startY= (pmArray[0].getHeight()/2)+ startYOffset;
      float endYOffset = (pmArray[0].getHeight()/2) - s.getEndVect().y;
      float endY= (pmArray[0].getHeight()/2)+ endYOffset;

      cout << "Segment successfully added: " << endl;
      cout << id << endl;
      cout << newSeg[0]<< " "<< startY << " "<<newSeg[2] << " "<<endY<< endl;
      newSeg.clear(); // clear global vector to prepare for more added segments
      hasCoords = false; 
}
   glutPostRedisplay();
}

//===============================================================================================
/*
void drawSegments()

* PURPOSE : Uses GLUT to draw segments associated with the currently dipslayed pixmap. This is
*	    useful both in the segment interpolation process as well as when files are initially
*	    read in. This represents visually to the user what the warps are based on. Segments will
*           be different colors to identify corresponding segment pairs and will have square "points"
*	    represented at the start coordinates of the segment to show direction.
* INPUTS :  global-- currentPm, this function retrieves the segments from the currentPm and draws them
* OUTPUTS : none, displays feature segments in currently displayed image
*/
//===============================================================================================
void drawSegments(){

      vector<Segment> segs = currentPm.getSegmentList(); // get segments from the currently displayed image
      
      
      glLineWidth(2.5); 
      // list of possible segment colors
      float colors [5][3] = {{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}, {1.0, 1.0, 0.0}, {1.0, 0.0, 1.0}}; 
      int colorIndex = 0;
      float sqOffset = 3.0;// determines width of square "point"
      float startY;
      float endY;
      float startYOffset; // offsets used to overcome differences in GLUT display coordinates and actual vector coordinates
      float endYOffset;
     
      for (int i = 0; i < segs.size(); i++){ //loop through segments
		glColor3f(colors[colorIndex][0], colors[colorIndex][1], colors[colorIndex][2]);
		// draw line
		glBegin(GL_LINES);
		startYOffset = (pmArray[0].getHeight()/2) - segs[i].getStartVect().y;
		startY= (pmArray[0].getHeight()/2)+ startYOffset;
		endYOffset = (pmArray[0].getHeight()/2) - segs[i].getEndVect().y;
		endY= (pmArray[0].getHeight()/2)+ endYOffset;
		glVertex3f(segs[i].getStartVect().x, startY, 0.0);
		glVertex3f(segs[i].getEndVect().x, endY, 0.0);
		glEnd();
		
		// draw square "point" at start of segment
		glBegin(GL_POLYGON);
		glVertex3f(segs[i].getStartVect().x - sqOffset, startY + sqOffset, 0.0);
		glVertex3f(segs[i].getStartVect().x + sqOffset, startY + sqOffset, 0.0);
		glVertex3f(segs[i].getStartVect().x + sqOffset, startY - sqOffset, 0.0);
		glVertex3f(segs[i].getStartVect().x - sqOffset, startY - sqOffset, 0.0);
		glEnd();

		glFlush(); // force draw
		if (colorIndex==4){
			colorIndex = 0;
		}
		else if (colorIndex < 4){ // loop through possible colors so each is different
			colorIndex +=1;		
		}
      }
}

//===============================================================================================
/*
void createIntermImages()

* PURPOSE : 1) Allocate pixmap space for the intermediate frames of the morph
*	    2) Calculate intermediate segments for the morph using a weighted average
*	       for between segment pairs.
*	    Before the function is called, the user will only be able to see the original
*	    images given with their segments. After the function is called, the user will
*	    "preview" the warp, being able to rotate through the morph frames. Intermediate
*	    frames will be represented by black backgrounds with the interpolated segments. 
* INPUTS : none
* OUTPUTS : none, globals pmArray and numPixmaps are updated to make changes to the display.
*/
//===============================================================================================
void createIntermImages(){
	int tempLength = ((numPixmaps - 1) * 3) + numPixmaps; //number of pixmaps in the desired morph sequence
	Pixmap* temp = new Pixmap[tempLength]; // create temporary array of Pixmaps

	int numIntermImages = 3; // number of transition images between source images
	int sourceImageCounter = 0;
	int transCounter = 0; // tells us which intermediate image we're at in the sequence
	float transVals[3] = {0.75, 0.5, 0.25}; // determines our calculations for intermediate segments
	int numSegments = pmArray[0].getNumSegments(); 

	// for i in temp array
	for (int i = 0; i < tempLength; i++){
		// if you're in one of the original images (0, 4, 8, 12) index mod 4 == 0
		if ((i + 4) % 4 == 0){
			// copy it over using image counter
			temp[i] = pmArray[sourceImageCounter];
			sourceImageCounter += 1;
		}
		else{   
			Pixmap src_image = pmArray[sourceImageCounter -1]; // previous source image in temp array
			Pixmap dest_image = pmArray[sourceImageCounter]; // next source image in temp array

			temp[i] = Pixmap(pmArray[0].getWidth(), pmArray[0].getHeight()); // create solid black image
			temp[i].fillSolidColor(0, 0, 0, 255);
			for (int j = 0; j < numSegments; j++){
				vector<Segment> src_segs = src_image.getSegmentList();
				vector<Segment> dest_segs = dest_image.getSegmentList();
				Segment src_seg = src_segs[j];
				// find segment in dest image with same id as current segment
				Segment dest_seg = dest_segs[dest_image.getSegmentById(src_seg.getId())];
				float startX = ((src_seg.getStartVect().x) * transVals[transCounter]) + ((dest_seg.getStartVect().x) * (1 - transVals[transCounter]));
				float startY = ((src_seg.getStartVect().y) * transVals[transCounter]) + ((dest_seg.getStartVect().y) * (1 - transVals[transCounter]));
				float endX = ((src_seg.getEndVect().x) * transVals[transCounter]) + ((dest_seg.getEndVect().x) * (1 - transVals[transCounter]));
				float endY = ((src_seg.getEndVect().y) * transVals[transCounter]) + ((dest_seg.getEndVect().y) * (1 - transVals[transCounter]));
				// create segment with new values
				Segment seg = Segment(startX, startY, endX, endY, src_seg.getId());
				// add segment to interm image
				temp[i].addSegment(seg);   				
			}
			if (transCounter == 2){
				transCounter == 0;
			}
			else if (transCounter < 2){
				transCounter += 1;
			}

		}
	}

	// display segment interpolation sequence	
	numPixmaps = tempLength;	
	pmArray = temp;
	glutPostRedisplay();
}
//===============================================================================================
/*
void morph()

* PURPOSE :  Use Beier-Neely algorithm to warp original images towards each other using 
*            interpolated line segments. In the center of the morph sequence, the subjects of
*	     both images should share the same shape and have a different, realistic looking
*	     subject. After being warped, the images will be gradually cross-dissolved to make
*	     a smooth morphing sequence.  
* INPUTS :  none, makes use of segment and pixel information of pmArray pixmaps
* OUTPUTS : none, displays complete morph sequence
*/
//===============================================================================================
void morph(){
   float times[5] = {0.0, 0.25, 0.5, 0.75, 1.0};

   // go through images in pmArray, 
   // determine images that will be at the beginning and end of each transition
   int numSourceImages = floor (numPixmaps/ 4) + 1; 
   for (int src =1; src < numSourceImages; src ++){ // for each image pair 
   	Pixmap imgA = pmArray[(4 * src) - 4]; // start image in morph
 	Pixmap imgB = pmArray[4 * src]; //end image in morph

	// create temporary array which will hold the warped images in each morph sequence
	// this will be larger than the pmArray as it will hold two images for t = 0.5
	// one where imgA warps to intermediate shape, and one where imgB warps to intermediate shape
	// later this will be referenced to cross dissolve into final morphed images
	int warpLength = 10; 
	Pixmap* warpArray = new Pixmap[warpLength];
	for (int w = 0; w < warpLength; w ++){  // loop through warp array
		warpArray[w] = Pixmap(pmArray[0].getWidth(), pmArray[0].getHeight()); // create black pixmaps
		warpArray[w].fillSolidColor(0, 0, 0, 255);
		Pixmap source; // informs pixel vals
		Pixmap dest;   // informs location vals
		int destIndex;
		
		if (w < 5){ // first half of morph
		        destIndex = w;
			source = imgA;
			dest = pmArray[destIndex];
		}
		else if( w >= 5){ // second half of morph
			destIndex = w - 5;
			source = imgB;
			dest = pmArray[destIndex]; // there will be two warps for middle of morph, need to re-index
		}
	
		Pixel** sourcePointer = source.getPmPointer();
		
		vector<Segment> destSegments = dest.getSegmentList();
		vector<Segment> sourceSegments = source.getSegmentList();
		for (int seg = 0; seg < dest.getNumSegments(); seg++){
			warpArray[w].addSegment(destSegments[seg]);
		}
		// constants to determine weights in warp
		double a = 1; // val barely greater than 0 gives precise control of warp, greater vals have smoother warp but less control
		double b = 2; // ideally in range 0.5 - 2
		double c = 0; // if 0, all segments have same weight; if 1, longer segments have more weight

		// for each pixel in destination pixmap
		for (int row = 0; row < dest.getHeight(); row++){
			for (int col = 0; col < dest.getWidth(); col++){
				Vector2D pixelX;
				pixelX.x = col;
				pixelX.y = row;				
				Vector2D pixelXprime;

				Vector2D dsum; // sum of displacements between source and destination pixels across segments
				dsum.x = 0;
				dsum.y = 0;
				float weightsum = 0; 
				
				// for each segment
				for (int s = 0; s < dest.getNumSegments(); s++){
					//define P,Q,P',Q' // these vals check out
					Vector2D P = destSegments[s].getStartVect();
					Vector2D Q = destSegments[s].getEndVect();
					Vector2D Pprime = sourceSegments[source.getSegmentById(destSegments[s].getId())].getStartVect();
					Vector2D Qprime = sourceSegments[source.getSegmentById(destSegments[s].getId())].getEndVect(); 
					//**************************
					//calculate u,v based on PQ
					//elements of u calculation					
					Vector2D XminusP; // (X - P)
					XminusP.x = pixelX.x - P.x;
					XminusP.y = pixelX.y - P.y;

					Vector2D QminusP; // (Q - P)
					QminusP.x = Q.x - P.x;
					QminusP.y = Q.y - P.y;
					
					float uNumer = (XminusP.x * QminusP.x) + (XminusP.y * QminusP.y); // (X-P) dot (Q-P), numerator in u calculation
					float uDenom = (sqrt((QminusP.x * QminusP.x) + (QminusP.y * QminusP.y))) * (sqrt((QminusP.x * QminusP.x) + (QminusP.y * QminusP.y))); // ||Q - P||^2 
					float u = uNumer/uDenom; // calculate u
	
					//elements of v calculation
					
					Vector2D perpQP; //Perpendicular(Q-P) (x, y) -> (-y,x)
					perpQP.x = (-1) *(QminusP.y);  
					perpQP.y =  QminusP.x;
					
					float vNumer = (XminusP.x * perpQP.x) + (XminusP.y * perpQP.y); // (X - P) dot Perpendicular(Q - P)
					float vDenom = sqrt((QminusP.x * QminusP.x) + (QminusP.y * QminusP.y)); //||Q -P||
					float v = vNumer/vDenom; // calculate v
				

					//**************************
					// calculate X' Based on u,v and P'Q'
					// elements of X' calculation
					
					Vector2D QminusPprime; //Q' - P'
					QminusPprime.x = Qprime.x - Pprime.x;
					QminusPprime.y = Qprime.y - Pprime.y;

					float xPrimeDenom = sqrt((QminusPprime.x * QminusPprime.x) +(QminusPprime.y * QminusPprime.y)); // ||Q' -P'||

					Vector2D perpQPprime; // Perpendicular (Q' - P')
					perpQPprime.x =  (-1) *(QminusPprime.y);
					perpQPprime.y =  QminusPprime.x;

					Vector2D uScalarMult; // u dot (Q' - P')
					uScalarMult.x = QminusPprime.x * u;
					uScalarMult.y = QminusPprime.y * u;
					
					Vector2D xPrimeNumer;  // v dot Perpendicular (Q'- P')
					xPrimeNumer.x = perpQPprime.x * v;
					xPrimeNumer.y = perpQPprime.y * v;

					Vector2D quotient;
					quotient.x = xPrimeNumer.x/ xPrimeDenom;
					quotient.y = xPrimeNumer.y/ xPrimeDenom;
					
					Vector2D localXprime;
					localXprime.x = (Pprime.x + uScalarMult.x + quotient.x); //calculate X'
					localXprime.y = (Pprime.y + uScalarMult.y + quotient.y);
					
					//**************************
					// calculate displacement D for this line segment
					Vector2D d; // displacement between X' and X on the current segment
					d.x = (localXprime.x - pixelX.x);
					d.y = (localXprime.y - pixelX.y);

					
					//**************************
					// calculate shortest distance from X to line segment PQ
					float dist;
					// if 0 > u > 1, shortest distance is abs(v)
					if (u >= 0 && u <= 1){
						dist = fabs(v);
					}
					// if u < 0, shortest is distance between P and X
					else if (u < 0){
						dist = sqrt(((P.x - pixelX.x)*(P.x - pixelX.x)) + ((P.y - pixelX.y) * (P.y - pixelX.y)));
					}
					// if u > 1, shortest is distance between Q and X 
					else if (u > 1){
						dist = sqrt(((Q.x - pixelX.x)*(Q.x - pixelX.x)) + ((Q.y - pixelX.y) * (Q.y - pixelX.y)));
					}
					
					//**************************
					// calculate weight
					double lengthPQ = sqrt(((P.x - Q.x)*(P.x - Q.x)) + ((P.y - Q.y) * (P.y- Q.y)));
					double weightNumer = pow (lengthPQ, c);//length ^p
					double weightBase = weightNumer/(a + dist);
					double weight = pow (weightBase, b);
					weight = float(weight);
					
					
					// update sums
					dsum.x += (d.x * (weight));
					dsum.y += (d.y * (weight));
					weightsum += weight;
					
				} // close loop through segments
				
				Pixel** destPointer = warpArray[w].getPmPointer();
				pixelXprime.x = float(pixelX.x + (dsum.x/weightsum));
				pixelXprime.y = float(pixelX.y + (dsum.y/weightsum));

				
				if (pixelXprime.x >= 0 && pixelXprime.x < source.getWidth() && pixelXprime.y >= 0 && pixelXprime.y < source.getHeight()){

					destPointer[int(pixelX.y)][int(pixelX.x)].setRVal(sourcePointer[int(pixelXprime.y)][int(pixelXprime.x)].getRVal());
					destPointer[int(pixelX.y)][int(pixelX.x)].setGVal(sourcePointer[int(pixelXprime.y)][int(pixelXprime.x)].getGVal());
					destPointer[int(pixelX.y)][int(pixelX.x)].setBVal(sourcePointer[int(pixelXprime.y)][int(pixelXprime.x)].getBVal());
				}
			} // close col loop
		} // close row loop	

	// overlay images with alphas based on time
	// create temp array size numPixmaps
	
	} // close loop through warp array
			
	//**************************
	// cross dissolve over time
	Pixmap* temp = new Pixmap[numPixmaps];
	for (int i = 0; i < numPixmaps; i ++){
		temp[i] = Pixmap(pmArray[0].getWidth(), pmArray[0].getHeight());
		temp[i].fillSolidColor(0, 0, 0, 255);
		int Xindex = i;
		int Yindex = i + (warpLength/2);
		Pixmap imageX = warpArray[Xindex]; // image X and Y will be two different images warped to the same segments
		Pixmap imageY = warpArray[Yindex];
		float alpha = times[i];  // alpha value of the images coincides with time changes
		Pixel** XPointer = imageX.getPmPointer();
		Pixel** YPointer = imageY.getPmPointer();
		Pixel** newPointer = temp[i].getPmPointer();
		for (int ro = 0; ro < pmArray[0].getHeight(); ro++){ // loop through pixels
			for (int co = 0; co < pmArray[0].getWidth(); co++){	
			// the alpha of imageX = 1 - alpha of imageY (ex. if imageX is at 0.25 visibility, imageY is at 0.75 visibility)
				unsigned char rVal = ((1 - alpha)*XPointer[ro][co].getRVal()) + (alpha * YPointer[ro][co].getRVal());
				unsigned char gVal = ((1 - alpha)*XPointer[ro][co].getGVal()) + (alpha * YPointer[ro][co].getGVal());
				unsigned char bVal = ((1 - alpha)*XPointer[ro][co].getBVal()) + (alpha * YPointer[ro][co].getBVal());
				// set new blended values to newly allocated space
				newPointer[ro][co].setRVal(rVal);
				newPointer[ro][co].setGVal(gVal);
				newPointer[ro][co].setBVal(bVal);
			}
		}
	}

	pmArray = temp; // display morph sequence
	
   } //close for loop through image pairs 
}
//===============================================================================================
/*
handleKey(unsigned char key, int x, int y)

* PURPOSE : Keyboard Callback Routine
* INPUTS :   param -- unsigned char key, defines the ascii code for the desired key
*            params -- int x, int y, define position of the key 
* OUTPUTS : none
*/
//===============================================================================================

void handleKey(unsigned char key, int x, int y){
  
  switch(key){
    
    case 'r':
    case 'R':
    readTextFile(); // read segment information from text file
    glutPostRedisplay();
    break;
  
    case 'i':
    case 'I':
    {
       if (pmArray[0].getNumSegments() != pmArray[1].getNumSegments()){
	  cerr << "Cannot interpolate segments, images do not have the same number of segments.";
       }
       else if(pmArray[0].getNumSegments() == pmArray[1].getNumSegments()){
       createIntermImages(); // allocate space and interpolate segments
       glutPostRedisplay();
       }
    }
    break;

    case 'm':
    case 'M':
    morph(); // perform morph
    glutPostRedisplay();
    break;

    case 'w':
    case 'W':
    {
    string filename; 
    cout << "Please provide filename: ";// prompt user for filename
    cin >> filename;
    writeMultiImages(filename); // write out sequence of morph images
    }
    break;
      
    case 'q':		// q - quit
    case 'Q':
    case 27:		// esc - quit
      exit(0);

    default:		// not a valid key -- just ignore it
      return;
  }
}
//===============================================================================================
/*
processSpecialKeys

* PURPOSE : Keyboard Callback Routine for 'special' GLUT key events. In the case of multiple images,
*           the left arrow will display the previous image and the right arrow will display the next
*           image. This is called every time a special key is pressed on the
*           keyboard
* INPUTS :   param -- unsigned char key, defines the ascii code for the desired key
*            params -- int x, int y, define position of the key 
* OUTPUTS : none
*/
//===============================================================================================
void processSpecialKeys(int key, int x, int y) {
   
   switch(key) {

	case GLUT_KEY_LEFT : // left arrow
	selectCurrentImage(0); // select previous pixmap
	glutPostRedisplay();
 	break;

	case GLUT_KEY_RIGHT : // right arrow
	selectCurrentImage(1); // select next pixmap
    	glutPostRedisplay(); 
	break;
}
}


//===============================================================================================
/*
display()

* PURPOSE : Display Callback Routine, Clear the screen and display a black background if no 
*           image has been read. This routine is called every time the window on the screen 
*           needs to be redrawn, like if a new image is displayed. It is also
*           called whenever the program calls glutPostRedisplay() 
* INPUTS :  global -- currentPm, current pixmap displayed
*           global -- hasReadImage, indicates whether an image was successfully
*                     read and can therefore be displayed 
* OUTPUTS : no returns, but displays background and images
*/
//===============================================================================================

void display(){
  

  // specify window clear (background) color to be opaque black
  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
 // clear window to background color
  // flush the OpenGL pipeline to the viewportq
  glFlush();

  // if there is an image to display, get image information
  //if (hasReadImage == true){
	int xres = currentPm.getWidth();
	int yres = currentPm.getHeight();
        
	// glut tends to display images upside down
  // the following glut code overcomes this to display image starting
  // in top-left corner
	glRasterPos2i(0, yres);
  	glPixelZoom(1, -1);
	glDepthFunc(GL_DEPTH_TEST);

  // display the image
      
      glDrawPixels(xres,yres,GL_RGBA,GL_UNSIGNED_BYTE,currentPm.getDataPointer());
      
      drawSegments();

      glFlush();
  //}
}

//===============================================================================================
/*
handleReshape(int w, int h)

* PURPOSE : Reshape Callback Routine: sets up the viewport and drawing coordinates
*           This routine is called when the window is created and every time the window
*           is resized, by the program or by the user
* INPUTS :   param -- int w, int h; define the intended width and height (respectively) 
*            for the display window
* OUTPUTS : none
*/
//===============================================================================================

void handleReshape(int w, int h){
  // set the viewport to be the entire window
  glViewport(0, 0, w, h);
  
  // define the drawing coordinate system on the viewport
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, w, 0, h);
}


//===============================================================================================
/*
main(int argc, char* argv[])

* PURPOSE : Main routine to respond to key presses and command line arguments to read, write, and 
*           manipulate images. 
* INPUTS :   param -- int argc; defines the number of arguments given in the command line
*            param -- char* argv[]; points to array which contains command line arguments given
* OUTPUTS : int
*/
//===============================================================================================

int main(int argc, char* argv[]){

  
  // start up the glut utilities
  glutInit(&argc, argv);
  
  // create the graphics window, giving width, height, and title text
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
  glutInitWindowSize(WIDTH, HEIGHT);
  glutCreateWindow("morpher");
  
  if (argc > 2){
	readMultiImages(argc,argv); // read in images
}

  
  // set up the callback routines to be called when glutMainLoop() detects
  // an event
  glutDisplayFunc(display);	  // display callback
  glutKeyboardFunc(handleKey);	  // keyboard callback
  glutSpecialFunc(processSpecialKeys);
  glutMouseFunc(clickAddSegment);
  glutReshapeFunc(handleReshape); // window resize callback
  
  
  // Routine that loops forever looking for events. It calls the registered
  // callback routine to handle each event that is detected
  glutMainLoop();
  return 0;
}
