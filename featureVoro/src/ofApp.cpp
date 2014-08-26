#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

vector<Point2f> corners;

void ofApp::setup() {
    ofEnableAlphaBlending();
    ofSetVerticalSync(true);
    ofEnableSmoothing();
    
    light.setPosition(100,500, 100);
    cam.setAutoDistance(true);
	
	image.loadImage(ofToDataPath("ill.jpg"));
	image.resize(480, 480 / image.getWidth() * image.getHeight());
	image.setImageType(OF_IMAGE_GRAYSCALE);
	image.update();
	ofxCv::blur(image, 3);
	Mat srcMat;
	srcMat = toCv(image);
	goodFeaturesToTrack(srcMat, corners, 2000, 0.04, 10.0);
	
	voro::container con(0,image.getWidth(),
                        0,image.getHeight(),
                        -10,10,
                        1,1,1,
                        true,true,true,
                        8);
	voro::wall_cylinder cyl(0,0,0,0,0,20, min(image.getWidth(), image.getHeight()));
    //con.add_wall(cyl);
	for(int i = 0; i < corners.size(); i++){
		ofPoint p = toOf(corners.at(i));
		p.z = ofNoise(i, p.x) * 10.f;
        addCellSeed(con, p, i, true);
    }
    
    cellMeshes = getCellsFromContainer(con,0.0);
    cellMeshWireframes = getCellsFromContainer(con,0.0,true);
    cellRadius = getCellsRadius(con);
    cellCentroids = getCellsCentroids(con);
	
}

void ofApp::update() {
}

void ofApp::draw() {
//	ofBackground(255);
//	
//	ofSetColor(255);
//	image.draw(0, 0);
//	ofNoFill();
//	ofSetColor(255, 0, 0);
//	for( int i = 0; i < corners.size(); i++ ) {
//		ofCircle(toOf(corners.at(i)), 3);
//	}
    ofBackground(255);
    
    ofPushMatrix();
    cam.begin();
    light.enable();
    ofEnableLighting();
    glEnable(GL_DEPTH_TEST);
    
	ofTranslate(-image.getWidth()/2, image.getHeight()/2, 0);
	ofScale(1, -1, 1);
	
	//    for (int i = 0; i < cellCentroids.size(); i++){
	//        ofSetColor(0);
	//		ofSphere(cellCentroids[i], cellRadius[i]*0.15);
	//    }
    
    for(int i = 0; i < cellMeshes.size(); i++){
        ofSetColor(20, 20, 200,130);
        cellMeshes[i].drawFaces();
        
        ofPushStyle();
        ofSetLineWidth(2);
        //ofSetColor(100);
        cellMeshWireframes[i].draw();
        ofPopStyle();
    }
    
    glDisable(GL_DEPTH_TEST);
    ofDisableLighting();
    light.disable();
    cam.end();
    ofPopMatrix();
}
