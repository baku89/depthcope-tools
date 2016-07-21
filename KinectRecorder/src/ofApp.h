#pragma once

#include "ofMain.h"
#include "ofxMultiKinectV2.h"
#include "ofxDatGui.h"
#include "ofxImageSequenceRecorder.h"

#define DEPTH_WIDTH     512
#define DEPTH_HEIGHT    424
#define STEP            1

#define GUI_WIDTH       320


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
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	
	ofxMultiKinectV2	kinect;
	ofTexture			depthTex, irTex;
    
    ofVec2f             center, fov;
    
    ofFloatPixels       dispPixels;
    ofFloatImage        dispImage;
    
    int step = STEP;
    
    ofxDatGui*      gui;
    
    bool            isRecording = false;
    
    ofxImageSequenceRecorder recorder;
    

    
    
    // parameters
    float near = 50, far = 500; // cm
    
    
};
