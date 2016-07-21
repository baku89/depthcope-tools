#include "ofApp.h"

#define depth_SCALE 1000

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetWindowShape(1280, 1280);
	
	kinect.open(true, true, 0);
	kinect.start();
    
    depthPixels.allocate(DEPTH_WIDTH, DEPTH_HEIGHT, OF_IMAGE_COLOR_ALPHA);
    depthImage.allocate(DEPTH_WIDTH, DEPTH_HEIGHT, OF_IMAGE_COLOR_ALPHA);
    
    recorder.setPrefix(ofToDataPath("recording1/frame_"));
    recorder.setFormat("exr");
    
    this->initScene();
    
    gui = new ofxDatGui(ofxDatGuiAnchor::TOP_LEFT);
    gui->addFRM();
    gui->addBreak()->setHeight(10.0f);
    
    gui->addSlider("near", 0, 800)->bind(near);
    gui->addSlider("far", 0, 800)->bind(far);
}

//--------------------------------------------------------------
void ofApp::initScene() {
    
    mesh.setMode(OF_PRIMITIVE_POINTS);
    
    int x, y;
    int w = DEPTH_WIDTH;
    int h = DEPTH_HEIGHT;
    
    for (y = 0; y < h; y++) {
        for (x = 0; x < w; x++) {
            mesh.addColor(ofColor(255));
            mesh.addTexCoord(ofVec2f(x / (float)w, y / (float)h));
            mesh.addVertex(ofVec3f(x - w / 2, y - h / 2, 0));
        }
    }
    
    // initalize camera
    camera.setPosition(300, 300, 300);
    camera.lookAt(ofVec3f());
    camera.setFov(70);
    
    // init shader
    pointShader.load("point");
}

//--------------------------------------------------------------
void ofApp::update(){
	
	kinect.update();
	if (kinect.isFrameNew()) {
        
        fov.set(kinect.getFov());
        
        colorImage.setFromPixels(kinect.getColorPixelsRef());
        
        int w = DEPTH_WIDTH;
        int h = DEPTH_HEIGHT;
        int x, y, offset;
        float value;
        
        for (y = 0; y < h; y++) {
            for (x = 0; x < w; x++) {
                
                float dist = kinect.getDistanceAt(x, y);
                offset = (y * w + x) * 4;
                
                if(near < dist && dist < far) {
                    value = ofMap(dist, far, near, 0.0f, 1.0f, true);
                    depthPixels[offset + 3] = 1.0f;
                } else {
                    value = 0.0f;
                    depthPixels[offset + 3] = 0.0f;
                    

                }
                depthPixels[offset    ] = value;
                depthPixels[offset + 1] = value;
                depthPixels[offset + 2] = value;
            }
        }
        
        depthImage.setFromPixels(depthPixels);
        
        
        if (isRecording) {
            recorder.addFrame(depthPixels);
        }
	}
    
    if (willStopRecording && recorder.q.size() == 0) {
        recorder.stopThread();
        willStopRecording = false;
        
        doPostProcessing();
    }
}

//--------------------------------------------------------------
void ofApp::doPostProcessing() {
    postProcessing.setup(dirname);
    postProcessing.startThread();
}

//--------------------------------------------------------------
void ofApp::exit() {
    recorder.waitForThread();
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    ofBackground(0);
    
    // draw 3d
    this->drawScene();
    
    
    // draw 2d
    ofPushMatrix();
    ofTranslate(GUI_WIDTH, 0);
    {
        ofSetColor(255);
        depthImage.draw(0, 0, DEPTH_WIDTH / 2, DEPTH_HEIGHT / 2);
//        colorImage.draw(DEPTH_WIDTH, 0);
        
        if (isRecording) {
            ofSetColor(255, 0, 0);
            ofDrawCircle(40, 40, 10);
            ss.str("");
            ss << "recording: "  << takeName << "_" << ofToString(recorder.counter, 6, '0');
            ofDrawBitmapString(ss.str(), 50, 40);
        } else if (willStopRecording) {
            ofSetColor(255, 0, 255);
            ofDrawCircle(40, 40, 10);
            ofDrawBitmapString("saving...", 50, 40);
        } else if (postProcessing.isThreadRunning()) {
            ofSetColor(0, 255, 0);
            ofDrawBitmapString("processing...", 50, 40);
            ofDrawRectangle(0, 0, ofGetWidth() * postProcessing.progress, 10);
        }
        
    }
    ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::drawScene() {
    
    camera.begin();
    
    ofDrawAxis(400);

    ofSetColor(255);
    
    pointShader.begin();
    pointShader.setUniform2f("resolution", DEPTH_WIDTH, DEPTH_HEIGHT);
    pointShader.setUniform1f("near", near);
    pointShader.setUniform1f("far", far);
    pointShader.setUniform2f("fov", fov.x, fov.y);
    pointShader.setUniformTexture("depth", depthImage, 0);
    
    mesh.draw();
    
    pointShader.end();
    
    camera.end();
    
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    switch (key) {
        case 'r':
            if (willStopRecording || postProcessing.isThreadRunning()) {
                ofLogError() << "Cannot recording while saving previous frames";
                break;
            }
            
            isRecording = !isRecording;
            if (isRecording) {
                ss.str("");
                ss << ofToString(ofGetYear(), 4, '0') << "-" << ofToString(ofGetMonth(), 2, '0') << "-" << ofToString(ofGetDay(), 2, '0') << "-"
                << ofToString(ofGetHours(), 2, '0') << "-" << ofToString(ofGetMinutes(), 2, '0') << "-" << ofToString(ofGetSeconds(), 2, '0');
                takeName = ss.str();
                
                ss.str("");
                ss << SAVED_DIR << "/" << takeName;
                dirname = ofToDataPath(ss.str());
                
                ss.str("");
                ss << SAVED_DIR << "/" << takeName << "/" << takeName << "_";
                string path = ofToDataPath(ss.str());
                
                recorder.setCounter(0);
                recorder.setPrefix(path);
                
                ofLogNotice() << "TakeName:" << takeName;
                
                recorder.startThread();
            } else {
//                recorder.stopThread();
                willStopRecording = true;
                ofLogNotice() << "End Recording frames:" << recorder.counter;
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
