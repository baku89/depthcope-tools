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
    
    // mesh
    ofEasyCam           camera;
	ofCamera			orthoCamera;
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
	bool				isPreviewHeight;
    string              takeName;
    
    
    // parameters
    ofxDatGui*          gui;
    float				near, far; // cm
	int					cropWidth, cropHeight;
	ofVec2f             focus;
	
    
};
