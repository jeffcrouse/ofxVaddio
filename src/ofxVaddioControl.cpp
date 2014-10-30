//
//  ofxVaddioControl.cpp
//  SerialTest
//
//  Created by Jeffrey Crouse on 10/18/14.
//  Copyright (c) 2014 __MyCompanyName__. All rights reserved.
//

#include "ofxVaddioControl.h"


// --------------------------------------------
void ofxVaddioControl::setup(bool keyEvents){
    serial.setup(0, 9600);
    startThread();

    if(keyEvents) {
        enableKeyEvents();
    }
    for(int i=0; i<255; i++) keyDown[i]=false;
}

// --------------------------------------------
void ofxVaddioControl::write(packet p) {
    stringstream out;
    
    unsigned char buf[p.size()];
    for(int i=0; i<p.size(); i++) {
        out << ofToHex((char)p[i]) << " ";
        buf[i] = p[i];
        //serial.writeByte(p[i]);
    }
    serial.writeBytes(buf, p.size());
    serial.drain();
    
    ofLogVerbose("ofxVaddioControl >") << out.str();
}

// --------------------------------------------
packet ofxVaddioControl::read() {
    
    packet response;
    stringstream in;
    float timeout = ofGetElapsedTimef() + 5;
    int ch;
    do {
        ch = serial.readByte();
        if(ch != OF_SERIAL_NO_DATA) {
            response.push_back(ch);
            in << ofToHex((char)ch) << " ";
        }
    } while(ch != 0xFF && ofGetElapsedTimef()<timeout);
    
    ofLogVerbose("ofxVaddioControl <") << in.str();
    
    return response;
}

// --------------------------------------------
void ofxVaddioControl::threadedFunction()
{
    while(isThreadRunning())
    {
        if(queue.size()>0)
        {
            if(lock())
            {
                packet cmd = queue[0];
                queue.erase(queue.begin());
                unlock();
                
                write(cmd);
                read();
            }
            else
            {
                ofLogWarning("threadedFunction()") << "Unable to lock mutex.";
            }
        }
        else
        {
            sleep(100);
        }
    }
}

// --------------------------------------------
void ofxVaddioControl::addToQueue(packet cmd) {
    if(lock()) {
        queue.push_back(cmd);
        unlock();
    }
}

// --------------------------------------------
void ofxVaddioControl::close(){
    serial.close();
    stopThread();
}

// --------------------------------------------------------
void ofxVaddioControl::home() {
    ofLogNotice("ofxVaddioControl") << "home";

    vector<int> home;
    home.push_back(0x81);
    home.push_back(0x01);
    home.push_back(0x06);
    home.push_back(0x04);
    home.push_back(0xFF);
    
    addToQueue(home);
}

// --------------------------------------------------------
// 81 01 7E 01 0B WW VV ZZ FF
void ofxVaddioControl::presetSpeed(float pan, float tilt, float zoom) {
    ofLogNotice("ofxVaddioControl") << "presetSpeed";
    
    vector<int> preset_speed;
    preset_speed.push_back(0x81);
    preset_speed.push_back(0x01);
    preset_speed.push_back(0x7E);
    preset_speed.push_back(0x01);
    preset_speed.push_back(0x0B);
    preset_speed.push_back(ofMap(pan, 0, 1, VADDIO_PAN_SPEED_MIN, VADDIO_PAN_SPEED_MAX, true));
    preset_speed.push_back(ofMap(tilt, 0, 1, VADDIO_TILT_SPEED_MIN, VADDIO_TILT_SPEED_MAX, true));
    preset_speed.push_back(ofMap(zoom, 0, 1, VADDIO_ZOOM_SPEED_MIN, VADDIO_ZOOM_SPEED_MAX, true));
    preset_speed.push_back(0xFF);

    addToQueue(preset_speed);
}


// --------------------------------------------------------
// 81 01 7E 01 70 00 00 FF
void ofxVaddioControl::hardMotorStops() {
    ofLogNotice("ofxVaddioControl") << "hardMotorStops";
    
    vector<int> hard_motor_stops;
    hard_motor_stops.push_back(0x81);
    hard_motor_stops.push_back(0x01);
    hard_motor_stops.push_back(0x7E);
    hard_motor_stops.push_back(0x01);
    hard_motor_stops.push_back(0x70);
    hard_motor_stops.push_back(0x00);
    hard_motor_stops.push_back(0x00);
    hard_motor_stops.push_back(0xFF);

    addToQueue(hard_motor_stops);

}

