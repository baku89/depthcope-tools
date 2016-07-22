#pragma once

#include "ofMain.h"

#include "ofxXmlSettings.h"
#include "ofxDatGui.h"
#include "ofxImageSequenceRecorder.h"

#include "Config.h"
#include "HeightmapRenderer.h"
#include "HeightmapExporter.h"

class ofApp : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();
    void exit();
    
    void loadGui();
    void saveGui();
    
    void loadFrame();
    
    void loadTake();
    void loadTakeWithDialog();
    
    void exportTake();
    
    void onSliderEvent(ofxDatGuiSliderEvent e);
    void onToggleEvent(ofxDatGuiToggleEvent e);

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
    
    // objects
    HeightmapRenderer       hmr;
    HeightmapExporter       exporter;
    
    // status
    string                  takeName;
    vector<ofFile>          frameList;
    int                     currentFrame = 0;
    int                     totalFrame = 0;
    bool                    needsUpdate = false;
    
    // gui
    ofxDatGui*              gui;
    ofxDatGuiLabel*         guiTakeName;
    
};
