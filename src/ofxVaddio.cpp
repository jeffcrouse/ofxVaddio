//
//  ofxVaddio.cpp
//  Camera02
//
//  Created by Jeffrey Crouse on 10/9/14.
//  Copyright (c) 2014 __MyCompanyName__. All rights reserved.
//

#include "ofxVaddio.h"

void ofxVaddio::setup() {
    
    serial.listDevices();

    serial.setup(0, 9600); //open the first device
    
    setPanSpeed(0.5);
    setTiltSpeed(0.5);
    setZoomSpeed(0.5);
    
    home();
    presetSpeed(0x09, 0x07, 4);
    
    bQueryInProgress = false;
    
    for(int i=0; i<255; i++) keyIsDown[i] = false;
}

// ----------------------------------
void ofxVaddio::update() {
    float now = ofGetElapsedTimef();
    
    if(bQueryInProgress && now>nextQuery) {
        nextQuery = now+0.5;
        
        if(queryZoom == -1) {
            queryZoom = getZoom();
        }
        
        if(queryPos.error) {
            queryPos = getPosition();
        }
        
        if(queryZoom!=-1 && !queryPos.error) {
            stringstream ss;
            ss << "ptz " << queryPos.pan << " " << queryPos.tilt << " " << queryZoom;
            ofSendMessage(ss.str());
            //ofSystemTextBoxDialog("PTZ command", ss.str());
            bQueryInProgress = false;
        }
    }
}

// ----------------------------------
void ofxVaddio::enableKeyEvents() {
    ofRegisterKeyEvents(this);
}

// ----------------------------------
void ofxVaddio::disableKeyEvents(){
    ofUnregisterKeyEvents(this);
}

// ----------------------------------
void ofxVaddio::writePacket(vector<int>& packet) {

    stringstream out;
    out << ofToHex((char)VADDIO_HEADER) << " ";
    serial.writeByte(VADDIO_HEADER);
    
    for(int i=0; i<packet.size(); i++) {
        serial.writeByte(packet[i]);
        out << ofToHex((char)packet[i]) << " ";
    }
    
    serial.writeByte(VADDIO_TERMINATOR);
    out << ofToHex((char)VADDIO_TERMINATOR);
    
    ofLogVerbose("ofxVaddio >") << out.str();
    
    stringstream in;
    packet.clear();
    
    int ch;
    int ttl = 1000;
    do {
        ch = serial.readByte();
        if(ch != OF_SERIAL_NO_DATA) {
            packet.push_back(ch);
            in << ofToHex((char)ch) << " ";
        }
        ttl--;
    } while(ch != VADDIO_TERMINATOR && ttl>0);

    ofLogVerbose("ofxVaddio <") << in.str();
}

// ----------------------------------
ofxVaddioPantiltPosition ofxVaddio::getPosition() {
    ofLogNotice("ofxVaddio") << "getPosition";
    
    vector<int> packet;
    packet.push_back(VADDIO_INQUIRY_PREFIX);
    packet.push_back(0x06);
    packet.push_back(0x12);
    writePacket(packet);
 
    ofxVaddioPantiltPosition pos;
    pos.error = false;
    pos.pan = 0;
    pos.tilt = 0;
    
    if(packet.size()<10)  {
        ofLogWarning() << "bad response from Pan-tiltPositionInq";
        pos.error = true;
        return pos;
    }
    
    // Lifted from VISCA_get_pantilt_position
    int pan =   ((packet[2] & 0xf) << 12) +
                ((packet[3] & 0xf) <<  8) +
                ((packet[4] & 0xf) <<  4) +
                (packet[5] & 0xf);
    
    int tilt =  ((packet[6] & 0xf) << 12) +
                ((packet[7] & 0xf) <<  8) +
                ((packet[8] & 0xf) <<  4) +
                (packet[9] & 0xf);
    
    if (pan > 0x8000) pan -= 0x10000;
    if (tilt > 0x8000) tilt -= 0x10000;
    
    
    pos.pan = ofMap(pan,
                    VADDIO_PAN_MIN,
                    VADDIO_PAN_MAX,
                    VADDIO_PAN_MIN_DEGREES,
                    VADDIO_PAN_MAX_DEGREES );
    pos.tilt = ofMap(tilt,
                     VADDIO_TILT_MIN,
                     VADDIO_TILT_MAX,
                     VADDIO_TILT_MIN_DEGREES,
                     VADDIO_TILT_MAX_DEGREES );
    
    ofLogNotice("ofxVaddio") << "pan = " << pos.pan << " tilt = " << pos.tilt;
    return pos;
}

// -----------------------------------
// CAM_ZoomPosInq  09 04 47 FF
float ofxVaddio::getZoom() {
    ofLogNotice("ofxVaddio") << "getZoom";
    
    vector<int> packet;
    packet.push_back(VADDIO_INQUIRY_PREFIX);
    packet.push_back(0x04);
    packet.push_back(0x47);
    writePacket(packet);
    
    if(packet.size()<6)  {
        ofLogWarning() << "bad response from CAM_ZoomPosInq";
        return -1;
    }
    
    int z =  (packet[2] << 12) +
                (packet[3] <<  8) +
                (packet[4] <<  4) +
                (packet[5]);
    
    float zoom = ofMap(z,
                 VADDIO_ZOOM_MIN,
                 VADDIO_ZOOM_MAX,
                 VADDIO_ZOOM_MIN_X,
                 VADDIO_ZOOM_MAX_X);
    
    ofLogNotice("ofxVaddio") << "zoom = " << zoom;
    return zoom;
}

