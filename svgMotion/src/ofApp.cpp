#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofHideCursor();
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	int count = 0;
	
	ofxSVG svg;
	svg.load(ofToDataPath("ill.svg"));
	for( int i = 0; i < svg.getNumPath(); i++ ) {
		ofPath p = svg.getPathAt(i);
		// svg defaults to non zero winding which doesn't look so good as contours
		p.setPolyWindingMode(OF_POLY_WINDING_ODD);
		vector<ofPolyline>& lines = p.getOutline();
		for(int j=0;j<(int)lines.size();j++){
			polylines.push_back(lines[j].getResampledBySpacing(1));
			for( int k = 0; k < polylines[polylines.size()-1].size(); k ++ ) {
				count++;
			}
			for( int k = 0; k < polylines[polylines.size()-1].size(); k ++ ) {
				polylines[polylines.size()-1].getVertices().at(k).y *= -1;
				polylines[polylines.size()-1].getVertices().at(k) -= ofVec2f(70, -50);
				polylines[polylines.size()-1].getVertices().at(k) *= 4;
			}
		}
	}
	
	mesh.setMode(OF_PRIMITIVE_POINTS);
	float amp = 200;
	for( int i = 0; i < count; i++ ) {
		mesh.addVertex(ofVec3f(ofRandom(amp*2) - amp, ofRandom(amp*2) - amp, ofRandom(amp*2) - amp));
	}
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
	ofBackground(24, 255);
	
	s.begin();
	
	//ofRotate(ofMap(ofGetElapsedTimef(), 0, 2, 0, 360), 0, 1, 0);
	ofColor color;
	color = ofColor(200, 255);
	ofFill();
	
	if( s++ ) {
		ofSetLineWidth(1);
		ofNoFill();
		cam.begin();
		
		ofSetColor(color);
		for (int i = 0; i < (int)polylines.size(); i++){
			ofPolyline & line = polylines[i];
			
			line.draw();
		}
		cam.end();
//		if( s.getElapsed() > 3.f ) s.proceed();
	}
	else if( s++ ) {
		ofSetColor(color);
		drawPoints(1, true);
//		if( s.getElapsed() > 5.f ) s.proceed();
	}
	else if( s++ ) {
		ofSetColor(color);
		drawPoints(ofMap(s.getElapsed(), 0, 1, 1, 6, true), false);
		if( s.getElapsed() > 1.f ) s.proceed();
	}
	else if( s++ ) {
		ofSetColor(color);
		drawPoints(ofMap(s.getElapsed(), 0, 3, 6, 0, true), false);
		if( s.getElapsed() > 3.f ) s.proceed();
	}
	
	s.end();
}

//--------------------------------------------------------------
void ofApp::drawPoints(float amp, bool transform){
	cam.begin();
	ofNoFill();
	
	float p = ofMap(mouseX, 300, 500, 1, 0, true);
	int i = 0;
	for (int j = 0; j < (int)polylines.size(); j++){
		ofSetPolyMode(OF_POLY_WINDING_ODD);
		ofBeginShape();
		ofPolyline & line = polylines[j];
		for( int k = 0; k < line.getVertices().size(); k++ ) {
			ofVec3f v = mesh.getVertex(i);
			ofVec3f vv;
			v.x += 10 * ofNoise(ofGetElapsedTimef(), i, mouseX);
			v.y += 10 * ofNoise(ofGetElapsedTimef() * 2, i, mouseY);
			v.z += 10 * ofNoise(ofGetElapsedTimef() * 3, i, mouseX);
			v *= amp * ofNoise(ofGetElapsedTimef() * 2, i);
			
			vv = v;
			
			if( transform ) {
				vv = v.getInterpolated(line.getVertices().at(k), p);
			}
			if( k == 0 || k == line.getVertices().size() - 1 ) {
				ofCurveVertex(vv.x, vv.y, vv.z);
			}
			ofCurveVertex(vv.x, vv.y, vv.z);
			
			i++;
		}
		ofEndShape(false);
	}
	
	cam.end();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
