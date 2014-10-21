//
//  ofxVaddioControl.h
//  SerialTest
//
//  Created by Jeffrey Crouse on 10/18/14.
//  Copyright (c) 2014 __MyCompanyName__. All rights reserved.
//

#pragma once
#include "ofMain.h"
#include "ofxHotKeys.h"

#define VADDIO_TILT_MIN -5999
#define VADDIO_TILT_MAX 19499

#define VADDIO_TILT_MIN_DEGREES -30.0f
#define VADDIO_TILT_MAX_DEGREES 90.0f

#define VADDIO_PAN_MIN -32700
#define VADDIO_PAN_MAX 32700

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


#define VADDIO_FOCUS_MIN 0x000
#define VADDIO_FOCUS_MAX 0x0C000

#define DEFAULT_SPEED 0.5

struct ofxVaddioPantilt {
    float pan;
    float tilt;
};

class ofxVaddioControl
{
public:
    void setup(bool keyEvents=true);
    void close();

    // Keyboard Events
    void enableKeyEvents() {
        ofRegisterKeyEvents(this);
        ptKeyboardEventInProgress=false;
        zoomKeyboardEventInProgress=false;
        focusKeyboardEventInProgress=false;
    }
    void disableKeyEvents(){
        ofUnregisterKeyEvents(this);
    }
    void keyReleased(ofKeyEventArgs& args);
    void keyPressed(ofKeyEventArgs& args);
    bool ptKeyboardEventInProgress;
    bool zoomKeyboardEventInProgress;
    bool focusKeyboardEventInProgress;
    
    // Pantilt Control
    void pantiltLeft(float panSpeed=DEFAULT_SPEED, float tiltSpeed=DEFAULT_SPEED);
    void pantiltRight(float panSpeed=DEFAULT_SPEED, float tiltSpeed=DEFAULT_SPEED);
    void pantiltUp(float panSpeed=DEFAULT_SPEED, float tiltSpeed=DEFAULT_SPEED);
    void pantiltDown(float panSpeed=DEFAULT_SPEED, float tiltSpeed=DEFAULT_SPEED);
    void pantiltStop();
    void pantiltAbsolute(float pan, float tilt, float panSpeed=DEFAULT_SPEED, float tiltSpeed=DEFAULT_SPEED);
    ofxVaddioPantilt pantiltInq();
    
    
    // Zoom Control
    void zoomIn(float speed=DEFAULT_SPEED);
    void zoomOut(float speed=DEFAULT_SPEED);
    void zoomStop();
    void zoomDirect(float zoom);
    void zoomDirect(float zoom, float speed);
    float zoomInq();
    
    // Focus
    void focusAuto();
    void focusManual();
    void focusDirect(float focus);
    void focusFar();
    void focusNear();
    void focusStop();
    float focusInq();
    
    // Other
    void home();
    void presetSpeed(float pan, float tilt, float zoom);
    void hardMotorStops();
    void softMotorStops();
    
protected:
    
    void write(vector<int> packet);
    vector<int> read();
    bool keyDown[255];
    ofSerial serial;
};

