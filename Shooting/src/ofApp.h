#pragma once

#include "ofMain.h"
#include "ofxMultiKinectV2Mod.h"
#include "ofxDatGui.h"
#include "ofxJSON.h"
#include "ofxCv.h"
#include "ofxSecondWindow.h"
#include "ofxKinectProjectorToolkitV2.h"
#include "ofxGrabCam.h"
#include "ofxDmx.h"
#include "ofxOsc.h"

#include "Config.h"

#define MODE_CALIB_PLANE    1
#define MODE_SHOOTING       2

#define F_ORIGIN            0
#define F_AXIS_X            1
#define F_AXIS_Y            2

class ofApp : public ofBaseApp{

public:
	void setup();
	void update();
    
    void initScene();
    void updateScene();
    void drawScene();
    
	void draw();
    void drawCalibration();
    void drawShooting();
    
    void exit();
	
	void loadGui();
//    void updateGui();
	void saveGui();
    
    void onSliderEvent(ofxDatGuiSliderEvent e);
    void onToggleEvent(ofxDatGuiToggleEvent e);
    void onButtonEvent(ofxDatGuiButtonEvent e);
    void on2dPadEvent(ofxDatGui2dPadEvent e);

    void keyPressed(int key);
    void keyReleased(int key);
    
    // calibration
    string getCurrentFeatureName();
    void moveFeature(float x, float y);
    void makePlaneMask();
    
    ofVec2f getProj(ofVec3f wc);
    ofVec3f getPlane(ofVec3f wc);
    
    // util
    string toString(ofVec2f val);
    string toString(ofVec3f val);
    
    
    // visualize
    ofFbo               view3d;
    ofxGrabCam          camera;
    ofShader            pointShader;
    ofMesh              mesh;
    ofFloatPixels       depthPixels;
    ofFloatImage        depthImage;
    
    // kinect
    ofxKinectProjectorToolkitV2 kpt;
    ofxMultiKinectV2Mod	kinect;
    
    ofShader            depthShader, irShader;
    ofTexture           depthTex, irTex, colorTex;
    
    int                 mode = MODE_CALIB_PLANE;
    int                 currentFeature = F_ORIGIN;
    
    int                 restCalibrationFrames = -1;
    bool                useCalibrated = false;
    
    // util
    stringstream        ss;
    ofxSecondWindow     secondWindow;
    
    // decorative key
    bool                altPressed = false;
    bool                shiftPressed = false;
    
    ofMesh              heightmap;
    
    ofPixels            planeMaskPixels;
    ofImage             planeMaskImage;
    
    // parameters
    ofxDatGui*          gui;
    float               projectorWidth, projectorHeight;
    ofVec2f             kOrigin, kAxisX, kAxisY;
    ofVec3f             wOrigin, wAxisX, wAxisY;
    ofMatrix4x4         planeMat, planeInvMat;
    float               planeMaskMargin;
    
    float				near, far; // cm
    ofVec2f             focus;
    
    float               planeMaskThreshold;
    
    //------------------------------------
    // shooting
    void                enableLight();
    void                disableLight();
    
    void                doBeforeShoot();
    void                doAfterShoot();
    
    ofxDmx              dmx;
    float               lightFront, lightBack;
    bool                isPreviewLight;
    bool                isShooting = false;
    
    ofxOscReceiver      dfReceiver;
    

};
