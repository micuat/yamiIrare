#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetVerticalSync(true);
	
	image.loadImage(ofToDataPath("ill.jpg"));
	imageDesaturated = image;
	imageDesaturated.setImageType(OF_IMAGE_GRAYSCALE);
	imageDesaturated.update();
	ofxCv::blur(imageDesaturated, 3);
	
	cv::Mat srcMat;
	srcMat = ofxCv::toCv(imageDesaturated);
	vector<cv::Point2f> cornersTemp;
	cv::goodFeaturesToTrack(srcMat, cornersTemp, 1000, 0.04, 10.0);
	for( int i = 0; i < cornersTemp.size(); i++ ) {
		cornersTemp.at(i) *= 480.f / image.getWidth();
	}
	
	ofFbo linePatch;
	lineMesh.setMode(OF_PRIMITIVE_LINE_STRIP);
	corners.setMode(OF_PRIMITIVE_POINTS);
	ofSetLineWidth(2.f);
	ofImage edge;
	ofxCv::Canny(imageDesaturated, edge, 80, 150);
	ofxCv::invert(edge);
	edge.update();
	imageDesaturated = edge;
	imageDesaturated.resize(480, image.getHeight() * 480 / image.getWidth());
	
	for( int i = 0; i < cornersTemp.size(); i++ ) {
		corners.addVertex(ofxCv::toOf(cornersTemp.at(i)));
//		for( int j = i+1; j < cornersTemp.size(); j++ ) {
//			cv::Point2f &pi = cornersTemp.at(i);
//			cv::Point2f &pj = cornersTemp.at(j);
//			cv::Point2f p;
//			p = pi - pj;
//			float length = cv::norm(p);
//			if( 1.0 < length && length < 30.0 ) {
//				linePatch.allocate(abs(p.x) + 1, abs(p.y) + 1);
//				linePatch.begin();
//				ofBackground(255);
//				ofSetColor(0);
//				ofTranslate(-min(pi.x, pj.x), -min(pi.y, pj.y));
//				ofLine(pi.x, pi.y, pj.x, pj.y);
//				linePatch.end();
//				
//				ofPixels pixels;
//				linePatch.readToPixels(pixels);
//				
//				int totalCount = 0, matchCount = 0;
//				for( int y = 0; y < pixels.getHeight(); y++ ) {
//					for( int x = 0; x < pixels.getWidth(); x++ ) {
//						if( pixels.getColor(x, y).getBrightness() < 128 ) {
//							totalCount++;
//							if( imageDesaturated.getColor(x + min(pi.x, pj.x), y + min(pi.y, pj.y)).getBrightness() > 50 )
//								matchCount++;
//						}
//					}
//				}
//				ofLogVerbose() << (float)matchCount / (float)totalCount;
//				
//				if( (float)matchCount / (float)totalCount > 0.5 ) {
//					lineMesh.addVertex(ofxCv::toOf(pi));
//					lineMesh.addVertex(ofxCv::toOf(pj));
//				}
//			}
//		}
	}
	
	ofLogVerbose() << corners.getNumVertices() << " points found";
	
	lastAddedIndex = 0;
	scale = scaleStart = 1.f;
	cornerRadius = 3.f;
	
	polylines.resize(256);
	draggables.resize(polylines.size());
	curPolyline = 0;
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
	bool bFinal = ofGetKeyPressed('a');
	
	ofBackground(204);
	ofSetLineWidth(1.f);
	
	ofPushMatrix();
	
	ofTranslate(translate);
	ofScale(scale, scale);
	
	ofSetColor(255);
	if( !bFinal ) {
		imageDesaturated.draw(0, 0);
	}
	ofSetColor(200, 30, 30);
	
	closestIndex = 0;
	float closestDistanceSquared = 480*480;
	
	ofSetColor(220, 170, 170);
