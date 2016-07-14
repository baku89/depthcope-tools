#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	
	kinect.init();
	kinect.open();
	
	plane.set(640, 480);
//	plane.setPosition(320, 240, 0);
	plane.setResolution(100, 100);
	
	sphere.set(300, 48);
	
	shader.load("shader");
	
	ofEnableNormalizedTexCoords();
	
	// initialize cam
	camera.setPosition(0, 0, -500);
	camera.lookAt(ofVec3f());
	camera.setFov(70);
	
	ofSetSmoothLighting(true);
	
	material.setShininess(120);
	material.setSpecularColor(ofColor(255, 255, 255, 255));
	
	light.setDiffuseColor(ofFloatColor(1, 1, 1));
	light.setSpecularColor(ofFloatColor(1, 1, 1));
	light.setPosition(-300, 300, -300);
	
	
	


}

//--------------------------------------------------------------
void ofApp::update(){
	
	kinect.update();

}

//--------------------------------------------------------------
void ofApp::draw(){
	
//	kinect.drawDepth(0, 0, kinect.width, kinect.height);
//	kinect.getDepthTexture();
	
	ofBackground(0);
	
	
	
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
		shader.begin();
		
		shader.setUniform1f("time", ofGetElapsedTimef());
		shader.setUniformTexture("depth", kinect.getDepthTexture(), 0);
		{
			
//			kinect.getDepthTexture().bind();
			{
				plane.draw();
			}
//			kinect.getDepthTexture().unbind();
			
		}
		shader.end();
//		light.disable();
//		ofDisableLighting();
		
	}
	camera.end();
	ofDisableDepthTest();
	
	

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
