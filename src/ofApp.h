#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxDropdown.h"

struct AudioClip {
public:
    ofSoundPlayer sound;
    ofPoint point;
    float t;
    float time;
    string speciesType;
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
    void gotMessage(ofMessage msg);
    
    vector<AudioClip> sounds;
    
    ofParameter<float> maxDuration;
    ofParameter<float> mouseRadius;
    ofParameter<float> pauseLength;
    ofxFloatSlider minTime; // Slider for minimum time
    ofxFloatSlider maxTime; // Slider for maximum time
    ofxFloatSlider space;
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