// --------------------------------------------------------
// 81 01 7E 01 70 00 01 FF
void ofxVaddioControl::softMotorStops() {
    ofLogNotice("ofxVaddioControl") << "softMotorStops";
    
    vector<int> soft_motor_stops;
    soft_motor_stops.push_back(0x81);
    soft_motor_stops.push_back(0x01);
    soft_motor_stops.push_back(0x7E);
    soft_motor_stops.push_back(0x01);
    soft_motor_stops.push_back(0x70);
    soft_motor_stops.push_back(0x00);
    soft_motor_stops.push_back(0x01);
    soft_motor_stops.push_back(0xFF);
    
    addToQueue(soft_motor_stops);
}


// --------------------------------------------------------
void ofxVaddioControl::pantiltLeft(float panSpeed, float tiltSpeed) {
    ofLogNotice("ofxVaddioControl") << "pantiltLeft";
    packet pantilt_left;
    pantilt_left.push_back(0x81);
    pantilt_left.push_back(0x01);
    pantilt_left.push_back(0x06);
    pantilt_left.push_back(0x01);
    pantilt_left.push_back(ofMap(panSpeed, 0, 1, VADDIO_PAN_SPEED_MIN, VADDIO_PAN_SPEED_MAX, true));
    pantilt_left.push_back(ofMap(tiltSpeed, 0, 1, VADDIO_TILT_SPEED_MIN, VADDIO_TILT_SPEED_MAX, true));
    pantilt_left.push_back(0x01);
    pantilt_left.push_back(0x03);
    pantilt_left.push_back(0xFF);
    

    addToQueue(pantilt_left);
}

// --------------------------------------------------------
void ofxVaddioControl::pantiltRight(float panSpeed, float tiltSpeed) {
    ofLogNotice("ofxVaddioControl") << "pantiltRight";
    
    packet pantilt_right;
    pantilt_right.push_back(0x81);
    pantilt_right.push_back(0x01);
    pantilt_right.push_back(0x06);
    pantilt_right.push_back(0x01);
    pantilt_right.push_back(ofMap(panSpeed, 0, 1, VADDIO_PAN_SPEED_MIN, VADDIO_PAN_SPEED_MAX, true));
    pantilt_right.push_back(ofMap(tiltSpeed, 0, 1, VADDIO_TILT_SPEED_MIN, VADDIO_TILT_SPEED_MAX, true));
    pantilt_right.push_back(0x02);
    pantilt_right.push_back(0x03);
    pantilt_right.push_back(0xFF);

    addToQueue(pantilt_right);
}
// --------------------------------------------------------
void ofxVaddioControl::pantiltUp(float panSpeed, float tiltSpeed) {
    ofLogNotice("ofxVaddioControl") << "pantiltUp";
    
    packet pantilt_up;
    pantilt_up.push_back(0x81);
    pantilt_up.push_back(0x01);
    pantilt_up.push_back(0x06);
    pantilt_up.push_back(0x01);
    pantilt_up.push_back(ofMap(panSpeed, 0, 1, VADDIO_PAN_SPEED_MIN, VADDIO_PAN_SPEED_MAX, true));
    pantilt_up.push_back(ofMap(tiltSpeed, 0, 1, VADDIO_TILT_SPEED_MIN, VADDIO_TILT_SPEED_MAX, true));
    pantilt_up.push_back(0x03);
    pantilt_up.push_back(0x01);
    pantilt_up.push_back(0xFF);

    addToQueue(pantilt_up);
}
// --------------------------------------------------------
void ofxVaddioControl::pantiltDown(float panSpeed, float tiltSpeed) {
    ofLogNotice("ofxVaddioControl") << "pantiltDown";
    
    packet pantilt_down;
    pantilt_down.push_back(0x81);
    pantilt_down.push_back(0x01);
    pantilt_down.push_back(0x06);
    pantilt_down.push_back(0x01);
    pantilt_down.push_back(ofMap(panSpeed, 0, 1, VADDIO_PAN_SPEED_MIN, VADDIO_PAN_SPEED_MAX, true));
    pantilt_down.push_back(ofMap(tiltSpeed, 0, 1, VADDIO_TILT_SPEED_MIN, VADDIO_TILT_SPEED_MAX, true));
    pantilt_down.push_back(0x03);
    pantilt_down.push_back(0x02);
    pantilt_down.push_back(0xFF);
    
    addToQueue(pantilt_down);
}
// --------------------------------------------------------
void ofxVaddioControl::pantiltStop() {
    ofLogNotice("ofxVaddioControl") << "pantiltStop";
    
    packet pantilt_stop;
    pantilt_stop.push_back(0x81);
    pantilt_stop.push_back(0x01);
    pantilt_stop.push_back(0x06);
    pantilt_stop.push_back(0x01);
    pantilt_stop.push_back(0x00);
    pantilt_stop.push_back(0x00);
    pantilt_stop.push_back(0x03);
    pantilt_stop.push_back(0x03);
    pantilt_stop.push_back(0xFF);

    addToQueue(pantilt_stop);
}

