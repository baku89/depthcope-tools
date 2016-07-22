//
//  PostProcessing.h
//  Kinect2Test
//
//  Created by Baku Hashimoto on 7/21/16.
//
//

#pragma once

#include "ofMain.h"
#include "DepthFiller.h"

#include "Config.h"

//string replaceString(string subject, string search, string replace) {
//	string::size_type pos = subject.find(search);
//	while(pos != string::npos){
//		subject.replace(pos, search.size(), replace);
//		pos = subject.find(search, pos + replace.size());
//	}
//	
//	return subject;
//}

class PostProcessing : public ofThread {
private:
	ofDirectory srcDir;
	stringstream ss;
	string takeName;
	DepthFiller depthFiller;
	ofFloatImage srcImage;
	ofFloatPixels srcPixels;
	
public:
    float progress = 0.0;
	float radius = 3.0;
	
	PostProcessing() : ofThread() {
		
		depthFiller.allocate(DEPTH_WIDTH, DEPTH_HEIGHT);
	}
    
    
    void setup(string _takeName) {
		takeName = _takeName;
        srcDir = ofDirectory(ofToDataPath(SAVED_DIR + "/" + takeName + "/" + RAW_NAME));
        srcDir.allowExt("exr");
    }
    
    void threadedFunction() {
		int i = 0;
		
		int w = DEPTH_WIDTH;
		int h = DEPTH_HEIGHT;
		
		for (auto file : srcDir.getFiles()) {
			processImage(file);
			progress = (float)++i / srcDir.size();
        }
    }
	
    // https://github.com/mazbox/ofxKinectInpainter/blob/master/src/ofxKinectInpainter.h
    void processImage(ofFile file) {
        
        // load image
        ofLoadImage(srcImage, file.getAbsolutePath());
        srcPixels = srcImage.getPixels();
		
		depthFiller.radius = radius;
		srcPixels = depthFiller.inpaint(srcPixels);

		string basename = file.getBaseName();
		
		ofStringReplace(basename, (const string &)RAW_NAME, (const string &)INPAINTED_NAME);
		
		string path = SAVED_DIR + "/" + takeName + "/" + INPAINTED_NAME + "/" + (string)basename + ".exr";
        
        ofSaveImage(srcPixels, ofToDataPath(path));
    }
    
    
};