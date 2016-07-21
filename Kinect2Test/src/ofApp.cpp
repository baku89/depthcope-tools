#include "ofApp.h"

#define depth_SCALE 1000

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetWindowShape(GUI_WIDTH + DEPTH_WIDTH * 3, 1280);
	
	kinect.open(true, true, 0);
	kinect.start();
    
	depthPixels.allocate(DEPTH_WIDTH, DEPTH_HEIGHT, OF_IMAGE_COLOR);
	testPixels.allocate(DEPTH_WIDTH, DEPTH_HEIGHT, OF_IMAGE_COLOR);
	testFilledPixels.allocate(DEPTH_WIDTH, DEPTH_HEIGHT, OF_IMAGE_COLOR);
    depthImage.allocate(DEPTH_WIDTH, DEPTH_HEIGHT, OF_IMAGE_COLOR);
	depthFiller.allocate(DEPTH_WIDTH, DEPTH_HEIGHT);
	
    recorder.setFormat("exr");
    
    this->initScene();
	this->loadGui();
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
    camera.setPosition(200, 200, -300);
    camera.lookAt(ofVec3f());
    camera.setFov(70);
    
    // init shader
	depthShader.setupShaderFromFile(GL_FRAGMENT_SHADER, "depth.frag");
	depthShader.linkProgram();
	filledShader.setupShaderFromFile(GL_FRAGMENT_SHADER, "filled.frag");
	filledShader.linkProgram();
    pointShader.load("point");
}

//--------------------------------------------------------------
void ofApp::loadGui() {
	gui = new ofxDatGui(ofxDatGuiAnchor::TOP_LEFT);
	gui->addFRM();
	gui->addBreak()->setHeight(10.0f);
	
	gui->addSlider("near", 0, 800)->bind(near);
	gui->addSlider("far", 0, 800)->bind(far);
	gui->addSlider("fill radius", 1, 15)->bind(postProcessing.radius);
	gui->setTheme(new ofxDatGuiThemeMidnight());
	
	// load
	ofxXmlSettings settings;
	settings.loadFile("settings.xml");
	
	near = settings.getValue("near", 50.0);
	far	 = settings.getValue("far", 500.0);
	postProcessing.radius = settings.getValue("fillRadius", 3.0);
}

void ofApp::saveGui() {
	
	ofxXmlSettings settings;
	settings.addValue("near", near);
	settings.addValue("far", far);
	settings.addValue("fillRadius", postProcessing.radius);
	settings.saveFile("settings.xml");
	
}

//--------------------------------------------------------------
void ofApp::update(){
	
	kinect.update();
	if (kinect.isFrameNew()) {
        
        fov.set(kinect.getFov());
		
        
        int w = DEPTH_WIDTH;
        int h = DEPTH_HEIGHT;
        int x, y, offset;
		float value, invalid;
		
        for (y = 0; y < h; y++) {
            for (x = 0; x < w; x++) {
                
                float dist = kinect.getDistanceAt(x, y);
                offset = (y * w + x) * 3;
                
                if(near < dist && dist < far) {
                    value = ofMap(dist, far, near, 0.0f, 1.0f, true);
                    invalid = 0.0f;
                } else {
                    value = 0.0f;
                    invalid = 1.0f;
                    

                }
                depthPixels[offset    ] = value;
				depthPixels[offset + 1] = invalid;
				depthPixels[offset + 2] = 0.0f;
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
    postProcessing.setup(takeName);
    postProcessing.startThread();
}

//--------------------------------------------------------------
void ofApp::exit() {
	saveGui();
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
		ofSetColor(0);
		ofDrawRectangle(0, 0, DEPTH_WIDTH * 3, DEPTH_HEIGHT);
		
		ofSetColor(255);
		depthShader.begin();
		{
			depthImage.draw(0, 0);
			
			if (testImage.isAllocated()) {
				testImage.draw(DEPTH_WIDTH, 0);
			}
		}
		depthShader.end();
		filledShader.begin();
		{
			if (testFilledImage.isAllocated()) {
				testFilledImage.draw(DEPTH_WIDTH * 2, 0);
			}
		}
		filledShader.end();
		
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
	
	ofPushMatrix();
	{
		ofTranslate(GUI_WIDTH, DEPTH_HEIGHT);
		
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
	ofPopMatrix();
    
    
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
				takeName = getTakeName();
                
                recorder.setCounter(0);
                recorder.setPrefix(ofToDataPath(SAVED_DIR + "/" + takeName + "/" + RAW_NAME + "/" + takeName + "_" + RAW_NAME + "_"));
                
                ofLogNotice() << "TakeName:" << takeName;
                
                recorder.startThread();
            } else {
//                recorder.stopThread();
                willStopRecording = true;
                ofLogNotice() << "End Recording frames:" << recorder.counter;
            }
            break;
			
		case 't':
			// test shooting
			testPixels.setFromPixels(depthPixels.getData(), DEPTH_WIDTH, DEPTH_HEIGHT, 3);
			depthFiller.radius = postProcessing.radius;
			testFilledPixels = depthFiller.inpaint(testPixels);
			
			testImage.setFromPixels(testPixels);
			testFilledImage.setFromPixels(testFilledPixels);
			break;
    }
}

//--------------------------------------------------------------
string ofApp::getTakeName() {
	return ofToString(ofGetYear(), 4, '0') + "-"
		+ ofToString(ofGetMonth(), 2, '0') + "-"
		+ ofToString(ofGetDay(), 2, '0') + "-"
		+ ofToString(ofGetHours(), 2, '0') + "-"
		+ ofToString(ofGetMinutes(), 2, '0') + "-"
		+ ofToString(ofGetSeconds(), 2, '0');
} 