// --------------------------------------------------------
ofxVaddioPantilt ofxVaddioControl::pantiltInq() {
    ofLogNotice("ofxVaddioControl") << "pantiltInq";
    ofxVaddioPantilt pt;
    pt.error = true;

    packet pantilt_inq;
    pantilt_inq.push_back(0x81);
    pantilt_inq.push_back(0x09);
    pantilt_inq.push_back(0x06);
    pantilt_inq.push_back(0x12);
    pantilt_inq.push_back(0xFF);
    
    write(pantilt_inq);
    packet response = read();
   
    if(response.size()!=11)  {
        pt.error= true;
        ofLogWarning() << "Bad response from pantiltInq";
        return pt;
    } else {
        
        int _pan =   ((response[2] & 0xf) << 12) +
            ((response[3] & 0xf) <<  8) +
            ((response[4] & 0xf) <<  4) +
            (response[5] & 0xf);
        
        int _tilt =  ((response[6] & 0xf) << 12) +
            ((response[7] & 0xf) <<  8) +
            ((response[8] & 0xf) <<  4) +
            (response[9] & 0xf);
        
        if (_pan > 0x8000) _pan -= 0x10000;
        if (_tilt > 0x8000) _tilt -= 0x10000;
        
        pt.error = false;
        
        pt.pan = ofMap(_pan,
                          VADDIO_PAN_MIN,
                          VADDIO_PAN_MAX,
                          VADDIO_PAN_MIN_DEGREES,
                          VADDIO_PAN_MAX_DEGREES );
        pt.tilt = ofMap(_tilt,
                           VADDIO_TILT_MIN,
                           VADDIO_TILT_MAX,
                           VADDIO_TILT_MIN_DEGREES,
                           VADDIO_TILT_MAX_DEGREES );
        
        return pt;
    }
}

// --------------------------------------------------------
void ofxVaddioControl::pantiltAbsolute(float pan, float tilt, float panSpeed, float tiltSpeed) {
    ofLogNotice("ofxVaddioControl") << "pantiltAbsolute " << pan << " " << tilt << " " << panSpeed << " " << tiltSpeed;
    
    int _pan = ofMap(pan,
                     VADDIO_PAN_MIN_DEGREES,
                     VADDIO_PAN_MAX_DEGREES,
                     VADDIO_PAN_MIN,
                     VADDIO_PAN_MAX);
    int _tilt = ofMap(tilt,
                      VADDIO_TILT_MIN_DEGREES,
                      VADDIO_TILT_MAX_DEGREES,
                      VADDIO_TILT_MIN,
                      VADDIO_TILT_MAX);
    
    packet pantilt_absolute;
    pantilt_absolute.push_back(0x81);
    pantilt_absolute.push_back(0x01);
    pantilt_absolute.push_back(0x06);
    pantilt_absolute.push_back(0x02);
    pantilt_absolute.push_back(ofMap(panSpeed, 0.0, 1.0, VADDIO_PAN_SPEED_MIN, VADDIO_PAN_SPEED_MAX, true));
    pantilt_absolute.push_back(ofMap(tiltSpeed, 0.0, 1.0, VADDIO_TILT_SPEED_MIN, VADDIO_TILT_SPEED_MAX, true));
    
    // Lifted from  VISCA_set_pantilt_absolute_position
    pantilt_absolute.push_back((_pan & 0x0f000) >> 12);
    pantilt_absolute.push_back((_pan & 0x00f00) >>  8);
    pantilt_absolute.push_back((_pan & 0x000f0) >>  4);
    pantilt_absolute.push_back((_pan & 0x0000f)      );
    
    pantilt_absolute.push_back((_tilt & 0xf000) >> 12);
    pantilt_absolute.push_back((_tilt & 0x0f00) >>  8);
    pantilt_absolute.push_back((_tilt & 0x00f0) >>  4);
    pantilt_absolute.push_back((_tilt & 0x000f)      );
    
    pantilt_absolute.push_back(0xFF);
    
    addToQueue(pantilt_absolute);
}

