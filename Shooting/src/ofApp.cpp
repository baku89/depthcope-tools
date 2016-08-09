#include "ofApp.h"

#define depth_SCALE 1000

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetVerticalSync(true);
    ofSetFrameRate(30);
    ofSetWindowShape(1024, 576);
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
    this->initShooting();
}

//--------------------------------------------------------------
void ofApp::initShooting() {
    // init shooting
    
    hud.allocate(140, 120, GL_RGBA);
    
    dmx.connect("/dev/tty.usbserial-EN159284");
    if (isPreviewLight) {
        enableLight();
    } else {
        disableLight();
    }
    
    dfReceiver.setup(1234);
    
    heightmapLoader.setup("/Users/mugi/Works/2016/26_techne/render/heightmap/heightmap_overlay_depth####.exr");
    heightmapPixels.allocate(HEIGHTMAP_WIDTH, HEIGHTMAP_WIDTH, 1);
    heightmapU8Pixels.allocate(HEIGHTMAP_WIDTH, HEIGHTMAP_WIDTH, 1);
    heightmapCvImage.allocate(HEIGHTMAP_WIDTH, HEIGHTMAP_WIDTH);
    
    overlayLoader.setup("/Users/mugi/Works/2016/26_techne/render/heightmap/heightmap_overlay####.exr");
    overlayPixels.allocate(HEIGHTMAP_WIDTH, HEIGHTMAP_WIDTH, 1);
    
    loadHeightmap();
    
    contour.setStrokeColor(255);
    contour.setStrokeWidth(1);
    contour.setFilled(false);
    
    
    int x, y;
    int w = DEPTH_WIDTH;
    int h = DEPTH_HEIGHT;
    int step = 2;
    
    for (y = 0; y < h; y += step) {
        for (x = 0; x < w; x += step) {
            depthPointCloud.addColor(ofColor(255));
            depthPointCloud.addTexCoord(ofVec2f(x / (float)w, y / (float)h));
            depthPointCloud.addVertex(ofVec3f(x - w / 2, y - h / 2, 0));
        }
    }
    
    ofPlanePrimitive plane;
    plane.set(DEPTH_WIDTH - 1, DEPTH_HEIGHT - 1);
    plane.setResolution(DEPTH_WIDTH, DEPTH_HEIGHT);
    
    heightmapMesh = plane.getMesh();
    
    for (y = 0; y < h; y++) {
        for (x = 0; x < w; x++) {
            heightmapMesh.addColor(ofColor(255));
            heightmapMesh.setTexCoord(y * w + x, ofVec2f(x, y));
        }
    }
    
//    heightmapMesh.setPosition(1280/2, 720/2, 0);
    heightmapMesh.setMode(OF_PRIMITIVE_POINTS);
    ofLogNotice() << "vertices=" << heightmapMesh.getVertices().size();
    ofLogNotice() << "indices=" << heightmapMesh.getIndices().size();
    ofLogNotice() << "colors=" << heightmapMesh.getColors().size();
    ofLogNotice() << "texCoords=" << heightmapMesh.getTexCoords().size();
    
    heightmapShader.load("heightmap");
}

