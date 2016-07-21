//
//  PostProcessing.h
//  Kinect2Test
//
//  Created by Baku Hashimoto on 7/21/16.
//
//

#pragma once

#include "ofMain.h"
//#include "ofxOpenCv.h"
#include "ofxCv.h"

#include "Config.h"

#include <opencv2/photo/photo.hpp>

class PostProcessing : public ofThread {
public:
    
    ofDirectory dir;
    stringstream ss;
    
    ofFloatImage originalImage;
    ofFloatPixels floatPixels;
    
    float progress = 0.0;
    
    
    void setup(string _dirname) {
        dir = ofDirectory(_dirname);
        dir.allowExt("exr");
    }
    
    void threadedFunction() {
        
        dir.getFiles();
        
        for (int i = 0; i < dir.size(); i++) {
            ofFile file = dir.getFile(i);
            progress = (float)i / dir.size();
            
            processImage(file);
        }
    }
    
    // https://github.com/mazbox/ofxKinectInpainter/blob/master/src/ofxKinectInpainter.h
    void processImage(ofFile file) {
        
        // load image
        ofLoadImage(originalImage, file.getAbsolutePath());
        floatPixels = originalImage.getPixels();
        
        int w = DEPTH_WIDTH;
        int h = DEPTH_HEIGHT;

        ofPixels depthPixels, maskPixels;
        depthPixels.allocate(w, h, OF_IMAGE_GRAYSCALE);
        maskPixels.allocate(w, h, OF_IMAGE_GRAYSCALE);
        
        int x, y, offset;
        
        // copy to cv context
        for (y = 0; y < h; y++) {
            for (x = 0; x < w; x++) {
                offset = y * w + x;
                float d = floatPixels[offset * 4];
                depthPixels[offset] = (unsigned char)floatPixels[offset * 4] * 255;
                maskPixels[offset] = floatPixels[offset * 4 + 3] < 0.5 ? 255 : 0;
            }
        }
        
        cv::Mat depthImage = ofxCv::toCv(depthPixels);
        cv::Mat maskImage   = ofxCv::toCv(maskPixels);
        
        cv::inpaint(depthImage, maskImage, depthImage, 3, cv::INPAINT_NS);
        
        
        
        
        
        ofPixels inpaintedPixels;
        inpaintedPixels.allocate(w, h, OF_IMAGE_GRAYSCALE);
        
        ofxCv::toOf(depthImage, inpaintedPixels);
        
        
        // save image
        string basename = file.getBaseName();
        ss.str("");
        ss << INPAINTED_DIR << "/" << basename << ".png";;
        
        ofSaveImage(inpaintedPixels, ofToDataPath(ss.str()));
        
        
        
    }
    
    
};