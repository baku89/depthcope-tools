#include "ofApp.h"

#define depth_SCALE 1000

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetVerticalSync(true);
    ofSetFrameRate(60);
    ofSetWindowShape(GUI_WIDTH + DEPTH_WIDTH * 4, DEPTH_HEIGHT * 4);
    ofSetWindowTitle("Calibration Control");
    ofSetWindowPosition(0, 0);
    ofEnableAlphaBlending();
    ofSetCircleResolution(12);
    ofBackground(0);
	
    // init kinect
	kinect.open(true, true, 0);
    kinect.setEnableFlipBuffer(true);
	kinect.start();
    
    kpt.loadCalibration("calibration.xml");
    
    // load shader
    depthShader.setupShaderFromFile(GL_FRAGMENT_SHADER, "depth.frag");
    depthShader.linkProgram();
    
    irShader.setupShaderFromFile(GL_FRAGMENT_SHADER, "ir.frag");
    irShader.linkProgram();
    
    // calibration
    projectorWidth = 1280;
    projectorHeight = 720;
    
    // setup the second window
    secondWindow.setup("Projector", -projectorWidth, 0, projectorWidth, projectorHeight, true);
    secondWindow.show();

    
	this->loadGui();
    this->initScene();
    
    dmx.connect("/dev/tty.usbserial-EN159284");
    if (isPreviewLight) {
        enableLight();
    } else {
        disableLight();
    }
    
    dfReceiver.setup(1234);
}

//--------------------------------------------------------------
void ofApp::initScene() {
    
    view3d.allocate(DEPTH_WIDTH, DEPTH_HEIGHT);
    
    depthPixels.allocate(DEPTH_WIDTH, DEPTH_HEIGHT, OF_IMAGE_COLOR);
    depthImage.allocate(DEPTH_WIDTH, DEPTH_HEIGHT, OF_IMAGE_COLOR);
    
    mesh.setMode(OF_PRIMITIVE_POINTS);
    
    int x, y;
    int w = DEPTH_WIDTH;
    int h = DEPTH_HEIGHT;
    int step = 2;
    
    for (y = 0; y < h; y += step) {
        for (x = 0; x < w; x += step) {
            mesh.addColor(ofColor(255));
            mesh.addTexCoord(ofVec2f(x / (float)w, y / (float)h));
            mesh.addVertex(ofVec3f(x - w / 2, y - h / 2, 0));
        }
    }
    
    // initalize camera
    camera.setPosition(0, -80, 0);
    camera.lookAt(ofVec3f(0, 0, -100));
    camera.setFov(50);
    
    // init shader
    pointShader.load("point");
}

