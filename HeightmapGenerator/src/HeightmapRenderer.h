//
//  HeightmapRenderer.h
//  HeightmapGenerator
//
//  Created by Baku Hashimoto on 7/22/16.
//
//

#pragma once

#include "ofMain.h"
#include "Config.h"

class HeightmapRenderer {
public:
    
    float               captureNear, captureFar, near, far;
    float               scale = 1;
    ofVec2f             focus, captureSize, translation;
    bool                isPreview;
    
    HeightmapRenderer() {
        captureSize.set(8, 8);
        focus.set(0, 0);
        
        shader.load("heightmap");
        
        plane.set(DEPTH_WIDTH, DEPTH_HEIGHT);
        plane.setResolution(DEPTH_WIDTH, DEPTH_HEIGHT);
    }
    
    void render() {
        
        if (fbo.getWidth() != captureSize.x || fbo.getHeight() != captureSize.y) {
            alloate();
        }
        
        fbo.begin();
        {
            ofBackground(0);
            
            if (depthImage.isAllocated()) {
                ofSetColor(255, 255, 255);
                
                shader.begin();
                {
                    shader.setUniformTexture("depth", depthImage.getTexture(), 0);
                    shader.setUniform1f("near", near);
                    shader.setUniform1f("far", far);
                    shader.setUniform1f("scale", scale);
                    shader.setUniform1f("captureNear", captureNear);
                    shader.setUniform1f("captureFar", captureFar);
                    shader.setUniform2f("focus", focus.x, focus.y);
                    shader.setUniform2f("translation", translation.x, translation.y);
                    shader.setUniform2f("captureSize", captureSize.x, captureSize.y);
                    shader.setUniform1i("isPreview", (int)isPreview);
                    
                    plane.draw();
                }
                
                shader.end();
            }
        }
        fbo.end();
    }
    
    void loadTexture(string path) {
        ofLogNotice() << "loading: " << path;
        depthImage.load(path);
    }
    
    ofTexture getTexture() {
        return fbo.getTexture();
    }
    
    int getWidth() {
        return (int)captureSize.x;
    }
    int getHeight() {
        return (int)captureSize.y;
    }
    
private:
    
    ofFbo               fbo;
    ofPlanePrimitive    plane;
    ofShader            shader;
    ofFloatImage        depthImage;
    
    void alloate() {
        int w = getWidth();
        int h = getHeight();
        fbo.allocate(w, h, GL_RGBA32F);
    }
};