//--------------------------------------------------------------
void ofApp::initScene() {
    
    view3d.allocate(DEPTH_WIDTH, DEPTH_HEIGHT);
    
    depthPixels.allocate(DEPTH_WIDTH, DEPTH_HEIGHT, OF_IMAGE_COLOR);
    depthImage.allocate(DEPTH_WIDTH, DEPTH_HEIGHT, OF_IMAGE_COLOR);
    
    depthPointCloud.setMode(OF_PRIMITIVE_POINTS);
    
    int x, y;
    int w = DEPTH_WIDTH;
    int h = DEPTH_HEIGHT;
    int step = 2;
    
    for (y = 0; y < h; y += step) {
        for (x = 0; x < w; x += step) {
            depthPointCloud.addColor(ofColor(255));
            depthPointCloud.addTexCoord(ofVec2f(x / (float)w, y / (float)h));
            depthPointCloud.addVertex(ofVec3f(x - w / 2, y - h / 2, 0));
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
    scene->addToggle("show ir")->bind(isDisplayIr);
	scene->addToggle("show scene")->bind(showScene);
    
    gui->addBreak()->setHeight(30.0f);
    ofxDatGuiFolder* disc = gui->addFolder("disc");
    disc->addButton("calibrate");
    disc->addToggle("use calibrated")->bind(useCalibrated);
    disc->addSlider("mask margin", 0, 50)->bind(discMaskMargin);
    disc->addSlider("mask threshold", 0, 400)->bind(discMaskThreshold);
    disc->expand();
    
    gui->addBreak()->setHeight(30.0f);
    ofxDatGuiFolder* shooting = gui->addFolder("shooting");
    shooting->addButton("switch shooting mode");
    shooting->addSlider("feedback rate", 0, 0.6)->bind(kinect.feedbackRate);
    shooting->addSlider("tolerance (cm)", 0, 1)->bind(tolerance);
    shooting->addToggle("display heightmap")->bind(isDisplayHeightmap);
    shooting->addToggle("preview light")->bind(isPreviewLight);
    shooting->addSlider("light front", 0, 1)->bind(lightFront);
    shooting->addSlider("light back", 0, 1)->bind(lightBack);
    shooting->addSlider("rotate step", 0, 360)->bind(rotateStep);
    shooting->addToggle("enable timer");
	shooting->addToggle("show overlay")->bind(showOverlay);
	shooting->addToggle("show heightmap")->bind(showHeightmap);
	shooting->addToggle("force display")->bind(forceDisplay);
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
        
        mode = settings.get("mode", MODE_CALIBRATION).asInt();
        
        // scene
        near = settings["scene"].get("near", 50).asFloat();
        far  = settings["scene"].get("far", 150).asFloat();
        isDisplayIr = settings["scene"].get("isDisplayIr", true).asBool();
		showScene = settings["scene"].get("showScene", true).asBool();
        
        // disc
        kOrigin.x = settings["disc"]["origin"]["kinect"].get((Json::ArrayIndex)0, DEPTH_WIDTH / 2).asFloat();
        kOrigin.y = settings["disc"]["origin"]["kinect"].get((Json::ArrayIndex)1, DEPTH_HEIGHT / 2).asFloat();
        wOrigin.x = settings["disc"]["origin"]["world"].get((Json::ArrayIndex)0, 0).asFloat();
        wOrigin.y = settings["disc"]["origin"]["world"].get((Json::ArrayIndex)1, 0).asFloat();
        wOrigin.z = settings["disc"]["origin"]["world"].get((Json::ArrayIndex)2, 100).asFloat();
        
        kAxisX.x = settings["disc"]["axisX"]["kinect"].get((Json::ArrayIndex)0, DEPTH_WIDTH).asFloat();
        kAxisX.y = settings["disc"]["axisX"]["kinect"].get((Json::ArrayIndex)1, DEPTH_HEIGHT / 2).asFloat();
        wAxisX.x = settings["disc"]["axisX"]["world"].get((Json::ArrayIndex)0, 40).asFloat();
        wAxisX.y = settings["disc"]["axisX"]["world"].get((Json::ArrayIndex)1, 0).asFloat();
        wAxisX.z = settings["disc"]["axisX"]["world"].get((Json::ArrayIndex)2, 100).asFloat();
        
        kAxisY.x = settings["disc"]["axisY"]["kinect"].get((Json::ArrayIndex)0, DEPTH_WIDTH / 2).asFloat();
        kAxisY.y = settings["disc"]["axisY"]["kinect"].get((Json::ArrayIndex)1, 0).asFloat();
        wAxisY.x = settings["disc"]["axisY"]["world"].get((Json::ArrayIndex)0, 0).asFloat();
        wAxisY.y = settings["disc"]["axisY"]["world"].get((Json::ArrayIndex)1, 40).asFloat();
        wAxisY.z = settings["disc"]["axisY"]["world"].get((Json::ArrayIndex)2, 100).asFloat();
        
        discMaskMargin = settings["disc"].get("maskMargin", 5).asFloat();
        discMaskThreshold = settings["disc"].get("maskThreshold", 0.2).asFloat();
        makeDiscMask();
        
        ofxJSON::ofxJSONElement mat = settings["disc"]["mat"];
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
        discMat.set(m00, m10, m20, m30, m01, m11, m21, m31, m02, m12, m22, m32, m03, m13, m23, m33);
        discInvMat = discMat.getInverse();
        
        // shooting
        kinect.feedbackRate = settings["shooting"].get("feedbackRate", 0.1).asFloat();
        
        tolerance = settings["shooting"].get("tolerance", 0.5).asFloat();
        isDisplayHeightmap = settings["shooting"].get("isDisplayHeightmap", true).asBool();
        
        isPreviewLight = settings["shooting"].get("isPreviewLight", false).asBool();
        lightFront = settings["shooting"].get("lightFront", 1.0).asFloat();
        lightBack  = settings["shooting"].get("lightBack", 1.0).asFloat();
        
        currentFrame = settings["shooting"].get("currentFrame", -1).asInt();
        hudOrigin.set(
                          settings["shooting"]["hudOrigin"].get((Json::ArrayIndex)0, 0).asFloat(),
                          settings["shooting"]["hudOrigin"].get((Json::ArrayIndex)1, 0).asFloat());
        
        rotateStep = settings["shooting"].get("rotateStep", 1).asFloat();
        
        timer.toggle( settings["shooting"].get("enableTimer", false).asBool() );
        timer.reset( settings["shooting"].get("elapsedTime", 0).asFloat() );
		showOverlay = settings["shooting"].get("showOverlay", true).asBool();
		showHeightmap = settings["shooting"].get("showHeightmap", true).asBool();
		forceDisplay = settings["shoting"].get("forceDisplay", false).asBool();
    }
}

void ofApp::onToggleEvent(ofxDatGuiToggleEvent e) {
    
    bool value = e.target->getChecked();
    
    if (e.target->is("preview light")) {
        if (value) {
            enableLight();
        } else {
            disableLight();
        }
    } else if (e.target->is("enable timer")) {
        timer.toggle(value);
    }
}

void ofApp::onSliderEvent(ofxDatGuiSliderEvent e) {
    if (e.target->is("light front") || e.target->is("light back")) {
        enableLight();
    }
}

void ofApp::onButtonEvent(ofxDatGuiButtonEvent e) {
    if (e.target->is("calibrate disc")) {
        mode = MODE_CALIBRATION;
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
    settings["scene"]["isDisplayIr"] = isDisplayIr;
	settings["scene"]["showScene"] = showScene;
    
    // disc
    settings["disc"]["origin"]["kinect"][0] = kOrigin.x;
    settings["disc"]["origin"]["kinect"][1] = kOrigin.y;
    settings["disc"]["origin"]["world"][0] = wOrigin.x;
    settings["disc"]["origin"]["world"][1] = wOrigin.y;
    settings["disc"]["origin"]["world"][2] = wOrigin.z;
    
    settings["disc"]["axisX"]["kinect"][0] = kAxisX.x;
    settings["disc"]["axisX"]["kinect"][1] = kAxisX.y;
    settings["disc"]["axisX"]["world"][0] = wAxisX.x;
    settings["disc"]["axisX"]["world"][1] = wAxisX.y;
    settings["disc"]["axisX"]["world"][2] = wAxisX.z;
    
    settings["disc"]["axisY"]["kinect"][0] = kAxisY.x;
    settings["disc"]["axisY"]["kinect"][1] = kAxisY.y;
    settings["disc"]["axisY"]["world"][0] = wAxisY.x;
    settings["disc"]["axisY"]["world"][1] = wAxisY.y;
    settings["disc"]["axisY"]["world"][2] = wAxisY.z;
    
    settings["disc"]["maskMargin"] = discMaskMargin;
    settings["disc"]["maskThreshold"] = discMaskThreshold;
    
    settings["disc"]["mat"][ 0] = discMat.getRowAsVec4f(0).x;
    settings["disc"]["mat"][ 1] = discMat.getRowAsVec4f(1).x;
    settings["disc"]["mat"][ 2] = discMat.getRowAsVec4f(2).x;
    settings["disc"]["mat"][ 3] = discMat.getRowAsVec4f(3).x;
    settings["disc"]["mat"][ 4] = discMat.getRowAsVec4f(0).y;
    settings["disc"]["mat"][ 5] = discMat.getRowAsVec4f(1).y;
    settings["disc"]["mat"][ 6] = discMat.getRowAsVec4f(2).y;
    settings["disc"]["mat"][ 7] = discMat.getRowAsVec4f(3).y;
    settings["disc"]["mat"][ 8] = discMat.getRowAsVec4f(0).z;
    settings["disc"]["mat"][ 9] = discMat.getRowAsVec4f(1).z;
    settings["disc"]["mat"][10] = discMat.getRowAsVec4f(2).z;
    settings["disc"]["mat"][11] = discMat.getRowAsVec4f(3).z;
    settings["disc"]["mat"][12] = discMat.getRowAsVec4f(0).w;
    settings["disc"]["mat"][13] = discMat.getRowAsVec4f(1).w;
    settings["disc"]["mat"][14] = discMat.getRowAsVec4f(2).w;
    settings["disc"]["mat"][15] = discMat.getRowAsVec4f(3).w;
    
    // shooting
    settings["shooting"]["feedbackRate"] = kinect.feedbackRate;
    settings["shooting"]["isDisplayHeightmap"] = isDisplayHeightmap;
    
    settings["shooting"]["isPreviewLight"] = isPreviewLight;
    settings["shooting"]["lightFront"] = lightFront;
    settings["shooting"]["lightBack"] = lightBack;
    settings["shooting"]["rotateStep"] = rotateStep;
    
    settings["shooting"]["currentFrame"] = currentFrame;
    settings["shooting"]["hudOrigin"][0] = hudOrigin.x;
    settings["shooting"]["hudOrigin"][1] = hudOrigin.y;
    settings["shooting"]["enableTimer"] = timer.isEnabled();
    settings["shooting"]["showOverlay"] = showOverlay;
	settings["shooting"]["showHeightmap"] = showHeightmap;
	settings["shooting"]["forceDisplay"] = forceDisplay;
    
    settings.save("settings.json");
	
}

//--------------------------------------------------------------
void ofApp::sendDmx() {
    dmx.setLevel(1, lightFront * 255);
    dmx.setLevel(2, lightBack * 255);
    dmx.update();
}

void ofApp::enableLight() {
	isPreviewLight = true;
    sendDmx();
}

void ofApp::disableLight() {
    isPreviewLight = false;
    dmx.setLevel(1, 0);
    dmx.setLevel(2, 0);
    dmx.update();
}

void ofApp::doBeforeShoot() {
//    enableLight();
}

void ofApp::doAfterShoot() {
//    disableLight();
    timer.reset();
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
    
    if (mode == MODE_CALIBRATION) {
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
                
                discMat.setTranslation(wOrigin);
                discMat.set(ex.x, ey.x, ez.x, wOrigin.x,
                            ex.y, ey.y, ez.y, wOrigin.y,
                            ex.z, ey.z, ez.z, wOrigin.z,
                            0, 0, 0, 1);
                discInvMat = discMat.getInverse();
                
                useCalibrated = true;
                restCalibrationFrames = -1;
                
                makeDiscMask();
            }
            
        }
        
    } else if (mode == MODE_SHOOTING) {
        
        while (dfReceiver.hasWaitingMessages()) {
            ofxOscMessage m;
            dfReceiver.getNextMessage(m);
            
            string address = m.getAddress();
            int frame = m.getArgAsInt(0);
            
            ofLogNotice() << "path:" << address << " value:" << frame;
            
            if (address == "/dragonframe/shoot" || address == "/dragonframe/position") {
                currentFrame = frame;
                loadHeightmap();
            }
            
            if (address == "/dragonframe/shoot") {
                doBeforeShoot();
            } else if (address == "/dragonframe/cc") {
                doAfterShoot();
            }
        }
        
    }
}