//--------------------------------------------------------------
void ofApp::loadGui() {
    ofxDatGuiLog::quiet();
    
	gui = new ofxDatGui(ofxDatGuiAnchor::TOP_LEFT);
	gui->addFRM();
    
    gui->addBreak()->setHeight(30.0f);
    ofxDatGuiFolder* scene = gui->addFolder("scene");
    scene->addSlider("near", 0, 300)->bind(near);
    scene->addSlider("far", 0, 300)->bind(far);
    
    gui->addBreak()->setHeight(30.0f);
    ofxDatGuiFolder* plane = gui->addFolder("plane");
    plane->addButton("calibrate plane");
    plane->addToggle("use calibrated")->bind(useCalibrated);
    plane->addSlider("mask margin", 0, 50)->bind(planeMaskMargin);
    plane->addSlider("mask threshold", 0, 400)->bind(planeMaskThreshold);
    plane->expand();
    
    
    gui->addBreak()->setHeight(30.0f);
    ofxDatGuiFolder* shooting = gui->addFolder("shooting");
    shooting->addButton("switch shooting mode");
    shooting->addSlider("feedback rate", 0, 0.2)->bind(kinect.feedbackRate);
    shooting->addToggle("preview light")->bind(isPreviewLight);
    shooting->addSlider("light front", 0, 1)->bind(lightFront);
    shooting->addSlider("light front", 0, 1)->bind(lightBack);
    shooting->expand();
    
    gui->onToggleEvent(this, &ofApp::onToggleEvent);
    gui->onSliderEvent(this, &ofApp::onSliderEvent);
    gui->onButtonEvent(this, &ofApp::onButtonEvent);
    gui->on2dPadEvent(this, &ofApp::on2dPadEvent);
    
	gui->setTheme(new ofxDatGuiThemeMidnight());
    gui->setWidth(GUI_WIDTH);
	
	// load
    {
        ofxJSONElement settings;
        settings.open("settings.json");
        
        mode = settings.get("mode", MODE_CALIB_PLANE).asInt();
        
        // scene
        near = settings["scene"].get("near", 50).asFloat();
        far  = settings["scene"].get("far", 150).asFloat();
        
        // plane
        kOrigin.x = settings["plane"]["origin"]["kinect"].get((Json::ArrayIndex)0, DEPTH_WIDTH / 2).asFloat();
        kOrigin.y = settings["plane"]["origin"]["kinect"].get((Json::ArrayIndex)1, DEPTH_HEIGHT / 2).asFloat();
        wOrigin.x = settings["plane"]["origin"]["world"].get((Json::ArrayIndex)0, 0).asFloat();
        wOrigin.y = settings["plane"]["origin"]["world"].get((Json::ArrayIndex)1, 0).asFloat();
        wOrigin.z = settings["plane"]["origin"]["world"].get((Json::ArrayIndex)2, 100).asFloat();
        
        kAxisX.x = settings["plane"]["axisX"]["kinect"].get((Json::ArrayIndex)0, DEPTH_WIDTH).asFloat();
        kAxisX.y = settings["plane"]["axisX"]["kinect"].get((Json::ArrayIndex)1, DEPTH_HEIGHT / 2).asFloat();
        wAxisX.x = settings["plane"]["axisX"]["world"].get((Json::ArrayIndex)0, 40).asFloat();
        wAxisX.y = settings["plane"]["axisX"]["world"].get((Json::ArrayIndex)1, 0).asFloat();
        wAxisX.z = settings["plane"]["axisX"]["world"].get((Json::ArrayIndex)2, 100).asFloat();
        
        kAxisY.x = settings["plane"]["axisY"]["kinect"].get((Json::ArrayIndex)0, DEPTH_WIDTH / 2).asFloat();
        kAxisY.y = settings["plane"]["axisY"]["kinect"].get((Json::ArrayIndex)1, 0).asFloat();
        wAxisY.x = settings["plane"]["axisY"]["world"].get((Json::ArrayIndex)0, 0).asFloat();
        wAxisY.y = settings["plane"]["axisY"]["world"].get((Json::ArrayIndex)1, 40).asFloat();
        wAxisY.z = settings["plane"]["axisY"]["world"].get((Json::ArrayIndex)2, 100).asFloat();
        
        planeMaskMargin = settings["plane"].get("maskMargin", 5).asFloat();
        planeMaskThreshold = settings["plane"].get("maskThreshold", 0.2).asFloat();
        makePlaneMask();
        
        ofxJSON::ofxJSONElement mat = settings["plane"]["mat"];
        float m00 = mat[ 0].asFloat();
        float m01 = mat[ 1].asFloat();
        float m02 = mat[ 2].asFloat();
        float m03 = mat[ 3].asFloat();
        float m10 = mat[ 4].asFloat();
        float m11 = mat[ 5].asFloat();
        float m12 = mat[ 6].asFloat();
        float m13 = mat[ 7].asFloat();
        float m20 = mat[ 8].asFloat();
        float m21 = mat[ 9].asFloat();
        float m22 = mat[10].asFloat();
        float m23 = mat[11].asFloat();
        float m30 = mat[12].asFloat();
        float m31 = mat[13].asFloat();
        float m32 = mat[14].asFloat();
        float m33 = mat[15].asFloat();
        planeMat.set(m00, m10, m20, m30, m01, m11, m21, m31, m02, m12, m22, m32, m03, m13, m23, m33);
        planeInvMat = planeMat.getInverse();
        
        // shooting
        kinect.feedbackRate = settings["shooting"].get("feedbackRate", 0.1).asFloat();
        
        isPreviewLight = settings["shooting"].get("isPreviewLight", false).asBool();
        lightFront = settings["shooting"].get("lightFront", 1.0).asFloat();
        lightBack  = settings["shooting"].get("lightBack", 1.0).asFloat();
        
        
    }
}