// --------------------------------------------------------
void ofxVaddioControl::zoomIn(float speed) {
    ofLogNotice("ofxVaddioControl") << "zoomIn";
    
    int _speed = ofMap(speed, 0, 1, VADDIO_ZOOM_SPEED_MIN, VADDIO_ZOOM_SPEED_MAX, true);
    
    packet zoom_in;
    zoom_in.push_back(0x81);
    zoom_in.push_back(0x01);
    zoom_in.push_back(0x04);
    zoom_in.push_back(0x07);
    zoom_in.push_back(0x20 | (_speed & 0x7));
    zoom_in.push_back(0xFF);
    
    addToQueue(zoom_in);
}
// --------------------------------------------------------
void ofxVaddioControl::zoomOut(float speed) {
    ofLogNotice("ofxVaddioControl") << "zoomOut";
    
    int _speed = ofMap(speed, 0, 1, VADDIO_ZOOM_SPEED_MIN, VADDIO_ZOOM_SPEED_MAX, true);
    
    packet zoom_out;
    zoom_out.push_back(0x81);
    zoom_out.push_back(0x01);
    zoom_out.push_back(0x04);
    zoom_out.push_back(0x07);
    zoom_out.push_back(0x30 | (_speed & 0x7));
    zoom_out.push_back(0xFF);
    
    addToQueue(zoom_out);
}

// --------------------------------------------------------
void ofxVaddioControl::zoomStop() {
    ofLogNotice("ofxVaddioControl") << "zoomStop";
    
    packet zoom_stop;
    zoom_stop.push_back(0x81);
    zoom_stop.push_back(0x01);
    zoom_stop.push_back(0x04);
    zoom_stop.push_back(0x07);
    zoom_stop.push_back(0x00);
    zoom_stop.push_back(0xFF);
    
    addToQueue(zoom_stop);
}

// --------------------------------------------------------
// 81 01 04 47 0p 0q 0r 0s FF
void ofxVaddioControl::zoomDirect(float zoom) {
    ofLogNotice("ofxVaddioControl") << "zoomDirect " << zoom;
    
    int _zoom = ofMap(zoom,
                      VADDIO_ZOOM_MIN_X,
                      VADDIO_ZOOM_MAX_X,
                      VADDIO_ZOOM_MIN,
                      VADDIO_ZOOM_MAX);
    packet zoom_direct;
    zoom_direct.push_back(0x81);
    zoom_direct.push_back(0x01);
    zoom_direct.push_back(0x04);
    zoom_direct.push_back(0x47);
    zoom_direct.push_back((_zoom & 0xF000) >> 12);
    zoom_direct.push_back((_zoom & 0x0F00) >>  8);
    zoom_direct.push_back((_zoom & 0x00F0) >>  4);
    zoom_direct.push_back((_zoom & 0x000F));
    zoom_direct.push_back(0xFF);
    
    addToQueue(zoom_direct);
}

// --------------------------------------------------------
// 81 01 7E 01 4A 0V 0p 0q 0r 0s FF
void ofxVaddioControl::zoomDirect(float zoom, float speed) {
    ofLogNotice("ofxVaddioControl") << "zoomDirect";
    
    int _zoom = ofMap(zoom,
                     VADDIO_ZOOM_MIN_X,
                     VADDIO_ZOOM_MAX_X,
                     VADDIO_ZOOM_MIN,
                     VADDIO_ZOOM_MAX);
    
    int _speed = ofMap(speed, 0, 1, VADDIO_ZOOM_SPEED_MIN, VADDIO_ZOOM_SPEED_MAX, true);
    
    packet zoom_direct;
    zoom_direct.push_back(0x81);
    zoom_direct.push_back(0x01);
    zoom_direct.push_back(0x7E);
    zoom_direct.push_back(0x01);
    zoom_direct.push_back(0x4A);
    zoom_direct.push_back(0x00 | (_speed & 0x7));
    zoom_direct.push_back((_zoom & 0xF000) >> 12);
    zoom_direct.push_back((_zoom & 0x0F00) >>  8);
    zoom_direct.push_back((_zoom & 0x00F0) >>  4);
    zoom_direct.push_back((_zoom & 0x000F));
    zoom_direct.push_back(0xFF);
    
    addToQueue(zoom_direct);
}

