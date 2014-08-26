#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxSvg.h"
#include "ofxXmlSettings.h"

struct draggableVertex {
	
	draggableVertex() : bBeingDragged(false), bOver(false), radius(2) {};
	bool 	bBeingDragged;
	bool 	bOver;
	float 	radius;
	
};

class ofApp : public ofBaseApp{

public:
	void setup();
	void update();
	void draw();
	void exit();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	
	ofImage image, imageDesaturated, imageThresholded;
	ofVboMesh corners;
	ofVboMesh lineMesh, lineMeshShort;
	int closestIndex, lastAddedIndex;
	float cornerRadius;
	ofVec2f translate;
	float scale, scaleStart;
	ofVec2f shiftStart;
	vector<ofPolyline> polylines;
	vector<vector<draggableVertex> > draggables;
	int curPolyline;
	
	ofxXmlSettings XML;

};
