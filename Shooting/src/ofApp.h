#pragma once

#include "ofMain.h"
#include "ofxMultiKinectV2Mod.h"
#include "ofxDatGui.h"
#include "ofxJSON.h"
#include "ofxCv.h"
#include "ofxOpenCv.h"
#include "ofxSecondWindow.h"
#include "ofxKinectProjectorToolkitV2.h"
#include "ofxGrabCam.h"
#include "ofxDmx.h"
#include "ofxOsc.h"
#include "ofxImageSequenceLoader.h"
#include "ofxStopwatch.h"

#include "DiscController.h"

#include "Config.h"

#define MODE_CALIBRATION    1
#define MODE_SHOOTING       2

#define F_ORIGIN            0
#define F_AXIS_X            1
#define F_AXIS_Y            2
#define F_DISPLAY           3

class ofApp : public ofBaseApp{

public:
	void setup();
	void update();
    
    void initShooting();
    
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
    void makeDiscMask();
    
    ofVec2f getProj(ofVec3f wc);
    ofVec3f getDisc(ofVec3f wc);
    ofVec3f getWorldFromDisc(ofVec2f dc);
    ofVec2f getProjFromDisc(ofVec2f dc);
    ofVec2f getProjFromDisc(float x, float y) { return getProjFromDisc(ofVec2f(x, y)); }
    
    // util
    string toString(ofVec2f val);
    string toString(ofVec3f val);
    
    
    // visualize
    ofFbo               view3d;
    ofxGrabCam          camera;
    ofShader            pointShader;
    ofMesh              depthPointCloud;
    ofFloatPixels       depthPixels;
    ofFloatImage        depthImage;
    
    // kinect
    ofxKinectProjectorToolkitV2 kpt;
    ofxMultiKinectV2Mod	kinect;
    
    ofShader            depthShader, irShader;
    ofTexture           depthTex, irTex, colorTex;
    
    int                 mode = MODE_CALIBRATION;
    int                 currentFeature = F_ORIGIN;
    
    int                 restCalibrationFrames = -1;
    bool                useCalibrated = false;
    
    // util
    stringstream        ss;
    ofxSecondWindow     secondWindow;
    
    // decorative key
    bool                altPressed = false;
    bool                shiftPressed = false;
    
    
    ofPixels            discMaskPixels;
    ofImage             discMaskImage;
    
    // parameters
    ofxDatGui*          gui;
    float               projectorWidth, projectorHeight;
    ofVec2f             kOrigin, kAxisX, kAxisY;
    ofVec3f             wOrigin, wAxisX, wAxisY;
    ofMatrix4x4         discMat, discInvMat;
    float               discMaskMargin;
    bool                isDisplayIr;
    
    float				near, far; // cm
    ofVec2f             focus;
    
    float               discMaskThreshold;
    
    //------------------------------------
    // shooting
    void                sendDmx();
    void                enableLight();
    void                disableLight();
    
    void                doBeforeShoot();
    void                doAfterShoot();
    void                loadHeightmap();
    
    ofMesh              heightmapMesh;
    ofShader            heightmapShader;
    
    ofPath              contour;
    
    ofVec2f             hudOrigin;
    
    ofxImageSequenceLoader heightmapLoader;
    
    float               tolerance;
    int                 currentFrame = -1;
    bool                isDisplayHeightmap;
    
    ofFloatPixels       heightmapPixels;
    ofPixels            heightmapU8Pixels;
    ofFloatImage        heightmapImage;
    ofxCvGrayscaleImage  heightmapCvImage;
    
    ofxCvContourFinder contourFinder;
    
    ofxDmx              dmx;
    float               lightFront, lightBack;
    bool                isPreviewLight = false;
    
    ofxOscReceiver      dfReceiver;
    
    DiscController      discController;
    float               heightmapRotation;
    float               rotateStep; // degrees
    
    ofFbo               hud;
    
    // timer
    ofxStopwatch        timer;
};