void ofApp::updateScene() {
	
	if (showScene) {
		focus.set(kinect.getFocus());
		
		
		int w = DEPTH_WIDTH;
		int h = DEPTH_HEIGHT;
		int x, y, offset;
		float value, invalid, dist;
		
		for (y = 0; y < h; y++) {
			for (x = 0; x < w; x++) {
				
				dist = kinect.getDistanceAt(x, y);
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
				depthPixels[offset + 2] = 0.0;
			}
		}
		
		depthPixels.mirror(false, true);
		depthImage.setFromPixels(depthPixels);
	}
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
        case F_DISPLAY:
            return "display";
    }
}

void ofApp::makeDiscMask() {
    if (!discMaskPixels.isAllocated()) {
        discMaskPixels.allocate(DEPTH_WIDTH, DEPTH_HEIGHT, 1);
    }
    
    float radius = max(kOrigin.distance(kAxisX), kOrigin.distance(kAxisY)) + discMaskMargin;
    
    ofVec2f p;
    for (p.y = 0; p.y < DEPTH_HEIGHT; p.y++) {
        for (p.x = 0; p.x < DEPTH_WIDTH; p.x++) {
            float dist = kOrigin.distance(p);
            discMaskPixels[p.y * DEPTH_WIDTH + p.x] = (dist <= radius) ? 255 : 0;
        }
    }
    
    discMaskImage.setFromPixels(discMaskPixels);
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(20);
    ofSetColor(255);
    ofPushMatrix();
    
    ofTranslate(GUI_WIDTH, 0);
    
    ofPushMatrix();
    
    this->drawScene();
    
    if (isDisplayIr) {
        // draw ir
        if (irTex.isAllocated()) {
            irShader.begin();
            irTex.draw(0, 0);
            irShader.end();
            
            ofDrawLine(DEPTH_WIDTH / 2, 0, DEPTH_WIDTH / 2, DEPTH_HEIGHT);
            ofDrawLine(0, DEPTH_HEIGHT / 2, DEPTH_WIDTH, DEPTH_HEIGHT / 2);
        }
        
        if (discMaskImage.isAllocated()) {
            ofSetColor(255, 255, 0, 30);
            discMaskImage.draw(0, 0);
            ofSetColor(255);
        }
        ofTranslate(0, DEPTH_HEIGHT);
    }
    
    if (mode == MODE_CALIBRATION) {
        
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
    ofDrawBitmapString("MODE: calibrating disc", 10, 0);
    
    ofTranslate(0, 20);
    ofDrawBitmapString("EDITING: " + getCurrentFeatureName(), 10, 0);
    
    ofTranslate(0, 20);
    ofDrawBitmapString("KINECT: origin-" + toString(kOrigin) + " axisX-" + toString(kAxisX) + " axisY-" + toString(kAxisY) + " display-" + toString(hudOrigin), 10, 0);
    
    if (restCalibrationFrames > 0) {
        
        ofSetColor(255, 0, 0);
        ofTranslate(0, 20);
        ofDrawBitmapString("calibrating..." + ofToString(restCalibrationFrames) + "frames", 10, 0);
        ofSetColor(255);
        
    } else {
        ofTranslate(0, 20);
        ofDrawBitmapString("WORLD: origin-" + toString(wOrigin) + " axisX-" + toString(wAxisX) + " axisY-" + toString(wAxisY), 10, 0);
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
        
        // draw display origin
        ofSetColor(0, 255, 255);
        ofDrawRectangle(hudOrigin, hud.getWidth(), hud.getHeight());
    }
    secondWindow.end();
}

void ofApp::drawShooting() {
    
    ofSetColor(255);
    ofDrawBitmapString("Disc Angle = " + ofToString(heightmapRotation / (PI * 2) * 360, 2), 10, 30);
    
    timer.isEnabled() ? ofSetColor(255) : ofSetColor(0, 0, 255);
    ofDrawBitmapString("Elapsed Time = " + timer.getTimeString(), 10, 50);
    
    ofTranslate(0, 60);
    
    if (heightmapLoader.isLoaded()) {
        ofSetColor(255);
        ofDrawBitmapString("frame=" + ofToString(currentFrame), 10, 20);
        
        if (isDisplayHeightmap) {
            heightmapImage.draw(0, 40, 512, 512);
            contourFinder.draw(0, 40, 512, 512);
            overlayImage.draw(512, 40, 512, 512);
        }
        ofSetColor(255);
    } else {
        ofSetColor(255, 0, 0);
        ofDrawBitmapString("frame=" + ofToString(currentFrame), 10, 20);
    }
    
    ofSetColor(255);
    
    // draw hud
    hud.begin();
    {
        ofClear(0, 0, 0, 0);
        
        ofSetColor(255);
        ofDrawBitmapString("Frame=" + ofToString(currentFrame), 10, 10);
        
        timer.isEnabled() ? ofSetColor(255) : ofSetColor(0, 0, 255);
        ofDrawBitmapString("Elapsed=" + timer.getTimeString(), 10, 30);
        ofDrawBitmapString("Disc Angle=" + ofToString(heightmapRotation, 2), 10, 50);
    }
    hud.end();
    
    secondWindow.begin();
    {
        ofBackground(0);
        ofPushMatrix();
        
        
        if ((irTex.isAllocated() && !isPreviewLight) || forceDisplay) {
            
            ofDrawBitmapString("CURRENT FRAME=" + ofToString(currentFrame), hudOrigin);
            int step = 1;
            int offset = 0;
            float u, v;
            ofVec2f kCoord, pCoord;
            ofVec3f wCoord;
            
            ofVec3f pos;
            float h, th, mix;
            
            ofFloatPixelsRef irPixels = kinect.getIrPixelsRef();
            
            for (kCoord.y = 0; kCoord.y < DEPTH_HEIGHT; kCoord.y += step) {
                for (kCoord.x = 0; kCoord.x < DEPTH_WIDTH; kCoord.x += step) {
                    offset = kCoord.y * DEPTH_WIDTH + kCoord.x;
                    
                    /*
                    if (discMaskPixels[offset] == 0 || irPixels[offset] < discMaskThreshold) {
                        continue;
                    }
                     */
                    
                    wCoord.set( kinect.getWorldCoordinateAt(kCoord.x, kCoord.y) );
                    heightmapMesh.setVertex(offset, wCoord);
                }
            }
            
            ofSetColor(255);
            heightmapShader.begin();
            
            vector<double> coffs = kpt.getCalibration();
            
            heightmapShader.setUniformTexture("heightmap", heightmapImage.getTexture(), 1);
            heightmapShader.setUniformMatrix4f("discInvMat", discInvMat);
            heightmapShader.setUniform2f("projectorSize", 1280, 720);
            heightmapShader.setUniform2f("projectorOffset", -96, 0);
            heightmapShader.setUniform1f("tolerance", tolerance);
            
//            heightmapShader.setUniformi
			heightmapShader.setUniform1i("showOverlay", showOverlay ? 1 : 0);
			heightmapShader.setUniform1i("showHeightmap", showHeightmap ? 1 : 0);
            heightmapShader.setUniformTexture("overlay", overlayImage.getTexture(), 2);
            
//            heightmapShader.setUniformTexture("depth", depthImage, 0);
//            heightmapShader.setUniform2f("focus", focus.x, focus.y);
//            heightmapShader.setUniform1f("near", near);
//            heightmapShader.setUniform1f("far", far);
            
            heightmapShader.setUniform1f("coff0", coffs[0]);
            heightmapShader.setUniform1f("coff1", coffs[1]);
            heightmapShader.setUniform1f("coff2", coffs[2]);
            heightmapShader.setUniform1f("coff3", coffs[3]);
            heightmapShader.setUniform1f("coff4", coffs[4]);
            heightmapShader.setUniform1f("coff5", coffs[5]);
            heightmapShader.setUniform1f("coff6", coffs[6]);
            heightmapShader.setUniform1f("coff7", coffs[7]);
            heightmapShader.setUniform1f("coff8", coffs[8]);
            heightmapShader.setUniform1f("coff9", coffs[9]);
            heightmapShader.setUniform1f("coff10", coffs[10]);
            
            heightmapMesh.draw();
            heightmapShader.end();
            
//            ofDrawLine(projectorWidth / 2, 0, projectorWidth / 2, projectorHeight);
//            ofDrawLine(0, projectorHeight / 2, projectorWidth, projectorHeight / 2);
         
            
            
            ofSetColor(255);
            for (auto blob : contourFinder.blobs) {
                contour.clear();
                
                bool isFirst = true;
                
                for (auto p : blob.pts ) {
                    ofVec3f wc = getWorldFromDisc(ofVec2f(ofMap(p.x, 0, HEIGHTMAP_WIDTH, -30, 30),
                                                  ofMap(p.y, 0, HEIGHTMAP_WIDTH, 30, -30)));
                    ofVec2f pc = getProj(wc);
                    
                    if (isFirst) {contour.moveTo(pc); isFirst = false;}
                    else {contour.lineTo(pc);}
                    
//                    ofDrawRectangle(pc.x, pc.y, 10, 10);
                }
                contour.close();
                contour.draw();
            }
            
            // draw angle
			{
				float rad = heightmapRotation;
				ofVec2f rotOrigin = getProjFromDisc(cos(rad) * 10, sin(rad) * 10);
				ofVec2f rotOrigin2 = getProjFromDisc(cos(rad) * 30, sin(rad) * 30);
				ofSetColor(255);
				ofDrawLine(rotOrigin, rotOrigin2);
			}
			
			{
				float rad = heightmapRotation + PI;
				ofVec2f rotOrigin = getProjFromDisc(cos(rad) * 10, sin(rad) * 10);
				ofVec2f rotOrigin2 = getProjFromDisc(cos(rad) * 30, sin(rad) * 30);
				ofSetColor(255);
				ofDrawLine(rotOrigin, rotOrigin2);
			}
			
			// draw circle
			ofSetColor(255);
			ofNoFill();
			
			ofVec2f prev = getProjFromDisc(ofVec2f(30, 0));
			ofVec2f curt;
			for (int i = 1; i <= 60; i++) {
				float angle = (float)(i / 60.0) * 2 * PI;
				curt.set(getProjFromDisc(cos(angle) * 30, sin(angle) * 30));
				ofDrawLine(prev, curt);
				prev.set(curt);
			}
			ofFill();

			
			
            // draw hud
            ofSetColor(255);
            hud.draw(hudOrigin);
            
            
        }
        
        ofPopMatrix();
    }
    secondWindow.end();
    
    
    
}

void ofApp::drawScene() {
	
	if (showScene) {
    
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
			pointShader.setUniform1f("discMaskThreshold", discMaskThreshold);
			depthPointCloud.draw();
			pointShader.end();
			
			ofSetColor(255);
			ofDrawCircle(wOrigin, 0.3);
			
			ofSetColor(255, 0, 0);
			ofDrawCircle(wAxisX, 0.3);
			ofDrawLine(discMat * ofVec3f(), discMat * ofVec3f(1, 0, 0));
			
			ofSetColor(0, 255, 0);
			ofDrawCircle(wAxisY, 0.3);
			ofDrawLine(discMat * ofVec3f(), discMat * ofVec3f(0, 1, 0));
			
			ofSetColor(0, 0, 255);
			ofDrawLine(discMat * ofVec3f(), discMat * ofVec3f(0, 0, 1));
		}
		
		ofSetColor(255);
		
		camera.end();
		ofDisableDepthTest();
	}
}

