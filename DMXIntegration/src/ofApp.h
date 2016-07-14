#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxDatGui.h"

#define DF_PORT			1234
#define TO_C4D_PORT		1300
#define FROM_C4D_PORT	1301

class ofApp : public ofBaseApp{

public:
	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	
	ofxOscReceiver	dfReceiver, c4dReceiver;
	ofxOscSender	c4dSender;
	
	stringstream	ss;
	
	ofxDatGui*		gui;
	
	
	// value
	int				position;
	float			rotation, fillLight, limLight;
	
	

	
	
};
