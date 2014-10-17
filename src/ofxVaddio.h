//
//  ofxVaddio.h
//  Camera02
//
//  Created by Jeffrey Crouse on 10/9/14.
//  Copyright (c) 2014 __MyCompanyName__. All rights reserved.
//

#pragma once
#include "ofMain.h"

#define VADDIO_HEADER 0x81
#define VADDIO_TERMINATOR 0xFF
#define VADDIO_COMMAND_PREFIX 0x01
#define VADDIO_INQUIRY_PREFIX 0x09

#define VADDIO_TILT_MIN -5999
#define VADDIO_TILT_MAX 19499

#define VADDIO_TILT_MIN_DEGREES -30.0f
#define VADDIO_TILT_MAX_DEGREES 90.0f

#define VADDIO_PAN_MIN -3270
#define VADDIO_PAN_MAX 3270

#define VADDIO_PAN_MIN_DEGREES -170.0f
#define VADDIO_PAN_MAX_DEGREES 170.0f

#define VADDIO_ZOOM_MIN 0
#define VADDIO_ZOOM_MAX 25600

#define VADDIO_ZOOM_MIN_X 0.0f
#define VADDIO_ZOOM_MAX_X 20.0f

#define VADDIO_PAN_SPEED_MIN 0x01
#define VADDIO_PAN_SPEED_MAX 0x18

#define VADDIO_TILT_SPEED_MIN 0x01
#define VADDIO_TILT_SPEED_MAX 0x14

#define VADDIO_ZOOM_SPEED_MIN 0
#define VADDIO_ZOOM_SPEED_MAX 7


struct ofxVaddioPantiltPosition {
    float pan;
    float tilt;
    bool error;
};

class ofxVaddio
{
public:
    void setup();
    void close();
    void update();
    
    void enableKeyEvents();
    void disableKeyEvents();
    
    void setPanSpeed(float s) {
        pan_speed = ofMap(s, 0, 1, 0x01, 0x18);
    }
    void setTiltSpeed(float s) {
        tilt_speed = ofMap(s, 0, 1, 0x01, 0x14);
    }
    void setZoomSpeed(float s) {
        zoom_speed = ofMap(s, 0, 1, 0, 7);
    }
    void setSpeed(float p, float t, float z) {
        setPanSpeed(p);
        setTiltSpeed(t);
        setZoomSpeed(z);
    }
    
    // Inquiry
    ofxVaddioPantiltPosition getPosition();
    float getZoom();
    
    
    // Commands
    void panLeft();
    void panRight();
    void tiltUp();
    void tiltDown();
    void zoomOut();
    void zoomIn();
    void zoomDirect(int zoom);
    void stopPanTilt();
    void stopZoom();
    void home();
    void absolutePosition(ofxVaddioPantiltPosition pos);
    void absolutePosition(int pan, int tilt);
    void memoryReset(int num);
    void memorySet(int num);
    void memoryRecall(int num);
    void presetSpeed(int pan, int tilt, int zoom);
    
    // Utility
    void setPTZ(float pan, float tilt, float zoom) {
        absolutePosition(pan, tilt);
        zoomDirect(zoom);
    }
    
    // Keyboard Events
    void keyReleased(ofKeyEventArgs& args);
    void keyPressed(ofKeyEventArgs& args);
    
protected:
    
    bool keyIsDown[255];
    
    ofSerial serial;

    void writePacket(vector<int>& bytes);
    char pan_speed;
    char tilt_speed;
    int zoom_speed;
    
    bool bQueryInProgress;
    ofxVaddioPantiltPosition queryPos;
    float queryZoom;
    float nextQuery;
    
};