//--------------------------------------------------------------
ofVec2f ofApp::getProj(ofVec3f wc) {
    return kpt.getProjectedPoint(wc * ofVec3f(1, 1, -1)) * ofVec2f(projectorWidth, projectorHeight) - ofVec2f(96, 0);


}

ofVec3f ofApp::getDisc(ofVec3f wc) {
    return discInvMat * wc;
}

ofVec3f ofApp::getWorldFromDisc(ofVec2f dc) {
    return discMat * ofVec3f(dc.x, dc.y, 0);
}

ofVec2f ofApp::getProjFromDisc(ofVec2f dc) {
    return getProj(getWorldFromDisc(dc));
}

void ofApp::loadHeightmap() {
    heightmapLoader.load(heightmapPixels, currentFrame);
    
    if (heightmapLoader.isLoaded()) {
        heightmapImage.setFromPixels(heightmapPixels);
        
        heightmapRotation = heightmapPixels[3 * HEIGHTMAP_WIDTH + 3] * PI * 2;
        
        int x, y;
        
        for (int i = 0, len = HEIGHTMAP_WIDTH * HEIGHTMAP_WIDTH; i < len; i++) {
            heightmapU8Pixels[i] = (unsigned char)(heightmapPixels[i] * 255 * 80);
        }
        
        heightmapCvImage.setFromPixels(heightmapU8Pixels);
        heightmapCvImage.threshold(0);
        contourFinder.findContours(heightmapCvImage, 2, HEIGHTMAP_WIDTH * HEIGHTMAP_WIDTH, 20, true);
    }
    
    overlayLoader.load(overlayPixels, currentFrame);
    if (overlayLoader.isLoaded()) {
        overlayImage.setFromPixels(overlayPixels);
    }
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
    } else if (currentFeature == F_DISPLAY) {
        hudOrigin += ofVec2f(x, y) * amp * 10;
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    if (mode == MODE_CALIBRATION) {
        
        switch (key) {
            case OF_KEY_LEFT:   moveFeature(-1,  0); break;
            case OF_KEY_UP:     moveFeature( 0, -1); break;
            case OF_KEY_RIGHT:  moveFeature(+1,  0); break;
            case OF_KEY_DOWN:   moveFeature( 0, +1); break;
                
            case '1': currentFeature = F_ORIGIN; break;
            case '2': currentFeature = F_AXIS_X; break;
            case '3': currentFeature = F_AXIS_Y; break;
            case '4': currentFeature = F_DISPLAY; break;
                
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
                
            case '5':  lightBack = ofClamp(lightBack + LIGHT_STEP, 0, 1); sendDmx(); break;
            case '2':  lightBack = ofClamp(lightBack - LIGHT_STEP, 0, 1); sendDmx(); break;
            case '4':  lightFront = ofClamp(lightFront + LIGHT_STEP, 0, 1); sendDmx(); break;
            case '1':  lightFront = ofClamp(lightFront - LIGHT_STEP, 0, 1); sendDmx(); break;
                
            case 'd':
                isDisplayHeightmap = !isDisplayHeightmap;
                break;
                
            case 'o':
                showOverlay = !showOverlay;
                break;
				
			case 'h':
				showHeightmap = !showHeightmap;
				break;
                
            case 'n':
                currentFrame += 1;
                loadHeightmap();
                break;
            case 'p':
                currentFrame -= 1;
                loadHeightmap();
                break;
				
			case 'm':
				forceDisplay = !forceDisplay;
				break;
        }
    }
    
    switch (key) {
        case OF_KEY_LEFT_ALT: altPressed = true; break;
        case OF_KEY_LEFT_SHIFT: shiftPressed = true; break;
        
        case 't':
            if (mode == MODE_SHOOTING) mode = MODE_CALIBRATION;
            else if (mode == MODE_CALIBRATION) mode = MODE_SHOOTING;
            break;
            
        case 'i':
            isDisplayIr = !isDisplayIr;
            break;
            
        case 'l':
            if (isPreviewLight) {
                disableLight();
            } else {
                enableLight();
            }
            break;
            
        case 'f':
            ofToggleFullscreen();
            break;
    }
    
}

void ofApp::keyReleased(int key) {
    switch (key) {
        case OF_KEY_LEFT_ALT: altPressed = false; break;
        case OF_KEY_LEFT_SHIFT: shiftPressed = false; break;
    }
}