//
//  DiscController.h
//  Shooting
//
//  Created by Baku Hashimoto on 7/30/16.
//
//

#pragma once

#include "ofMain.h"

#define DATA_LENGTH 3

#define STEPPER_RPM 800
#define ENCODER_RPM 360

#define ENCODER_STEP 0.1481481481

class DiscController {
public:
    
    void setup(string port, float initialAngle) {
        
        serial.setup(port, 9600);
        
        currentAngle = initialAngle;
    }
    
    void update() {
        
        while (serial.available() >= 3) {
            
            char code = serial.readByte();
            
            if (code == 'H') {
                unsigned char low = serial.readByte();
                unsigned char high = serial.readByte();
                
                int value = (high << 8) | low;
                cout << "code=" << code << "\tvalue=" << value << endl;
                
            } else if (code == 'D') {
                bRotating = false;
            }
            
        }
    }
    
    void rotate(float degrees) {
        if (bRotating) {
            ofLogError() << "DiscController: cannot rotate while rotating";
            return;
        }
        
        currentAngle += degrees;
        
        float rotateDegrees = degrees;
        
        ofLogNotice() << "Rotating..." + ofToString(rotateDegrees, 2) + "deg";
        
        unsigned int minutes = rotateDegrees * 60;
        unsigned char buff[3] = {'R', (unsigned char)(minutes & 0xff), (unsigned char)((minutes >> 8) & 0xff)};
        serial.writeBytes(&buff[0], 3);
        
        bRotating = true;
    }
    
    float getAngle() {
        return currentAngle;
    }
    
    void setAngle(float degrees) {
        currentAngle = degrees;
    }
    
    bool isRotating() {
        return bRotating;
    }
    
private:
    
    ofSerial serial;
    
    float currentAngle;
    
    bool    bRotating = false;
    
    
    
};