//
//  HeightmapExporter.h
//  Kinect2Test
//
//  Created by Baku Hashimoto on 7/21/16.
//
//

#pragma once

#include "ofMain.h"
#include "HeightmapRenderer.h"

#include "Config.h"

class HeightmapExporter {
private:
    ofDirectory         srcDir;
    string              takeName;
    HeightmapRenderer   hmr;
    ofFloatPixels       rendereredImage;
    
public:
    
    
    void start(const HeightmapRenderer _hmr, const string _takeName) {
        takeName = _takeName;
        srcDir = ofDirectory(ofToDataPath(SAVED_DIR + "/" + takeName + "/" + INPAINTED_NAME));
        srcDir.allowExt("exr");
        
        hmr.captureNear = _hmr.captureNear;
        hmr.captureFar  = _hmr.captureFar;
        hmr.near        = _hmr.near;
        hmr.far         = _hmr.far;
        hmr.scale       = _hmr.scale;
        
        hmr.captureSize.set(_hmr.captureSize);
        hmr.focus.set(_hmr.focus);
        hmr.translation.set(_hmr.translation);
        
        hmr.isPreview   = false;
        
        // load
        int i = 0;
        
        for (auto file : srcDir.getFiles()) {
            processImage(file);
        }
    }
    
    void processImage(ofFile file) {
        // load image
        string path = file.getAbsolutePath();
        hmr.loadTexture(path);
        hmr.render();
        hmr.getTexture().readToPixels(rendereredImage);
        
        
        ofStringReplace(path, (const string &)INPAINTED_NAME, (const string &)HEIGHTMAP_NAME);
        ofSaveImage(rendereredImage, path);
    }
    
    
};