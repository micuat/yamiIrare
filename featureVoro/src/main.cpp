#include "ofApp.h"
#include "ofAppGlutWindow.h"

int main() {
	ofAppGlutWindow window;
	ofSetupOpenGL(&window, 480 * 2, 640, OF_WINDOW);
	ofRunApp(new ofApp());
}
