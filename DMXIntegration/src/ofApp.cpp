#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	
	ofSetWindowTitle("TECHNE Interface");
	ofSetWindowShape(320, 320);
	
	dfReceiver.setup(DF_PORT);
	c4dReceiver.setup(FROM_C4D_PORT);
	c4dSender.setup("localhost", TO_C4D_PORT);
	
	gui = new ofxDatGui(ofxDatGuiAnchor::TOP_LEFT);
	
	gui->addSlider("position", 0, 500)->bind(position);
	
	gui->addBreak()->setHeight(10.0f);
	gui->addSlider("rotation", 0, PI * 2)->bind(rotation);
	gui->addSlider("fill light", 0, 1)->bind(fillLight);
	gui->addSlider("lim light", 0, 1)->bind(limLight);
}

//--------------------------------------------------------------
void ofApp::update(){
	
	while (dfReceiver.hasWaitingMessages()) {
		ofxOscMessage m;
		dfReceiver.getNextMessage(&m);
		
		string address = m.getAddress();
		int value = m.getArgAsInt(0);
//		ofLogNotice() << "address:" << address << " value:" << value;
		
		if (address == "/dragonframe/position") {
			position = value;
			
			ofxOscMessage sm;
			sm.setAddress("/position");
			sm.addIntArg(position);
			c4dSender.sendMessage(sm);
		}
	}
	
	while (c4dReceiver.hasWaitingMessages()) {
		ofxOscMessage m;
		c4dReceiver.getNextMessage(&m);
		
		/*
		int p = m.getArgAsInt(0);
		if (position != p) {
			ofSystemAlertDialog("Error");
		}
		*/
		
		rotation = m.getArgAsFloat(1);
		fillLight = m.getArgAsFloat(2);
		limLight = m.getArgAsFloat(3);
	}

}

//--------------------------------------------------------------
void ofApp::draw(){
	
	ofBackground(0);
	

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

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