void ofApp::onToggleEvent(ofxDatGuiToggleEvent e) {
    if (e.target->is("preview light")) {
        if (e.target->getChecked()) {
            enableLight();
        } else {
            disableLight();
        }
    }
}

void ofApp::onSliderEvent(ofxDatGuiSliderEvent e) {
    if (e.target->is("light front") || e.target->is("light back")) {
        enableLight();
    }
}

void ofApp::onButtonEvent(ofxDatGuiButtonEvent e) {
    if (e.target->is("calibrate plane")) {
        mode = MODE_CALIB_PLANE;
    } else if (e.target->is("switch shooting mode")) {
        mode = MODE_SHOOTING;
    }
}

void ofApp::on2dPadEvent(ofxDatGui2dPadEvent e) {
    /*
    if (e.target->is("position offset")) {
        ofPoint value = e.target->getPoint();
//        ofLogNotice() << value.x << " x " << value.y;
        kinectProjectorCalibration.setChessboardTranslation(projectorWidth * (value.x - 0.5), projectorHeight * (value.y - 0.5));
    }*/
}

void ofApp::saveGui() {
    
    ofxJSONElement settings;
    
    settings["mode"] = mode;
    
    // scene
    settings["scene"]["near"] = near;
    settings["scene"]["far"]  = far;
    
    // plane
    settings["plane"]["origin"]["kinect"][0] = kOrigin.x;
    settings["plane"]["origin"]["kinect"][1] = kOrigin.y;
    settings["plane"]["origin"]["world"][0] = wOrigin.x;
    settings["plane"]["origin"]["world"][1] = wOrigin.y;
    settings["plane"]["origin"]["world"][2] = wOrigin.z;
    
    settings["plane"]["axisX"]["kinect"][0] = kAxisX.x;
    settings["plane"]["axisX"]["kinect"][1] = kAxisX.y;
    settings["plane"]["axisX"]["world"][0] = wAxisX.x;
    settings["plane"]["axisX"]["world"][1] = wAxisX.y;
    settings["plane"]["axisX"]["world"][2] = wAxisX.z;
    
    settings["plane"]["axisY"]["kinect"][0] = kAxisY.x;
    settings["plane"]["axisY"]["kinect"][1] = kAxisY.y;
    settings["plane"]["axisY"]["world"][0] = wAxisY.x;
    settings["plane"]["axisY"]["world"][1] = wAxisY.y;
    settings["plane"]["axisY"]["world"][2] = wAxisY.z;
    
    settings["plane"]["maskMargin"] = planeMaskMargin;
    settings["plane"]["maskThreshold"] = planeMaskThreshold;
    
    settings["plane"]["mat"][ 0] = planeMat.getRowAsVec4f(0).x;
    settings["plane"]["mat"][ 1] = planeMat.getRowAsVec4f(1).x;
    settings["plane"]["mat"][ 2] = planeMat.getRowAsVec4f(2).x;
    settings["plane"]["mat"][ 3] = planeMat.getRowAsVec4f(3).x;
    settings["plane"]["mat"][ 4] = planeMat.getRowAsVec4f(0).y;
    settings["plane"]["mat"][ 5] = planeMat.getRowAsVec4f(1).y;
    settings["plane"]["mat"][ 6] = planeMat.getRowAsVec4f(2).y;
    settings["plane"]["mat"][ 7] = planeMat.getRowAsVec4f(3).y;
    settings["plane"]["mat"][ 8] = planeMat.getRowAsVec4f(0).z;
    settings["plane"]["mat"][ 9] = planeMat.getRowAsVec4f(1).z;
    settings["plane"]["mat"][10] = planeMat.getRowAsVec4f(2).z;
    settings["plane"]["mat"][11] = planeMat.getRowAsVec4f(3).z;
    settings["plane"]["mat"][12] = planeMat.getRowAsVec4f(0).w;
    settings["plane"]["mat"][13] = planeMat.getRowAsVec4f(1).w;
    settings["plane"]["mat"][14] = planeMat.getRowAsVec4f(2).w;
    settings["plane"]["mat"][15] = planeMat.getRowAsVec4f(3).w;
    
    // shooting
    settings["shooting"]["feedbackRate"] = kinect.feedbackRate;
    
    settings["shooting"]["isPreviewLight"] = isPreviewLight;
    settings["shooting"]["lightFront"] = lightFront;
    settings["shooting"]["lightFront"] = lightBack;
    
    settings.save("settings.json");
	
}

