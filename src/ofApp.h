#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxDropdown.h"
#include "ofxOsc.h"
#define PORT 3333

struct AudioClip {
public:
    ofSoundPlayer sound;
    ofPoint point;
    float t;
    float time;
    string speciesType;
    float green_score;
    int id;
};

class ofApp : public ofBaseApp{
    
public:

    void setup();
    void update();
    void draw();
    
    void eLoad();
    void eClear();
    void load(string file);
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void sendMessage(int soundID);
    
    vector<AudioClip> sounds;
    
    // osc tings
    ofxOscSender oscSender;
    std::string oscHost;
    int oscPort;

    ofParameter<float> maxDuration;
    ofParameter<float> mouseRadius;
    ofParameter<float> pauseLength;
    ofxFloatSlider minTime; 
    ofxFloatSlider maxTime; 
    ofxFloatSlider maxSpace;
    ofxFloatSlider minSpace;
    ofxButton bLoad;
    ofxButton bClear;
    ofxPanel gui;
    bool parsingSuccessful;
    string tsnePath;
    string spPath;

    // void onDropdownChange(int &selectedIndex);
    ofParameter<string> speciesTypes;
    unique_ptr<ofxDropdown> spDropdown;
    vector<std::string> species;
    string selectedSpecies;
    void onDropdownChange(std::string &selectedItem);
    private:
        float convertTimeStringToMinutes(const std::string& timeStr);
        set<string> extractUniqueSpeciesTypes(const ofJson& json);

};


