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
			particlesLight.push_back(vector<Particle>());
			vector<Particle>& ps = particles.at(particles.size() - 1);
			vector<Particle>& ls = particlesLight.at(particlesLight.size() - 1);
			
			polylines.push_back(lines[j].getResampledBySpacing(10));
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
				ls.push_back(p);
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
			particlesLight.at(i).at(j).resetForce();
		}
    }
	ofVec3f mouseXY = ofVec3f(mouseX - ofGetWidth()/2, - mouseY + ofGetHeight()/2, 0.0);
	float t = ofGetElapsedTimef();
	//ofVec3f mouseXY = ofVec3f(200*sin(4*t)*cos(t), 200*sin(4*t)*sin(t), 0.0);
	//ofVec3f mouseXY = ofVec3f(200*cos(t), 200*sin(8*t), 0.0);
	
	if( !isnan(mouseXY.x) && !isnan(mouseXY.y) ) {
		for (int i = 0; i < particles.size(); i++){
			for (int j = 0; j < particles.at(i).size(); j++){
				if(ofGetKeyPressed('a')){
					particles.at(i).at(j).addAttractionForce(mouseXY.x, mouseXY.y, 150, 1.0);
					particlesLight.at(i).at(j).addAttractionForce(mouseXY.x, mouseXY.y, 200, 1.5);
					particles.at(i).at(j).addAttractionForce(-mouseXY.x, -mouseXY.y, 150, 1.0);
					particlesLight.at(i).at(j).addAttractionForce(-mouseXY.x, -mouseXY.y, 200, 1.5);
				} else {
					particles.at(i).at(j).addRepulsionForce(mouseXY.x, mouseXY.y, 150, 2.0*ofNoise(ofGetElapsedTimef(), (float)i/particles.size()));
					particlesLight.at(i).at(j).addRepulsionForce(mouseXY.x, mouseXY.y, 200, 4.0*ofNoise(ofGetElapsedTimef()));
					particles.at(i).at(j).addRepulsionForce(-mouseXY.x, -mouseXY.y, 150, 2.0*ofNoise(ofGetElapsedTimef(), (float)i/particles.size()));
					particlesLight.at(i).at(j).addRepulsionForce(-mouseXY.x, -mouseXY.y, 200, 4.0*ofNoise(ofGetElapsedTimef()));
				}
				ofVec2f origin(polylines.at(i).getVertices().at(j));
				particles.at(i).at(j).addAttractionForce(origin.x, origin.y, 10000, 0.5);
				particlesLight.at(i).at(j).addAttractionForce(origin.x, origin.y, 10000, 0.5);
			}
		}
	}
    for (int i = 0; i < particles.size(); i++){
		for (int j = 0; j < particles.at(i).size(); j++){
			particles.at(i).at(j).updateForce();
			particlesLight.at(i).at(j).updateForce();
			particles.at(i).at(j).update();
			particlesLight.at(i).at(j).update();
			
			ofVec2f origin(polylines.at(i).getVertices().at(j));
			if(particles.at(i).at(j).position.distance(origin) < 2.f)
				particles.at(i).at(j).velocity = ofVec2f(0, 0);
			if(particlesLight.at(i).at(j).position.distance(origin) < 2.f)
				particlesLight.at(i).at(j).velocity = ofVec2f(0, 0);
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
		ofSetLineWidth(2);
		glPointSize(2.0);
		ofNoFill();
		cam.begin();
		
		ofMesh meshPoints;
		meshPoints.setMode(OF_PRIMITIVE_POINTS);
		for (int i = 0; i < particles.size(); i++){
			for (int j = 0; j < particles.at(i).size(); j++){
				ofPoint solid = particles.at(i).at(j).position;
				ofPoint light = particlesLight.at(i).at(j).position;
				float res = 8.0;
				for( int k = res/2; k < res; k++ ) {
					meshPoints.addVertex(solid.getInterpolated(light, k / res));
					meshPoints.addColor(color.getLerped(ofColor(0, 0, 0, 0), k / res));
				}
			}
		}
		meshPoints.draw();
		ofEnableBlendMode(OF_BLENDMODE_ADD);
		for( int k = 0; k < 3; k++ ) {
			switch (k) {
				case 0:
					ofSetColor(255, 0, 0, 100);
					break;
				case 1:
					ofSetColor(0, 255, 0, 100);
					break;
				case 2:
					ofSetColor(0, 0, 255, 100);
					break;
				default:
					break;
			}
			//ofSetColor(color);
			for (int i = 0; i < particles.size(); i++){
				ofBeginShape();
				for (int j = 0; j < particles.at(i).size(); j++){
					ofPoint solid = particles.at(i).at(j).position;
					ofPoint light = particlesLight.at(i).at(j).position;
					ofPoint inter = solid.getInterpolated(light, k / 32.0);
					ofCurveVertex(inter.x, inter.y);
					if( j == 0 || j == particles.at(i).size() - 1 ) {
						ofCurveVertex(inter.x, inter.y);
					}
				}
				ofEndShape();
			}
		}
		ofDisableBlendMode();
		
		cam.end();
	}
	else if( s++ ) {
		ofSetColor(color);
		drawPoints(1, true);
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