//--------------------------------------------------------------
void ofApp::update(){
	
	kinect.update();
    
    if (kinect.isFrameNew()) {
        ofFloatPixels& irPixels = kinect.getIrPixelsRef();
//        irPixels.mirror(false, true);
        irTex.loadData(irPixels);
        
        
        this->updateScene();
    }
    
    if (mode == MODE_CALIB_PLANE) {
        if (restCalibrationFrames-- > 0) {

            wOrigin += kinect.getWorldCoordinateAt(kOrigin.x, kOrigin.y);
            wAxisX += kinect.getWorldCoordinateAt(kAxisX.x, kAxisX.y);
            wAxisY += kinect.getWorldCoordinateAt(kAxisY.x, kAxisY.y);
            
            // on finished
            if (restCalibrationFrames == 0) {
                wOrigin /= (float)CALIB_FRAMES;
                wAxisX /= (float)CALIB_FRAMES;
                wAxisY /= (float)CALIB_FRAMES;
                
                // make matrix
                ofVec3f ex, ey, ez;
                ex = (wAxisX - wOrigin).getNormalized();
                ey = (wAxisY - wOrigin).getNormalized();
                ez = ex.getCrossed(ey).getNormalized();
                
                planeMat.setTranslation(wOrigin);
                planeMat.set(ex.x, ey.x, ez.x, wOrigin.x,
                             ex.y, ey.y, ez.y, wOrigin.y,
                             ex.z, ey.z, ez.z, wOrigin.z,
                             0, 0, 0, 1);
                planeInvMat = planeMat.getInverse();
                
                useCalibrated = true;
                restCalibrationFrames = -1;
                
                makePlaneMask();
            }
            
        }
        
    } else if (mode == MODE_SHOOTING) {
        
        while (dfReceiver.hasWaitingMessages()) {
            ofxOscMessage m;
            dfReceiver.getNextMessage(m);
            
            string address = m.getAddress();
            int value = m.getArgAsInt(0);
            ofLogNotice() << "address:" << address << " value:" << value;
            
            if (address == "/dragonframe/shoot") {
                doBeforeShoot();
            } else if (address == "/dragonframe/cc") {
                doAfterShoot();
            }
        }
        
    }
}

void ofApp::enableLight() {
    ofLogNotice() << "update dmx" << (dmx.isConnected() ? " connedted" : " not connected")<< " front=" << ofToString(lightFront * 255, 0) << " back=" << ofToString(lightBack * 255, 0);
    dmx.setLevel(1, lightFront * 255);
    dmx.setLevel(2, lightBack * 255);
    dmx.update();
}

void ofApp::disableLight() {
    dmx.setLevel(1, 0);
    dmx.setLevel(2, 0);
    dmx.update();
}

void ofApp::doBeforeShoot() {
    enableLight();
    isShooting = true;
}

void ofApp::doAfterShoot() {
    disableLight();
    isShooting = false;
}

//--------------------------------------------------------------
void ofApp::updateScene() {
    focus.set(kinect.getFocus());
    
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
    
    depthPixels.mirror(false, true);
    depthImage.setFromPixels(depthPixels);
}