//	lineMesh.draw();
	
	if( !bFinal ) {
		for( int j = 0; j < polylines.size(); j++ ) {
			polylines.at(j).draw();
		}
	}

	ofPushStyle();
	for( int j = 0; j < polylines.size(); j++ ) {
		if( bFinal ) {
			ofSetColor(200, 50, 50, 255);
			ofSetLineWidth(1.f);
		} else if( j == curPolyline ) {
			ofSetColor(200, 50, 50, 255);
			ofSetLineWidth(2.f);
		} else {
			ofSetColor(200, 50, 50, 80);
			ofSetLineWidth(1.f);
		}
		if( polylines.at(j).size() > 0 ) {
			if( !bFinal ) {
				ofPushStyle();
				ofSetColor(200, 50, 50, 255);
				ofDrawBitmapString(ofToString(j), polylines.at(j)[0] + ofVec2f(3, 0));
				ofPopStyle();
			}
			
			ofNoFill();
			ofBeginShape();
			ofCurveVertex(polylines.at(j)[0]);
			for (int i = 0; i < polylines.at(j).size(); i++){
				ofCurveVertex(polylines.at(j)[i]);
			}
			ofCurveVertex(polylines.at(j)[polylines.at(j).size() - 1]);
			ofEndShape();
		}
	}
	ofPopStyle();
	
	// show a faint the non-curve version of the same polygon:
	ofEnableAlphaBlending();
	ofNoFill();
	
	if( !bFinal ) {
		ofSetColor(0,0,0,80);
		for( int j = 0; j < polylines.size(); j++ ) {
			for (int i = 0; i < polylines.at(j).size(); i++){
				if (draggables.at(j).at(i).bOver == true) ofFill();
				else ofNoFill();
				ofCircle(polylines.at(j)[i], draggables.at(j).at(i).radius);
			}
		}
	}
	ofDisableAlphaBlending();
	
	if( !bFinal ) {
		ofSetColor(50, 200, 50);
		glPointSize(2);
		corners.draw();
	}
	
	closestIndex = -1;
	for( int i = 0; i < corners.getNumVertices(); i++ ) {
		ofVec2f p = (ofVec2f(mouseX, mouseY) - translate) / scale;
		float distanceSquared = p.distanceSquared(corners.getVertex(i));
		if( distanceSquared < cornerRadius * cornerRadius ) {
			closestIndex = i;
			break;
		}
		
//		ofCircle(corners.at(i).x, corners.at(i).y, 1);
	}
	ofSetColor(30, 30, 200);
	ofCircle(corners.getVertex(closestIndex), cornerRadius);
	
	ofPopMatrix();
	
	ofDrawBitmapString(ofToString(curPolyline) + " th selected", 20, 20);
	ofDrawBitmapString(ofToString(ofGetFrameRate()) + " fps", 20, 40);
}

