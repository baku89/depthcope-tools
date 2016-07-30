//
//  ofxStopwatch.h
//  Shooting
//
//  Created by Baku Hashimoto on 7/30/16.
//
//

#pragma once

#include "ofMain.h"

class ofxStopwatch {
public:
    
    ofxStopwatch() {
        prevTime = ofGetElapsedTimef();
    }
    
    void reset() {
        this->reset(0.0f);
    }
    void reset(float initialTime) {
        elapsedTime = initialTime;
        prevTime = ofGetElapsedTimef();
    }
    
    void stop() {
        enabled = false;
    }
    
    void start() {
        enabled = true;
        prevTime = ofGetElapsedTimef();
    }
    
    float getTime() {
        update();
        return elapsedTime;
    }
    
    string getTimeString() {
        update();
        
        int t = elapsedTime;
        int h, m, s;
        string str = "";
        
        h = floor(t / 3600);
        t -= h * 3600;
        
        m = floor(t / 60);
        t -= m * 60;
        
        s = t;
        
        if (h > 0) {
            str += ofToString(h) + ":";
        }
        
        str += ofToString(m, 2, '0') + ":" + ofToString(s, 2, '0');
        
        return str;
    }
    
    bool isEnabled() {
        return enabled;
    }
    
    void toggle(bool _enabled) {
        if (_enabled) {
            start();
        } else {
            stop();
        }
    }
    
private:
    
    void update() {
        if (enabled) {
            float currentTime = ofGetElapsedTimef();
            elapsedTime += currentTime - prevTime;
            prevTime = currentTime;
        }
    }
    
    bool enabled = true;
    float elapsedTime = 0, prevTime;
    
};