//--------------------------------------------------------------
void ofApp::drawScene() {
//    view3d.begin();
    
    ofBackground(0);
    ofSetColor(255);
    
    ofEnableDepthTest();
    camera.begin();
    {
        ofDrawAxis(200);
        
        ofSetColor(255);
        
        pointShader.begin();
        pointShader.setUniform2f("resolution", DEPTH_WIDTH, DEPTH_HEIGHT);
        pointShader.setUniform1f("near", near);
        pointShader.setUniform1f("far", far);
        pointShader.setUniform2f("focus", focus.x, focus.y);
        pointShader.setUniformTexture("depth", depthImage, 0);
        pointShader.setUniformTexture("irTex", irTex, 1);
        pointShader.setUniform1f("planeMaskThreshold", planeMaskThreshold);
        mesh.draw();
        pointShader.end();
        
        ofSetColor(255);
        ofDrawCircle(wOrigin, 0.3);
        
        ofSetColor(255, 0, 0);
        ofDrawCircle(wAxisX, 0.3);
        ofDrawLine(planeMat * ofVec3f(), planeMat * ofVec3f(1, 0, 0));
        
        ofSetColor(0, 255, 0);
        ofDrawCircle(wAxisY, 0.3);
        ofDrawLine(planeMat * ofVec3f(), planeMat * ofVec3f(0, 1, 0));
        
        ofSetColor(0, 0, 255);
        ofDrawLine(planeMat * ofVec3f(), planeMat * ofVec3f(0, 0, 1));
    }
    
    ofSetColor(255);
    
    camera.end();
    ofDisableDepthTest();
}

//--------------------------------------------------------------
void ofApp::exit() {
	saveGui();
}

//--------------------------------------------------------------
string ofApp::getCurrentFeatureName() {
    switch (currentFeature) {
        case F_ORIGIN:
            return "origin";
        case F_AXIS_X:
            return "axis x";
        case F_AXIS_Y:
            return "axis y";
    }
}

void ofApp::makePlaneMask() {
    if (!planeMaskPixels.isAllocated()) {
        planeMaskPixels.allocate(DEPTH_WIDTH, DEPTH_HEIGHT, 1);
    }
    
    float radius = max(kOrigin.distance(kAxisX), kOrigin.distance(kAxisY)) + planeMaskMargin;
    
    ofVec2f p;
    for (p.y = 0; p.y < DEPTH_HEIGHT; p.y++) {
        for (p.x = 0; p.x < DEPTH_WIDTH; p.x++) {
            float dist = kOrigin.distance(p);
            planeMaskPixels[p.y * DEPTH_WIDTH + p.x] = (dist <= radius) ? 255 : 0;
        }
    }
    
    planeMaskImage.setFromPixels(planeMaskPixels);
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofClear(10);
    ofSetColor(255);
    ofPushMatrix();
    
    ofTranslate(GUI_WIDTH, 0);
    
    ofPushMatrix();
    
    this->drawScene();
    
    // draw kinect
    if (irTex.isAllocated()) {
        irShader.begin();
        irTex.draw(0, 0);
        irShader.end();
        
        ofDrawLine(DEPTH_WIDTH / 2, 0, DEPTH_WIDTH / 2, DEPTH_HEIGHT);
        ofDrawLine(0, DEPTH_HEIGHT / 2, DEPTH_WIDTH, DEPTH_HEIGHT / 2);
    }
    
    if (planeMaskImage.isAllocated()) {
        ofSetColor(255, 255, 0, 30);
        planeMaskImage.draw(0, 0);
        ofSetColor(255);
    }
    
//    view3d.draw(DEPTH_WIDTH, 0);
    
    ofTranslate(0, DEPTH_HEIGHT);
    
    if (mode == MODE_CALIB_PLANE) {
        
        drawCalibration();

    } else if (mode == MODE_SHOOTING) {
        
        drawShooting();
    }
    
    ofPopMatrix();
}

