#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxVoro.h"

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	
	ofImage image;
	
    ofEasyCam   cam;
    ofLight     light;
	
	vector<ofPoint> cellCentroids;
    vector<float>   cellRadius;
    vector<ofVboMesh>  cellMeshes;
    vector<ofVboMesh>  cellMeshWireframes;
};