// -----------------------------------
void ofxVaddio::panLeft() {
    ofLogNotice("ofxVaddio") << "panLeft";
    vector<int> packet;
    packet.push_back(VADDIO_COMMAND_PREFIX);
    packet.push_back(0x06);
    packet.push_back(0x01);
    packet.push_back(pan_speed);
    packet.push_back(tilt_speed);
    packet.push_back(0x01);
    packet.push_back(0x03);
    writePacket(packet);
}

// -----------------------------------
void ofxVaddio::panRight() {
    ofLogNotice("ofxVaddio") << "panRight";
    vector<int> packet;
    packet.push_back(VADDIO_COMMAND_PREFIX);
    packet.push_back(0x06);
    packet.push_back(0x01);
    packet.push_back(pan_speed);
    packet.push_back(tilt_speed);
    packet.push_back(0x02);
    packet.push_back(0x03);
    writePacket(packet);
}

// -----------------------------------
void ofxVaddio::tiltUp() {
    ofLogNotice("ofxVaddio") << "tiltUp";
    vector<int> packet;
    packet.push_back(VADDIO_COMMAND_PREFIX);
    packet.push_back(0x06);
    packet.push_back(0x01);
    packet.push_back(pan_speed);
    packet.push_back(tilt_speed);
    packet.push_back(0x03);
    packet.push_back(0x01);
    writePacket(packet);
}

// -----------------------------------
void ofxVaddio::tiltDown() {
    ofLogNotice("ofxVaddio") << "tiltDown";
    vector<int> packet;
    packet.push_back(VADDIO_COMMAND_PREFIX);
    packet.push_back(0x06);
    packet.push_back(0x01);
    packet.push_back(pan_speed);
    packet.push_back(tilt_speed);
    packet.push_back(0x03);
    packet.push_back(0x02);
    writePacket(packet);
}

// -----------------------------------
void ofxVaddio::zoomOut() {
    ofLogNotice("ofxVaddio") << "zoomOut";
    vector<int> packet;
    packet.push_back(VADDIO_COMMAND_PREFIX);
    packet.push_back(0x04);
    packet.push_back(0x07);
    packet.push_back(0x03);
    writePacket(packet);
}

// -----------------------------------
void ofxVaddio::zoomIn() {
    ofLogNotice("ofxVaddio") << "zoomIn";
    vector<int> packet;
    packet.push_back(VADDIO_COMMAND_PREFIX);
    packet.push_back(0x04);
    packet.push_back(0x07);
    packet.push_back(0x02);
    writePacket(packet);
}

// -----------------------------------
void ofxVaddio::stopPanTilt() {
    ofLogNotice("ofxVaddio") << "stopPanTilt";
    vector<int> packet;
    packet.push_back(VADDIO_COMMAND_PREFIX);
    packet.push_back(0x06);
    packet.push_back(0x01);
    packet.push_back(pan_speed);
    packet.push_back(tilt_speed);
    packet.push_back(0x03);
    packet.push_back(0x03);
    writePacket(packet);
}

// -----------------------------------
void ofxVaddio::stopZoom() {
    ofLogNotice("ofxVaddio") << "stopZoom";
    vector<int> packet;
    packet.push_back(VADDIO_COMMAND_PREFIX);
    packet.push_back(0x04);
    packet.push_back(0x07);
    packet.push_back(0x00);
    writePacket(packet);
}

// -----------------------------------
void ofxVaddio::home() {
//    ofLogNotice("ofxVaddio") << "home";
//    vector<int> packet;
//    packet.push_back(VADDIO_COMMAND_PREFIX);
//    packet.push_back(0x06);
//    packet.push_back(0x04);
//    writePacket(packet);
    absolutePosition(0, 0);
    zoomDirect(0);
}

// -----------------------------------
void ofxVaddio::absolutePosition(ofxVaddioPantiltPosition pos) {
    absolutePosition(pos.pan, pos.tilt);
}

