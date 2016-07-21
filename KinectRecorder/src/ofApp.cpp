#include "ofApp.h"

#define DISP_SCALE 100

//--------------------------------------------------------------
void ofApp::setup(){
	
	kinect.open(true, true, 0);
	kinect.start();
    
    dispPixels.allocate(DEPTH_WIDTH / step, DEPTH_HEIGHT / step, OF_IMAGE_COLOR);
    dispImage.allocate(DEPTH_WIDTH / step, DEPTH_HEIGHT / step, OF_IMAGE_COLOR);
    
    recorder.setPrefix(ofToDataPath("recording1/frame_"));
    recorder.setFormat("png");
    
    
    gui = new ofxDatGui(ofxDatGuiAnchor::TOP_LEFT);
    gui->addFRM();
    gui->addBreak()->setHeight(10.0f);
    
    gui->addSlider("near", 0, 800)->bind(near);
    gui->addSlider("far", 0, 800)->bind(far);
    


}

//--------------------------------------------------------------
void ofApp::update(){
	
	kinect.update();
	if (kinect.isFrameNew()) {
		depthTex.loadData(kinect.getDepthPixelsRef());
        
//        center.set(kinect.getCenter());
//        fov.set(kinect.getFov());
        
        /*
        int w = DEPTH_WIDTH / step;
        int h = DEPTH_HEIGHT / step;
        
        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                float dist = kinect.getDistanceAt(x * step, y * step);
                if(near < dist && dist < far) {
                    ofVec3f pt = kinect.getWorldCoordinateAt(x * step, y * step, dist);
                    dispPixels[(y * w + x) * 3    ] = ofMap(pt.x, -DISP_SCALE, DISP_SCALE, 0.0f, 1.0f, true);
                    dispPixels[(y * w + x) * 3 + 1] = ofMap(pt.y, -DISP_SCALE, DISP_SCALE, 0.0f, 1.0f, true);
                    dispPixels[(y * w + x) * 3 + 2] = ofMap(pt.z, -DISP_SCALE, DISP_SCALE, 0.0f, 1.0f, true);
                } else {
                    dispPixels[(y * w + x) * 3    ] = 0.5f;
                    dispPixels[(y * w + x) * 3 + 1] = 0.5f;
                    dispPixels[(y * w + x) * 3 + 2] = 0.5f;
                }
            }
        }
        */
        
        if (isRecording) {
//            recorder.addFrame(dispImage);
        }
	}

}

//--------------------------------------------------------------
void ofApp::exit() {
    recorder.waitForThread();
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    ofBackground(0);
    ofSetColor(255);
    
    ofPushMatrix();
    ofTranslate(GUI_WIDTH, 0);
    {
        
        dispImage.draw(0, 0);
        depthTex.draw(DEPTH_WIDTH, 0);
        
        if (isRecording) {
            
            ofSetColor(255, 0, 0);
            ofDrawCircle(40, 40, 10);
        }
        
    }
    ofPopMatrix();}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    switch (key) {
        case 'r':
            isRecording = !isRecording;
            break;
        case 't':
            if (recorder.isThreadRunning()) {
                recorder.stopThread();
            } else {
                recorder.startThread();
            }
            break;
            
    }

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
