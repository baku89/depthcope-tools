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

string replaceString(string subject, string search, string replace) {
	string::size_type pos = subject.find(search);
	while(pos != string::npos){
		subject.replace(pos, search.size(), replace);
		pos = subject.find(search, pos + replace.size());
	}
	
	return subject;
}

class PostProcessing : public ofThread {
public:
	
	ofDirectory srcDir;
    stringstream ss;
	string takeName;
    
    ofFloatImage srcImage;
    ofFloatPixels srcPixels;
	ofPixels depthPixels, maskPixels, inpaintedPixels;
	cv::Mat depthImage;
	cv::Mat maskImage;
	
    float progress = 0.0;
    
    
    void setup(string _takeName) {
		takeName = _takeName;
        srcDir = ofDirectory(ofToDataPath(SAVED_DIR + "/" + takeName + "/" + RAW_NAME));
        srcDir.allowExt("exr");
    }
    
    void threadedFunction() {
		int i = 0;
		
		int w = DEPTH_WIDTH;
		int h = DEPTH_HEIGHT;
		
		depthPixels.allocate(w, h, OF_IMAGE_GRAYSCALE);
		maskPixels.allocate(w, h, OF_IMAGE_GRAYSCALE);
		inpaintedPixels.allocate(w, h, OF_IMAGE_GRAYSCALE);
		
		for (auto file : srcDir.getFiles()) {
			processImage(file);
			progress = (float)++i / srcDir.size();
        }
    }
	
    // https://github.com/mazbox/ofxKinectInpainter/blob/master/src/ofxKinectInpainter.h
    void processImage(ofFile file) {
		
		// R channel represents raw Kinect depth
		// G channel: invalid detph
		// G channel: hole-filled depth
        
        // load image
        ofLoadImage(srcImage, file.getAbsolutePath());
        srcPixels = srcImage.getPixels();
        
        int w = DEPTH_WIDTH;
        int h = DEPTH_HEIGHT;

        int x, y, offset;
        
        // copy to cv context
        for (y = 0; y < h; y++) {
            for (x = 0; x < w; x++) {
                offset = y * w + x;
                depthPixels[offset] = srcPixels[offset * 3] * 255;
                maskPixels[offset] = srcPixels[offset * 3 + 1] > 0.5 ? 255 : 0;
            }
        }
        depthImage = ofxCv::toCv(depthPixels);
        maskImage   = ofxCv::toCv(maskPixels);
        
        cv::inpaint(depthImage, maskImage, depthImage, 3, cv::INPAINT_NS);
        ofxCv::toOf(depthImage, inpaintedPixels);
		
		// composite with original image
		for (y = 0; y < h; y++) {
			for (x = 0; x < w; x++) {
				offset = y * w + x;
				if (maskPixels[offset] > 0.5) {
					srcPixels[offset * 3 + 2] = inpaintedPixels[offset] / 255.0f;
				} else {
					srcPixels[offset * 3 + 2] = srcPixels[offset * 3];
				}
			}
		}
		
		string path = SAVED_DIR + "/" + takeName + "/" + INPAINTED_NAME + "/" + (string)file.getBaseName() + ".exr";
        
        ofSaveImage(srcPixels, ofToDataPath(path));
        
        
        
    }
    
    
};