// -----------------------------------
void ofxVaddio::absolutePosition(int pan_pos, int tilt_pos) {
    ofLogNotice("ofxVaddio") << "absolutePosition " << pan_pos << " " << tilt_pos;
    
    pan_pos = ofMap(pan_pos,
                  VADDIO_PAN_MIN_DEGREES,
                  VADDIO_PAN_MAX_DEGREES,
                  VADDIO_PAN_MIN,
                  VADDIO_PAN_MAX);
    tilt_pos = ofMap(tilt_pos,
                  VADDIO_TILT_MIN_DEGREES,
                  VADDIO_TILT_MAX_DEGREES,
                  VADDIO_TILT_MIN,
                  VADDIO_TILT_MAX);
    
    vector<int> packet;
    packet.push_back(VADDIO_COMMAND_PREFIX);
    packet.push_back(0x06);
    packet.push_back(0x02);
    packet.push_back(pan_speed);
    packet.push_back(tilt_speed);
    
    // Lifted from  VISCA_set_pantilt_absolute_position
    packet.push_back((pan_pos & 0x0f000) >> 12);
    packet.push_back((pan_pos & 0x00f00) >>  8);
    packet.push_back((pan_pos & 0x000f0) >>  4);
    packet.push_back((pan_pos & 0x0000f)      );
    
    packet.push_back((tilt_pos & 0xf000) >> 12);
    packet.push_back((tilt_pos & 0x0f00) >>  8);
    packet.push_back((tilt_pos & 0x00f0) >>  4);
    packet.push_back((tilt_pos & 0x000f)      );
    
    writePacket(packet);
}

// -----------------------------------
void ofxVaddio::presetSpeed(int pan, int tilt, int zoom) {
    ofLogNotice("ofxVaddio") << "presetSpeed";
    vector<int> packet;
    packet.push_back(VADDIO_COMMAND_PREFIX);
    packet.push_back(0x7E);
    packet.push_back(0x01);
    packet.push_back(0x0B);
    packet.push_back(ofClamp(pan, VADDIO_PAN_SPEED_MIN, VADDIO_PAN_SPEED_MAX));
    packet.push_back(ofClamp(tilt, VADDIO_TILT_SPEED_MIN, VADDIO_TILT_SPEED_MAX));
    packet.push_back(ofClamp(zoom, VADDIO_ZOOM_SPEED_MIN, VADDIO_ZOOM_SPEED_MAX));
    writePacket(packet);
}

// -----------------------------------
void ofxVaddio::memoryReset(int num) {
    ofLogNotice("ofxVaddio") << "memoryReset";
    vector<int> packet;
    packet.push_back(VADDIO_COMMAND_PREFIX);
    packet.push_back(0x04);
    packet.push_back(0x3F);
    packet.push_back(0x00);
    packet.push_back(num);
    writePacket(packet);
}

// -----------------------------------
void ofxVaddio::memorySet(int num) {
    ofLogNotice("ofxVaddio") << "memorySet";
    vector<int> packet;
    packet.push_back(VADDIO_COMMAND_PREFIX);
    packet.push_back(0x04);
    packet.push_back(0x3F);
    packet.push_back(0x01);
    packet.push_back(num);
    writePacket(packet);
}

// -----------------------------------
void ofxVaddio::memoryRecall(int num) {
    ofLogNotice("ofxVaddio") << "memoryRecall";
    vector<int> packet;
    packet.push_back(VADDIO_COMMAND_PREFIX);
    packet.push_back(0x04);
    packet.push_back(0x3F);
    packet.push_back(0x02);
    packet.push_back(num);
    writePacket(packet);
}

// -----------------------------------
void ofxVaddio::zoomDirect(int zoom) {
    ofLogNotice("ofxVaddio") << "zoomDirect " << zoom;
    zoom = ofMap(zoom,
                  VADDIO_ZOOM_MIN_X,
                  VADDIO_ZOOM_MAX_X,
                  VADDIO_ZOOM_MIN,
                  VADDIO_ZOOM_MAX);
    
    vector<int> packet;
    packet.push_back(VADDIO_COMMAND_PREFIX);
    packet.push_back(0x04);
    packet.push_back(0x47);
    packet.push_back((zoom & 0xF000) >> 12);
    packet.push_back((zoom & 0x0F00) >>  8);
    packet.push_back((zoom & 0x00F0) >>  4);
    packet.push_back((zoom & 0x000F));
    
    writePacket(packet);
}

// ----------------------------------
void ofxVaddio::keyPressed(ofKeyEventArgs& args) {
    if(keyIsDown[args.key]) return;
    
    if(args.key==OF_KEY_LEFT) {
        panLeft();
    }
    if(args.key==OF_KEY_RIGHT) {
        panRight();
    }
    if(args.key==OF_KEY_UP) {
        tiltUp();
    }
    if(args.key==OF_KEY_DOWN) {
        tiltDown();
    }
    if(args.key=='h') {
        home();
    }
    if(args.key==']') {
        zoomIn();
    }
    if(args.key=='[') {
        zoomOut();
    }
    if(args.key=='p') {
        ofLogNotice("ofxVaddio") << "Starting query";
        bQueryInProgress=true;
        queryZoom = -1;
        queryPos.error = true;
        nextQuery = ofGetElapsedTimef();
    }
    keyIsDown[args.key]=true;
}

// ----------------------------------
void ofxVaddio::keyReleased(ofKeyEventArgs& args) {
    keyIsDown[args.key]=false;
    
    if(args.key==OF_KEY_LEFT || args.key==OF_KEY_RIGHT || args.key==OF_KEY_UP || args.key==OF_KEY_DOWN) {
        stopPanTilt();
    }
    if(args.key==']' || args.key=='[') {
         stopZoom();
    }
}

// ----------------------------------
void ofxVaddio::close() {
    serial.close();
}