void ofApp::drawCalibration() {
    
    ofVec3f wo = useCalibrated ? wOrigin : kinect.getWorldCoordinateAt(kOrigin.x, kOrigin.y);
    ofVec3f wx = useCalibrated ? wAxisX : kinect.getWorldCoordinateAt(kAxisX.x, kAxisX.y);
    ofVec3f wy = useCalibrated ? wAxisY : kinect.getWorldCoordinateAt(kAxisY.x, kAxisY.y);
    
    
    ofTranslate(0, 20);
    ofDrawBitmapString("MODE: calibrating plane", 10, 0);
    
    ofTranslate(0, 20);
    ofDrawBitmapString("EDITING: " + getCurrentFeatureName(), 10, 0);
    
    ofTranslate(0, 20);
    ofDrawBitmapString("KINECT: origin-" + toString(kOrigin) + " axisX-" + toString(kAxisX) + " axisY-" + toString(kAxisY) , 10, 0);
    
    if (restCalibrationFrames > 0) {
        
        ofSetColor(255, 0, 0);
        ofTranslate(0, 20);
        ofDrawBitmapString("calibrating..." + ofToString(restCalibrationFrames) + "frames", 10, 0);
        ofSetColor(255);
        
    } else {
        ofTranslate(0, 20);
        ofDrawBitmapString("WORLD: origin-" + toString(wOrigin) + " axisX-" + toString(wAxisX) + " axisY-" + toString(wAxisY) , 10, 0);
        ofTranslate(0, 20);
        
        ofDrawBitmapString("Radius X=" + ofToString((wx - wo).length(), 1) +
                           "Y=" + ofToString((wy - wo).length(), 1), 10, 0);
    }
    
    ofTranslate(0, 20);
    ofDrawBitmapString(useCalibrated ? "USE CALIBRATED" : "REALTIME", 10, 0);
    
    // draw point
    ofPopMatrix();
    
    ofSetColor(255, 255, 255); ofDrawCircle(kOrigin.x, kOrigin.y, 3);
    ofSetColor(255, 0, 0); ofDrawCircle(kAxisX.x, kAxisX.y, 3);
    ofSetColor(0, 255, 0); ofDrawCircle(kAxisY.x, kAxisY.y, 3);
    
    secondWindow.begin();
    {
        ofVec3f ex = wx - wo, ey = wy - wo;
        
        ofVec2f po = getProj(wo), px = getProj(wx), py = getProj(wy);
        
        ofBackground(50, 0, 0);
        ofSetColor(255, 255, 255);
        ofDrawCircle(po, 3);
        
        ofSetColor(255, 0, 0);
        ofDrawCircle(px, 3);
        ofDrawLine(po, px);
        
        ofSetColor(0, 255, 0);
        ofDrawCircle(py, 3);
        ofDrawLine(po, py);
        
        // draw circle
        ofSetColor(255);
        ofNoFill();
        
        ofVec2f prev = getProj(wx), curt;
        for (int i = 1; i <= 60; i++) {
            float angle = (float)(i / 60.0) * 2 * PI;
            curt.set(getProj(wo + ex * cos(angle) + ey * sin(angle)));
            ofDrawLine(prev, curt);
            prev.set(curt);
        }
        ofFill();
        
        ofDrawLine(projectorWidth / 2, 0, projectorWidth / 2, projectorHeight);
        ofDrawLine(0, projectorHeight / 2, projectorWidth, projectorHeight / 2);
        
        // draw center
        ofSetColor(255, 255, 0);
        ofDrawCircle(getProj(kinect.getWorldCoordinateAt(DEPTH_WIDTH / 2, DEPTH_HEIGHT / 2)), 3);

    }
    secondWindow.end();
}

