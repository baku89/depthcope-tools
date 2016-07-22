#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetWindowShape(DEPTH_WIDTH * 3 + GUI_WIDTH, 1280);
    
    loadGui();
    
    loadTake();
}

//--------------------------------------------------------------
void ofApp::loadGui() {
    gui = new ofxDatGui(ofxDatGuiAnchor::TOP_LEFT);

    gui->addSlider("capture near", 0, 300.0)->bind(hmr.captureNear);
    gui->addSlider("capture far", 0, 300.0)->bind(hmr.captureFar);
    gui->addSlider("capture width", 4, DEPTH_WIDTH * 3.0f)->bind(hmr.captureSize.x);
    gui->addSlider("capture height", 4, DEPTH_HEIGHT * 3.0f)->bind(hmr.captureSize.y);
    gui->addSlider("translation x", -0.5, +0.5)->bind(hmr.translation.x);
    gui->addSlider("translation y", -0.5, +0.5)->bind(hmr.translation.y);
    gui->addSlider("scale", 1, 12.0)->bind(hmr.scale);
    
    gui->addBreak()->setHeight(20.0f);
    
    gui->addSlider("time", 0, 0)->bind(currentFrame);
    gui->addToggle("preview")->bind(hmr.isPreview);
    
    gui->addBreak()->setHeight(20.0f);
    
    
    gui->addBreak();
    
    gui->setTheme(new ofxDatGuiThemeMidnight);
    gui->setWidth(GUI_WIDTH);
    gui->onSliderEvent(this, &ofApp::onSliderEvent);
    gui->onToggleEvent(this, &ofApp::onToggleEvent);
    
    ofxXmlSettings settings;
    settings.loadFile(SETTINGS_PATH);
    
    takeName = settings.getValue("takeName", "");
    hmr.captureNear = settings.getValue("capturrNear", 50.0);
    hmr.captureFar = settings.getValue("captureFar", 500.0);
    hmr.captureSize.x = settings.getValue("captureWidth", 640.0);
    hmr.captureSize.y = settings.getValue("captureHeight", 480.0);
    hmr.translation.x = settings.getValue("translationX", 0.0);
    hmr.translation.y = settings.getValue("translationY", 0.0);
    hmr.scale = settings.getValue("scale", 1);
    hmr.isPreview = (bool)settings.getValue("isPreview", (int)true);
}

void ofApp::saveGui() {
    ofxXmlSettings settings;
    
    settings.addValue("takeName", takeName);
    settings.addValue("captureNear", hmr.captureNear);
    settings.addValue("captureFar", hmr.captureFar);
    settings.addValue("captureWidth", hmr.captureSize.x);
    settings.addValue("captureHeight", hmr.captureSize.y);
    settings.addValue("translationX", hmr.translation.x);
    settings.addValue("translationY", hmr.translation.y);
    settings.addValue("scale", hmr.scale);
    settings.addValue("isPreview", (int)hmr.isPreview);
    
    settings.saveFile(SETTINGS_PATH);
}

void ofApp::onSliderEvent(ofxDatGuiSliderEvent e) {
    needsUpdate = true;
    
    string label = e.target->getLabel();
    
    if (label == "TIME") {
        loadFrame();
    }
}

void ofApp::onToggleEvent(ofxDatGuiToggleEvent e) {
    needsUpdate = true;
}

//--------------------------------------------------------------
void ofApp::exit() {
    saveGui();
    
    exporter.waitForThread();
}

//--------------------------------------------------------------
void ofApp::update(){
    
    if (frameList.size() > 0 && needsUpdate) {
        needsUpdate = false;
        hmr.render();
    }
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    ofBackground(0);
    
    ofPushMatrix();
    ofTranslate(GUI_WIDTH, 0);
    {
        
        // image
        ofSetColor(255);
        if (hmr.getTexture().isAllocated()) {
            hmr.getTexture().draw(0, 50, DEPTH_WIDTH / 2, DEPTH_HEIGHT / 2);
        }
        
        hmr.getTexture().draw(DEPTH_WIDTH / 2, 50);
        
        // frame
        ofNoFill();
        ofSetColor(255);
        ofSetLineWidth(LINE_WIDTH);
        ofRectangle(50 + LINE_WIDTH / 2, 0, DEPTH_WIDTH / 2 - LINE_WIDTH, DEPTH_HEIGHT / 2 - LINE_WIDTH);
        ofFill();
        
        // gui
        if (exporter.isThreadRunning()) {
            ofSetColor(0, 0, 255);
            ofDrawRectangle(0, 0, exporter.progress * (ofGetWidth() - GUI_WIDTH), 50);
        }
        
        ofSetColor(255);
        ofDrawBitmapString("Take Name:" + takeName, 10, 15);
        ofDrawBitmapString("Current Frame:" + ofToString(currentFrame) + "/" + ofToString(totalFrame) + " near=" + ofToString(hmr.near, 4) + " far=" + ofToString(hmr.far, 4) + " fx=" + ofToString(hmr.focus.x) + " outputWidth=" + ofToString(hmr.getWidth()) + " outputHeight=" + ofToString(hmr.getHeight()), 10, 35);
    }
    ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::loadTake() {
    ofDirectory dir(ofToDataPath(SAVED_DIR + "/" + takeName + "/" + INPAINTED_NAME));
    dir.allowExt("exr");
    frameList = dir.getFiles();
    
    ofxXmlSettings params;
    params.loadFile(ofToDataPath(SAVED_DIR + "/" + takeName + "/params.xml"));
    hmr.focus.set(params.getValue("fx", 365.0),
                  params.getValue("fy", 365.0));
    hmr.near = params.getValue("near", 0);
    hmr.far  = params.getValue("far", 0);
    
    totalFrame = frameList.size();
    currentFrame = 0;
    
    gui->getSlider("time")->setMax(totalFrame - 1);
    loadFrame();
    ofLogNotice() << "Take Name: " << dir.getAbsolutePath() << " Frame Count: " << totalFrame;

}

void ofApp::loadTakeWithDialog() {
    ofFileDialogResult result = ofSystemLoadDialog("load take", true, ofToDataPath(SAVED_DIR));
    takeName = result.getName();
    
    loadTake();
}

//--------------------------------------------------------------
void ofApp::loadFrame() {
    string path = frameList[currentFrame].getAbsolutePath();
    hmr.loadTexture(path);
    needsUpdate = true;
    ofLogNotice() << "Loading: " << path;
}

//--------------------------------------------------------------
void ofApp::exportTake() {
    if (exporter.isThreadRunning()) {
        exporter.stopThread();
    }
    exporter.setup(hmr, takeName);
    exporter.startThread();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    switch (key) {
        case 'o':
            loadTakeWithDialog();
            break;
            
        case 's':
            exportTake();
            break;
            
        case OF_KEY_LEFT:
            if (--currentFrame < 0) currentFrame = 0;
            loadFrame();
            needsUpdate = true;
            break;
        case OF_KEY_RIGHT:
            if (++currentFrame >= totalFrame) currentFrame = totalFrame - 1;
            loadFrame();
            needsUpdate = true;
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
