//
//  ofxImageSequenceLoader.cpp
//  Shooting
//
//  Created by Baku Hashimoto on 7/28/16.
//
//

#pragma once

#include <glob.h>
#include <regex>

#include "ofMain.h"

class ofxImageSequenceLoader {
public:

    void setup(string pattern) {
        
        std::regex re("^(.*?)(#+)(.*)$");
        std::smatch match;
        regex_match(pattern, match, re);
        
        
        prefix = match[1];
        numWidth = ((string)match[2]).size();
        suffix = match[3];
    
        /*
        glob_t globbuf;
        int ret = glob((prefix + "*" + suffix).c_str(), 0, NULL, &globbuf);
//        glob(const char *, int, int (*)(const char *, int), glob_t *);
        
        for (int i = 0; i < globbuf.gl_pathc; i++) {
            files.push_back(globbuf.gl_pathv[i]);
            ofLogNotice() << files[i];
        }
        
        globfree(&globbuf);
         */
        
    }
    
    bool load(ofFloatPixels &pix, int frame) {
        ss.str("");
        ss << prefix << setw(numWidth) << setfill('0') << frame << suffix;
        
        string path = ss.str();
        ofLogNotice() << "loading: " << path;
        bLoaded = ofLoadImage(pix, path);
        return bLoaded;
    }
    
    bool isLoaded() {
        return bLoaded;
    }
    
    
private:
    stringstream ss;
    string prefix, suffix;
    int numWidth;
    bool bLoaded = false;
};