// --------------------------------------------------------
float ofxVaddioControl::zoomInq() {
    ofLogNotice("ofxVaddioControl") << "zoomInq";

    packet zoom_inq;
    zoom_inq.push_back(0x81);
    zoom_inq.push_back(0x09);
    zoom_inq.push_back(0x04);
    zoom_inq.push_back(0x47);
    zoom_inq.push_back(0xFF);
    
    write(zoom_inq);
    packet response = read();

    
    if(response.size()!=7) {
        ofLogWarning("ofxVaddioControl") << "bad response from zoomInq";
        return -1;
    } else {
        
        int _zoom =  (response[2] << 12) +
            (response[3] <<  8) +
            (response[4] <<  4) +
            (response[5]);
        float zoom = ofMap(_zoom,
                           VADDIO_ZOOM_MIN,
                           VADDIO_ZOOM_MAX,
                           VADDIO_ZOOM_MIN_X,
                           VADDIO_ZOOM_MAX_X);
        ofLogNotice("ofxVaddioControl") << " zoom = " << zoom;
        return zoom;
    }
}


// --------------------------------------------------------
// 81 01 04 38 02 FF
void ofxVaddioControl::focusAuto(){
    ofLogNotice("ofxVaddioControl") << "focusAuto";
    
    packet focus_auto;
    focus_auto.push_back(0x81);
    focus_auto.push_back(0x01);
    focus_auto.push_back(0x04);
    focus_auto.push_back(0x38);
    focus_auto.push_back(0x02);
    focus_auto.push_back(0xFF);
    
    addToQueue(focus_auto);
}

// --------------------------------------------------------
// 81 01 04 38 03 FF
void ofxVaddioControl::focusManual(){
    ofLogNotice("ofxVaddioControl") << "focusManual";
    
    packet focus_manual;
    focus_manual.push_back(0x81);
    focus_manual.push_back(0x01);
    focus_manual.push_back(0x04);
    focus_manual.push_back(0x38);
    focus_manual.push_back(0x03);
    focus_manual.push_back(0xFF);
    
    addToQueue(focus_manual);
}

// --------------------------------------------------------
// 81 01 04 48 0p 0q 0r 0s FF
void ofxVaddioControl::focusDirect(float focus){
    ofLogNotice("ofxVaddioControl") << "focusDirect " << focus;
    
    int _focus = ofMap(focus, 0, 1, VADDIO_FOCUS_MIN, VADDIO_FOCUS_MAX);
    
    packet focus_direct;
    focus_direct.push_back(0x81);
    focus_direct.push_back(0x01);
    focus_direct.push_back(0x04);
    focus_direct.push_back(0x48);
    focus_direct.push_back((_focus & 0xF000) >> 12);
    focus_direct.push_back((_focus & 0x0F00) >>  8);
    focus_direct.push_back((_focus & 0x00F0) >>  4);
    focus_direct.push_back((_focus & 0x000F));
    focus_direct.push_back(0xFF);
    
    addToQueue(focus_direct);
}

// --------------------------------------------------------
// 81 01 04 08 02 FF
void ofxVaddioControl::focusFar(){
    ofLogNotice("ofxVaddioControl") << "focusFar";
    
    packet focus_far;
    focus_far.push_back(0x81);
    focus_far.push_back(0x01);
    focus_far.push_back(0x04);
    focus_far.push_back(0x08);
    focus_far.push_back(0x02);
    focus_far.push_back(0xFF);
    
    addToQueue(focus_far);
}

// --------------------------------------------------------
// 81 01 04 08 03 FF
void ofxVaddioControl::focusNear(){
    ofLogNotice("ofxVaddioControl") << "focusNear";
    
    vector<int> focus_near;
    focus_near.push_back(0x81);
    focus_near.push_back(0x01);
    focus_near.push_back(0x04);
    focus_near.push_back(0x08);
    focus_near.push_back(0x03);
    focus_near.push_back(0xFF);
    
    addToQueue(focus_near);
}

