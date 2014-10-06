#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofHideCursor();
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetVerticalSync(true);
    ofSetFrameRate(60);
	
	int count = 0;
	
	ofxSVG svg;
	svg.load(ofToDataPath("ill.svg"));
	for( int i = 0; i < svg.getNumPath(); i++ ) {
		ofPath p = svg.getPathAt(i);
		// svg defaults to non zero winding which doesn't look so good as contours
		p.setPolyWindingMode(OF_POLY_WINDING_ODD);
		vector<ofPolyline>& lines = p.getOutline();
		
		for(int j=0;j<(int)lines.size();j++){
			particles.push_back(vector<Particle>());
			springs.push_back(vector<Spring>());
			vector<Particle>& ps = particles.at(particles.size() - 1);
			vector<Spring>& ss = springs.at(springs.size() - 1);
			
			polylines.push_back(lines[j].getResampledBySpacing(1));
			for( int k = 0; k < polylines[polylines.size()-1].size(); k ++ ) {
				count++;
			}
			for( int k = 0; k < polylines[polylines.size()-1].size(); k ++ ) {
				polylines[polylines.size()-1].getVertices().at(k) -= ofVec2f(svg.getWidth(), svg.getHeight())/2;
				polylines[polylines.size()-1].getVertices().at(k).y *= -1;
				polylines[polylines.size()-1].getVertices().at(k) *= 0.75;
				
				Particle p;
				p.friction = 0.02;
				p.radius = 2;
				p.setup(polylines[polylines.size()-1].getVertices().at(k), ofVec2f(0, 0));
				ps.push_back(p);
				if( k > 0 ) {
					Spring s;
					s.distance = ps.at(k-1).position.distance(ps.at(k).position);
					s.springiness = 0.8;
					s.particleA = &(ps.at(k-1));
					s.particleB = &(ps.at(k));
					ss.push_back(s);
				}
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
	for (int i = 0; i < particles.size(); i++){
		for (int j = 0; j < particles.at(i).size(); j++){
			particles.at(i).at(j).resetForce();
		}
    }
	ofVec3f mouseXY = ofVec3f(mouseX - ofGetWidth()/2, - mouseY + ofGetHeight()/2, 0.0);
	
	if( !isnan(mouseXY.x) && !isnan(mouseXY.y) ) {
		for (int i = 0; i < particles.size(); i++){
			for (int j = 0; j < particles.at(i).size(); j++){
				if(ofGetKeyPressed('a')){
					particles.at(i).at(j).addAttractionForce(mouseXY.x, mouseXY.y, 100, 2.0);
				} else {
					particles.at(i).at(j).addRepulsionForce(mouseXY.x, mouseXY.y, 100, 2.0);
				}
				ofVec2f origin(polylines.at(i).getVertices().at(j));
				particles.at(i).at(j).addAttractionForce(origin.x, origin.y, 10000, 0.5);
			}
		}
	}
//    for (int i = 0; i < springs.size(); i++){
//		for (int j = 0; j < springs.at(i).size(); j++){
//			springs.at(i).at(j).update();
//		}
//    }
    for (int i = 0; i < particles.size(); i++){
		for (int j = 0; j < particles.at(i).size(); j++){
			particles.at(i).at(j).updateForce();
			particles.at(i).at(j).update();
			ofVec2f origin(polylines.at(i).getVertices().at(j));
			if(particles.at(i).at(j).position.distance(origin) < 2.f)
				particles.at(i).at(j).velocity = ofVec2f(0, 0);
		}
    }
	ofSetWindowTitle(ofToString(ofGetFrameRate()));
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
		for (int i = 0; i < particles.size(); i++){
			ofBeginShape();
			ofCurveVertex(particles.at(i).at(0).position.x, particles.at(i).at(0).position.y);
			for (int j = 0; j < particles.at(i).size(); j++){
				ofCurveVertex(particles.at(i).at(j).position.x, particles.at(i).at(j).position.y);
			}
			ofCurveVertex(particles.at(i).at(particles.at(i).size()-1).position.x, particles.at(i).at(particles.at(i).size()-1).position.y);
			ofEndShape();
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
