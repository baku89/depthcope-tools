//
//  Utils.h
//  Kinect2Test
//
//  Created by 麦 on 7/22/16.
//
//

#pragma once

#include "ofMain.h"
#include "ofxCv.h"

#include <opencv2/photo/photo.hpp>

class DepthFiller {
private:
	
	int w, h;
	ofPixels depthPixels, maskPixels, inpaintedPixels;
	cv::Mat depthImage;
	cv::Mat maskImage;
	
public:
	
	float radius = 3.0;
	
	void allocate(int _w, int _h) {
		w = _w;
		h = _h;
		
		depthPixels.allocate(w, h, OF_IMAGE_GRAYSCALE);
		maskPixels.allocate(w, h, OF_IMAGE_GRAYSCALE);
		inpaintedPixels.allocate(w, h, OF_IMAGE_GRAYSCALE);
	}
	
	ofFloatPixels inpaint(ofFloatPixels srcPixels) {
		
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
		
		cv::inpaint(depthImage, maskImage, depthImage, (double)radius, cv::INPAINT_TELEA);
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
		
		return srcPixels;
	}
};