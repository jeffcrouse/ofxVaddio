//
//  ofxVaddioControl.h
//  SerialTest
//
//  Created by Jeffrey Crouse on 10/18/14.
//  Copyright (c) 2014 __MyCompanyName__. All rights reserved.
//

#pragma once
#include "ofMain.h"

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
    }
    void disableKeyEvents(){
        ofUnregisterKeyEvents(this);
    }
    void keyReleased(ofKeyEventArgs& args);
    void keyPressed(ofKeyEventArgs& args);
    
    // Pantilt Control
    void home();
    void pantiltLeft(float panSpeed=0.5, float tiltSpeed=0.5);
    void pantiltRight(float panSpeed=0.5, float tiltSpeed=0.5);
    void pantiltUp(float panSpeed=0.5, float tiltSpeed=0.5);
    void pantiltDown(float panSpeed=0.5, float tiltSpeed=0.5);
    void pantiltStop();
    void pantiltAbsolute(float pan, float tilt, float panSpeed=0.5, float tiltSpeed=0.5);
    ofxVaddioPantilt pantiltInq();
    
    // Zoom Control
    void zoomIn(float speed=0.5);
    void zoomOut(float speed=0.5);
    void zoomStop();
    void zoomDirect(float zoom);
    void zoomDirect(float zoom, float speed);
    float zoomInq();
    

protected:
    
    void write(vector<int> packet);
    vector<int> read();
    bool keyDown[255];
    ofSerial serial;
};