// --------------------------------------------------------
// 81 01 04 08 00 FF
void ofxVaddioControl::focusStop(){
    ofLogNotice("ofxVaddioControl") << "focusStop";
    
    packet focus_stop;
    focus_stop.push_back(0x81);
    focus_stop.push_back(0x01);
    focus_stop.push_back(0x04);
    focus_stop.push_back(0x08);
    focus_stop.push_back(0x00);
    focus_stop.push_back(0xFF);
    
    addToQueue(focus_stop);
}

// --------------------------------------------------------
// 81 09 04 48 FF
float ofxVaddioControl::focusInq(){
    ofLogNotice("ofxVaddioControl") << "focusInq";

    vector<int> focus_inq;
    focus_inq.push_back(0x81);
    focus_inq.push_back(0x09);
    focus_inq.push_back(0x04);
    focus_inq.push_back(0x48);
    focus_inq.push_back(0xFF);

    write(focus_inq);
    packet response = read();

    if(response.size()!=7) {
        ofLogWarning("ofxVaddioControl") << "bad response from focusInq";
        return -1;
    } else {
        int focus =  (response[2] << 12) +
                    (response[3] <<  8) +
                    (response[4] <<  4) +
                    (response[5]);
        return ofMap(focus, VADDIO_FOCUS_MIN, VADDIO_FOCUS_MAX, 0, 1);
    }
}



// --------------------------------------------------------
void ofxVaddioControl::keyPressed(ofKeyEventArgs& args) {
    if(keyDown[args.key]) return;
    
    if(args.key==OF_KEY_ESC) {
        home();
    }
    
    if(args.key=='f' && ofGetModifierShortcutKeyPressed()) {
        serial.flush();
    }
    
    //
    // PANTILT
    //
    if(args.key==OF_KEY_LEFT && !ofGetModifierShiftPressed()) {
        pantiltLeft(panSpeed, tiltSpeed);
        ptKeyboardEventInProgress=true;
    }
    if(args.key==OF_KEY_RIGHT && !ofGetModifierShiftPressed()) {
        pantiltRight(panSpeed, tiltSpeed);
        ptKeyboardEventInProgress=true;
    }
    if(args.key==OF_KEY_UP && !ofGetModifierShiftPressed()) {
        pantiltUp(panSpeed, tiltSpeed);
        ptKeyboardEventInProgress=true;
    }
    if(args.key==OF_KEY_DOWN && !ofGetModifierShiftPressed()) {
        pantiltDown(panSpeed, tiltSpeed);
        ptKeyboardEventInProgress=true;
    }
    
    //
    // ZOOM
    //
    if(args.key==OF_KEY_DOWN && ofGetModifierShiftPressed()) {
        zoomOut(zoomSpeed);
        zoomKeyboardEventInProgress=true;
    }
    if(args.key==OF_KEY_UP && ofGetModifierShiftPressed()) {
        zoomIn(zoomSpeed);
        zoomKeyboardEventInProgress=true;
    }

    
    //
    // FOCUS
    //
    if(args.key==']') {
        focusFar();
        focusKeyboardEventInProgress=true;
    }
    if(args.key=='[') {
        focusNear();
        focusKeyboardEventInProgress=true;
    }
    keyDown[args.key]=true;
}

// ----------------------------------------------------------
void ofxVaddioControl::keyReleased(ofKeyEventArgs& args) {
    
    if((args.key==OF_KEY_LEFT ||
        args.key==OF_KEY_RIGHT ||
        args.key==OF_KEY_UP ||
        args.key==OF_KEY_DOWN) &&
       ptKeyboardEventInProgress) {
        pantiltStop();
        ptKeyboardEventInProgress=false;
    }
    if((args.key==OF_KEY_DOWN || args.key==OF_KEY_UP)
       && zoomKeyboardEventInProgress) {
        zoomStop();
        zoomKeyboardEventInProgress=false;
    }
    if((args.key=='[' || args.key==']')
       && focusKeyboardEventInProgress) {
        focusStop();
        focusKeyboardEventInProgress=false;
    }
    keyDown[args.key]=false;
}