//--------------------------------------------------------------
void ofApp::exit(){
//	lineMesh.save(ofToDataPath("mesh.ply"));
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if( key == OF_KEY_SHIFT ) {
		// initial position
		shiftStart.x = mouseX;
		shiftStart.y = mouseY;
		scaleStart = scale;
	}
	else if( key == OF_KEY_CONTROL ) {
		// initial position
		shiftStart.x = translate.x - mouseX;
		shiftStart.y = translate.y - mouseY;
	}
	else if( key == 'z' ) {
		curPolyline++;
	}
	else if( key == 'x' ) {
		if( curPolyline > 0 ) {
			curPolyline = curPolyline--;
		}
	}
	else if( key == OF_KEY_BACKSPACE ) {
		if( polylines.at(curPolyline).size() > 0 ) {
			polylines.at(curPolyline).getVertices().pop_back();
		}
	}
	else if(key == 's'){
		cout << "<svg width=\"5cm\" height=\"4cm\" viewBox=\"0 0 500 400\">" << endl;

		for( int j = 0; j < polylines.size(); j++ ) {
			if( polylines.at(j).size() <= 0 ) continue;
			
			cout << "<path d=\"";
			cout << "M" << polylines.at(j)[0].x << "," << polylines.at(j)[0].y;
			for(int i = -1; i < (int)polylines.at(j).size() - 2; i++) {
				ofPoint pT;
				if( i < 0 ) pT = polylines.at(j)[0];
				else pT = polylines.at(j)[i];
				
				ofPoint p1 = polylines.at(j)[i+1];
				ofPoint p2 = polylines.at(j)[i+2];
				ofPoint p3;
				if( i+3 < polylines.at(j).size() ) p3 = polylines.at(j)[i+3];
				else p3 = polylines.at(j)[i+2];
				
				float cmr_to_bezier = 6.f;
				
				float dx1 = (p2.x - pT.x) / cmr_to_bezier;
				float dy1 = (p2.y - pT.y) / cmr_to_bezier;
				
				float dx2 = (p3.x - p1.x) / cmr_to_bezier;
				float dy2 = (p3.y - p1.y) / cmr_to_bezier;
				
				if( i < 0 ) {
					cout << "C" << (p1.x+dx1) << "," << (p1.y+dy1) << "," << (p2.x-dx2) << "," << (p2.y-dy2) << "," << p2.x << "," << p2.y;
				} else {
					cout << "S" << (p2.x-dx2) << "," << (p2.y-dy2) << "," << p2.x << "," << p2.y;
				}

			}
			cout << "\" stroke=\"red\" />" << endl;
		}
		cout << "</svg>" << endl;
//		XML.pushTag("STROKE", numDragTags-1);
//		XML.saveFile("ill.svg");
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
	if( ofGetKeyPressed(OF_KEY_SHIFT) ) {
		scale = ofMap(y - shiftStart.y, -ofGetWindowHeight()/2, ofGetWindowHeight()/2, scaleStart-1.f, scaleStart+1.f);
	}
	if( ofGetKeyPressed(OF_KEY_CONTROL) ) {
		translate.x = x + shiftStart.x;
		translate.y = y + shiftStart.y;
	}
	for (int i = 0; i < polylines.at(curPolyline).size(); i++){
		float diffx = x - (polylines.at(curPolyline)[i].x * scale + translate.x);
		float diffy = y - (polylines.at(curPolyline)[i].y * scale + translate.y);
		float dist = sqrt(diffx*diffx + diffy*diffy);
		if (dist < draggables.at(curPolyline).at(i).radius * scale){
			draggables.at(curPolyline).at(i).bOver = true;
		} else {
			draggables.at(curPolyline).at(i).bOver = false;
		}	
	}
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
	ofVec2f p = (ofVec2f(x, y) - translate) / scale;
	
	for (int i = 0; i < polylines.at(curPolyline).size(); i++){
		if (draggables.at(curPolyline).at(i).bBeingDragged == true){
			polylines.at(curPolyline)[i].x = p.x;
			polylines.at(curPolyline)[i].y = p.y;
		}
	}
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
//	lineMesh.addVertex((ofVec2f(x, y) - translate) / scale);
	
	ofVec2f p = (ofVec2f(x, y) - translate) / scale;
	
	bool isDragged = false;
	for (int i = 0; i < polylines.at(curPolyline).size(); i++){
		float diffx = x - (polylines.at(curPolyline)[i].x * scale + translate.x);
		float diffy = y - (polylines.at(curPolyline)[i].y * scale + translate.y);
		float dist = sqrt(diffx*diffx + diffy*diffy);
		if (dist < draggables.at(curPolyline).at(i).radius * scale){
			draggables.at(curPolyline).at(i).bBeingDragged = true;
			isDragged = true;
		} else {
			draggables.at(curPolyline).at(i).bBeingDragged = false;
		}	
	}
	if( !isDragged ) {
		if( closestIndex >= 0 ) {
			polylines.at(curPolyline).addVertex(corners.getVertex(closestIndex));
		} else {
			polylines.at(curPolyline).addVertex(p);
		}
		draggables.at(curPolyline).push_back(draggableVertex());
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	for (int i = 0; i < polylines.at(curPolyline).size(); i++){
		draggables.at(curPolyline).at(i).bBeingDragged = false;	
	}
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
