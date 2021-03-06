#pragma once

#include "ofMain.h"

#include "ofxSlides.h"
#include "ofxSvg.h"

#include "Particle.h"
#include "Spring.h"

class ofApp : public ofBaseApp{

public:
	void setup();
	void update();
	void draw();
	void drawPoints(float, bool);

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	
	ofxSlides s;
	ofEasyCam cam;
	ofMesh mesh;
	vector<ofPolyline> polylines;
	
	vector<vector<Particle> > particles;
	vector<vector<Particle> > particlesLight;
};
