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
                       
	
//	ofEnableNormalizedTexCoords();
	
    /*
    plane.set(DEPTH_WIDTH, DEPTH_HEIGHT);
    plane.setPosition(DEPTH, 240, 0);
    plane.setResolution(DEPTH_WIDTH / step, DEPTH_HEIGHT / step);
    sphere.set(300, 48);
    shader.load("shader");
    */
    
	// initialize cam
    /*
	camera.setPosition(0, 0, -500);
	camera.lookAt(ofVec3f());
	camera.setFov(70);
	
	ofSetSmoothLighting(true);
	
	material.setShininess(120);
	material.setSpecularColor(ofColor(255, 255, 255, 255));
	
	light.setDiffuseColor(ofFloatColor(1, 1, 1));
	light.setSpecularColor(ofFloatColor(1, 1, 1));
	light.setPosition(-300, 300, -300);
    */
    
    
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
        
        dispImage.setFromPixels(dispPixels);
        
        if (isRecording) {
            recorder.addFrame(dispImage);
        }
        
        
//        ofLogNotice() << depthTex.getWidth() << " " << depthTex.getHeight();
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
    
    dispImage.draw(0, 0);
    depthTex.draw(DEPTH_WIDTH, 0);
    
    if (isRecording) {
        
        ofSetColor(255, 0, 0);
        ofDrawCircle(40, 40, 10);
    }
    
    ofPopMatrix();

//
//	ofBackground(0);
//    
//    ofPushMatrix();
//    
//    ofTranslate(GUI_WIDTH, 0);
//    
////    dispImage.draw(0, 0);
//    
//    dispFbo.draw(0, 0);
    
    /*
    
	ofEnableDepthTest();
	camera.begin();
	{
		// 1. draw util
		ofDrawAxis(400);
		ofDrawGrid(80.0f, 6);
		light.draw();
		
		// 2. draw kinect
//		ofEnableLighting();
//		light.enable();
		
		if (depthTex.isAllocated()) {
        
            
			shader.begin();
			shader.setUniform1f("time", ofGetElapsedTimef());
            shader.setUniform2f("resolution", depthTex.getWidth(), depthTex.getHeight());
//            shader.setUniform2f("center", center.x, center.y);
//            shader.setUniform2f("fov", fov.x, fov.y);
//			shader.setUniformTexture("depth", depthTex, 0);
            shader.setUniformTexture("depth", dispImage.getTexture(), 0);
            
            plane.draw();
            
			shader.end();
            
		}
//		light.disable();
//		ofDisableLighting();
		
	}
	camera.end();
	ofDisableDepthTest();
     */
    
//    kinect.getDepthPixelsRef();
    
//    depthTex.draw(0, 0);
    
    
//    dispImage.setFromPixels(dispPixels);
//    dispImage.draw(0, 0, DEPTH_WIDTH * 2, DEPTH_HEIGHT * 2);
    
}

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
