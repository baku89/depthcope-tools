#pragma once

#include "ofMain.h"
#include "ofxMultiKinectV2.h"
#include "ofxDatGui.h"
#include "ofxImageSequenceRecorder.h"
#include "ofxXmlSettings.h"

#include "PostProcessing.h"
#include "DepthFiller.h"

#include "Config.h"


class ofApp : public ofBaseApp{

public:
	void setup();
	void update();
	void draw();
    void exit();
    
    void initScene();
    void drawScene();
	
	void loadGui();
	void saveGui();
	
    void doPostProcessing();
	
	string getTakeName();

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
    
    // mesh
    ofEasyCam           camera;
    ofShader            pointShader;
    ofMesh              mesh;
    
	ofShader            depthShader, filledShader;
	
	// kinect
	ofxMultiKinectV2	kinect;
	
	DepthFiller			depthFiller;
    ofFloatPixels       depthPixels, testPixels, testFilledPixels;
    ofFloatImage        depthImage, testImage, testFilledImage;
	
    ofxImageSequenceRecorder recorder;
    
    PostProcessing      postProcessing;
    
    
    stringstream        ss;
    bool                isRecording = false;
    bool                willStopRecording = false;
    string              takeName;
    
    
    // parameters
    ofxDatGui*          gui;
    float				near, far; // cm
    ofVec2f             fov;
    
    
};