void ofApp::drawShooting() {
    
    
    secondWindow.begin();
    {
        ofBackground(0);
        ofPushMatrix();
        
        if (irTex.isAllocated() && !isShooting && !isPreviewLight) {
            
            int step = 1;
            int offset = 0;
            ofVec2f kCoord, pCoord;
            ofVec3f wCoord, dCoord;
            float h, dist;
            
            ofFloatPixelsRef irPixels = kinect.getIrPixelsRef();
            
            for (kCoord.y = 0; kCoord.y < DEPTH_HEIGHT; kCoord.y += step) {
                for (kCoord.x = 0; kCoord.x < DEPTH_WIDTH; kCoord.x += step) {
                    offset = kCoord.y * DEPTH_WIDTH + kCoord.x;
                    if (planeMaskPixels[offset] == 0 || irPixels[offset] < planeMaskThreshold) {
                        continue;
                    }
                    
                    wCoord.set( kinect.getWorldCoordinateAt(kCoord.x, kCoord.y) );
                    pCoord = getProj(wCoord);
                    
                    dCoord = wCoord - wOrigin;
                    
                    h = getPlane(wCoord).z;
                    dist = abs(h - TH);
                    
                    if (h < 0 || 20 < h) {
                        
                    } else {
                        
                        if (dist < 0.3) {
                            ofSetColor(0, 255, 0);
                        } else {
                            float lum = ofMap(dist, 0, 10, 0, 255);
                            if (h > TH) { // too high
                                ofSetColor(255, 0, 0);
                            } else { // too low
                                ofSetColor(0, 0, 255);
                            }
                        }
                        ofDrawRectangle(pCoord, 1, 1);
                    }
                }
            }
            
            ofSetColor(255);
            
        }
        
        ofPopMatrix();
    }
    secondWindow.end();
}

//--------------------------------------------------------------
ofVec2f ofApp::getProj(ofVec3f wc) {
    return kpt.getProjectedPoint(wc * ofVec3f(1, 1, -1)) * ofVec2f(projectorWidth, projectorHeight) - ofVec2f(96, 0);

}

ofVec3f ofApp::getPlane(ofVec3f wc) {
    return planeInvMat * wc;
}

//--------------------------------------------------------------
string ofApp::toString(ofVec2f value) {
    return "(" + ofToString(value.x, 1) + ", " + ofToString(value.y, 1) + ")";
}

string ofApp::toString(ofVec3f value) {
    return "(" + ofToString(value.x, 1) + ", " + ofToString(value.y, 1) + ", " + ofToString(value.z, 1) + ")";
}

//--------------------------------------------------------------
void ofApp::moveFeature(float x, float y) {
    float amp = 1.0;
    if (altPressed) amp = 0.1;
    else if (shiftPressed) amp = 10.0;
    
    if (currentFeature == F_ORIGIN) {
        kOrigin += ofVec2f(x, y) * amp;
    } else if (currentFeature == F_AXIS_X) {
        kAxisX += ofVec2f(x, y) * amp;
    } else if (currentFeature == F_AXIS_Y) {
        kAxisY += ofVec2f(x, y) * amp;
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    if (key == OF_KEY_LEFT_ALT) {
        altPressed = true;
        ofLogNotice() << "alt!";
    }
    if (key == OF_KEY_LEFT_SHIFT) {
        shiftPressed = true;
    }
    
    if (mode == MODE_CALIB_PLANE) {
        
        switch (key) {
            case OF_KEY_LEFT:   moveFeature(-1,  0); break;
            case OF_KEY_UP:     moveFeature( 0, -1); break;
            case OF_KEY_RIGHT:  moveFeature(+1,  0); break;
            case OF_KEY_DOWN:   moveFeature( 0, +1); break;
                
            case '1': currentFeature = F_ORIGIN; break;
            case '2': currentFeature = F_AXIS_X; break;
            case '3': currentFeature = F_AXIS_Y; break;
                
            case 'c':
                wOrigin.set(0, 0, 0);
                wAxisX.set(0, 0, 0);
                wAxisY.set(0, 0, 0);
                restCalibrationFrames = CALIB_FRAMES;
                break;
            
            case 'd':
                useCalibrated = !useCalibrated;
                break;
        }
        
    } else if (mode == MODE_SHOOTING) {
        
        switch (key) {
        }
        
    }
    

    
    switch (key) {
        case 'f':
            ofToggleFullscreen();
            break;
    }
    
}

void ofApp::keyReleased(int key) {
    if (key == OF_KEY_LEFT_ALT) {
        altPressed = false;
    }
    
    if (key == OF_KEY_LEFT_SHIFT) {
        shiftPressed = false;
    }
}