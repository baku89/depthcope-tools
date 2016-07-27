//
//  ofxMultiKinectV2Mod
//
//  Created by Yuya Hanai on 10/16/14.
//

#pragma once
#include "ofMain.h"

#define USE_OFX_TURBO_JPEG

#ifdef USE_OFX_TURBO_JPEG
#include "ofxTurboJpeg.h"
#endif

class ofProtonect2;

class ofxMultiKinectV2Mod : public ofThread
{
public:
    ofxMultiKinectV2Mod();
    ~ofxMultiKinectV2Mod();
    
    static int getDeviceCount();
    void open(bool enableColor = true, bool enableIr = true, int deviceIndex = 0, int oclDeviceIndex = -1);
    void start();
    void update();
    void close();
    
    bool isFrameNew();
    
    ofPixels& getColorPixelsRef();
    ofFloatPixels& getDepthPixelsRef();
    ofFloatPixels& getIrPixelsRef();
    
    const vector<char>& getJpegBuffer();
    
    void setEnableJpegDecode(bool b) {bEnableJpegDecode = b;}
    bool isEnableJpegDecode() {return bEnableJpegDecode;}
    void setEnableFlipBuffer(bool b) {bEnableFlipBuffer = b;}
    bool isEnableFlipBuffer() {return bEnableFlipBuffer;}
    
    ofProtonect2* getProtonect() {return protonect2;}
    
    float getDistanceAt(int x, int y);
    ofVec3f getWorldCoordinateAt(int x, int y);
//    ofVec3f getWorldCoordinateAtFloat(float x, float y);
    ofVec3f getWorldCoordinateAt(int x, int y, float z);
    
    ofVec2f getFocus();
    
    float feedbackRate = 0.1;
    
protected:
    void threadedFunction();
    
    bool bEnableJpegDecode;
    bool bOpened;
    bool bNewBuffer;
    bool bNewFrame;
    bool bEnableFlipBuffer;
    
    
    
    ofPixels colorPix;
    ofPixels colorPixFront;
    ofPixels colorPixBack;
    
    vector<char> jpeg;
    vector<char> jpegFront;
    vector<char> jpegBack;
    
    ofFloatPixels irPix;
    ofFloatPixels irPixFront;
    ofFloatPixels irPixBack;
    
    ofFloatPixels depthPix, depthPixDistance;
    ofFloatPixels depthPixFront;
    ofFloatPixels depthPixBack;
    
    ofProtonect2* protonect2;
    
    int lastFrameNo;
    
#ifdef USE_OFX_TURBO_JPEG
    ofxTurboJpeg turbo;
#endif
    
};