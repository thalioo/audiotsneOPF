#include "ofApp.h"
#include <regex>

float ofApp::convertTimeStringToMinutes(const std::string &timeStr) {
    std::regex timeFormatPattern(R"(\d{2}:\d{2})");
    if(std::regex_match(timeStr, timeFormatPattern)){
        int hours = std::stoi(timeStr.substr(0, 2));
        int minutes = std::stoi(timeStr.substr(3, 2));
        return hours * 60.0f + minutes;
    }

    return 0;
}

//--------------------------------------------------------------
void ofApp::setup() {
    tsnePath = ofToDataPath("output.json");
    bLoad.addListener(this, &ofApp::eLoad);
    gui.setup();
    gui.setName("Audio t-SNE");
    // Initialize sliders with default values
    gui.add(maxDuration.set("maxDuration", 1.0, 0.1, 2.0));
    gui.add(mouseRadius.set("mouseRadius", 250, 100, 500));
    gui.add(pauseLength.set("pauseLength", 2.0, 0.2, 5.0));
    gui.add(minTime.setup("Min Time", 0.0, 0.0, 1440.0)); 
    gui.add(maxTime.setup("Max Time", 1440.0, 0.0, 1440.0)); 
    gui.add(minSpace.setup("Less Green Space", -1.0, 0.0, 1.0));
    gui.add(maxSpace.setup("Most Green Space", 1.0, 0.0, 1.0)); 
    gui.add(bLoad.setup("Load model"));
    // dropdown settin
    speciesTypes.setName("Species");
    spDropdown =  make_unique<ofxDropdown>("Metadata");
    gui.add(spDropdown->getDropdownParameters());
    spDropdown->addListener(this, &ofApp::onDropdownChange);
    
    load(tsnePath);

    spPath = ofToDataPath("species.json");
    ofJson jsp;
    ofFile file(spPath);
    file >> jsp;
    for (auto & entry : jsp) {
        if (!entry.empty()) {
            species.push_back(entry.get<std::string>());
        }
    }   
		for(int i = 0; i < 5; i++){
			spDropdown->add(species[i]);
		}
    gui.add(spDropdown.get());
    bClear.addListener(this, &ofApp::eClear);
    gui.add(bClear.setup("Clear Selection"));
    selectedSpecies = "";
    // sending messssage
    oscHost = "192.168.1.193"; // Change this to the desired host
    oscPort = 3333; // Change this to the desired port
    oscSender.setup(oscHost, oscPort);

}
//--------------------------------------------------------------
void ofApp::eLoad() {
    ofFileDialogResult result = ofSystemLoadDialog("Select JSON file", true);
    if (result.bSuccess) {
        load(result.filePath);
    }
}
void ofApp::eClear() {
    selectedSpecies = ""; // Clear the selection to show all species
    // spDropdown->selectNone(); // Clear the dropdown selection
    ofLog() << "Selection cleared.";
}
//--------------------------------------------------------------
void ofApp::load(string filename) {
    tsnePath = filename;

    ofJson js;
    ofFile file(tsnePath);
    parsingSuccessful = file.exists();
    
    if (!parsingSuccessful) {
        ofLog(OF_LOG_ERROR) << "parsing not successful";
        return;
    }

    sounds.clear();
    file >> js;
    species.clear();
    
    ofVec3f minPoint(1e17, 1e18);
    ofVec3f maxPoint(-1e18, -1e18);
    
    for (auto & entry : js) {
        if (!entry.empty()) {
            string path = entry["path"];
            float x = entry["point"][0];
            float y = entry["point"][1];
            float z = entry["point"][2];
            string timeStr = entry["time"];
            float time = convertTimeStringToMinutes(timeStr);
            string species = entry["species"];
            float green_score = entry["green_space"];
            int id = entry["id"];
            
            minPoint.x = min(minPoint.x, x);
            minPoint.y = min(minPoint.y, y);
            minPoint.z = min(minPoint.z, z);
            maxPoint.x = max(maxPoint.x, x);
            maxPoint.y = max(maxPoint.y, y);
            maxPoint.z = max(maxPoint.z, z);
            AudioClip newSound;
            
            newSound.path = path;
            newSound.sound.load(path);
            newSound.point.set(x, y,z);
            newSound.t = 0;
            newSound.time = time;
            newSound.speciesType = species;
            newSound.green_score = green_score;
            newSound.id = id;
            sounds.push_back(newSound);
            
        }
        
    }

    // Normalize the points
    for (int i = 0; i < sounds.size(); i++) {
        sounds[i].point.set(ofMap(sounds[i].point.x, minPoint.x, maxPoint.x, 0, 1),
                            ofMap(sounds[i].point.y, minPoint.y, maxPoint.y, 0, 1),
                            ofMap(sounds[i].point.z, minPoint.z, maxPoint.z, 0, 1));
    }


}

//--------------------------------------------------------------
void ofApp::update() {
    if (!parsingSuccessful) {
        return;
    }
    
    for (int i = 0; i < sounds.size(); i++) {
        if (sounds[i].sound.isPlaying() && sounds[i].sound.getPositionMS() > maxDuration * 1000) {
            sounds[i].sound.stop();
        }
    }
    ofSoundUpdate();
}

//--------------------------------------------------------------
void ofApp::draw() {
    ofBackgroundGradient(ofColor(100), ofColor(20));
    if (!parsingSuccessful) {
        ofDrawBitmapString("Could not find file " + tsnePath + "\nSee the instructions for how to create one.", 50, 50);
        return;
    }

    for (int i = 0; i < sounds.size(); i++) {
        if ((sounds[i].time >= minTime && sounds[i].time <= maxTime)
         && (selectedSpecies == "" || sounds[i].speciesType == selectedSpecies)
         && (sounds[i].green_score >= minSpace && sounds[i].green_score <= maxSpace)
         ) {
            if (sounds[i].sound.isPlaying()) {
                ofSetColor(0, 255, 0, 180);
            } else {
                ofSetColor(255, 180);
            }
            ofDrawCircle(ofGetWidth() * sounds[i].point.x, ofGetHeight() * sounds[i].point.y, 4);
        }
        else {
            // Stop sounds that should not be displayed
            if (sounds[i].sound.isPlaying()) {
                sounds[i].sound.stop();
            }
        }
    }
    gui.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {
    for (int i = 0; i < sounds.size(); i++) {
        float distanceToMouse = ofDistSquared(x, y, ofGetWidth() * sounds[i].point.x, ofGetHeight() * sounds[i].point.y);
        if (distanceToMouse < mouseRadius && !sounds[i].sound.isPlaying() && (ofGetElapsedTimef() - sounds[i].t > pauseLength)) {
            sounds[i].t = ofGetElapsedTimef();
            sounds[i].sound.play();
            string sound_path = sounds[i].path;
            std::string filename = sound_path.substr(sound_path.find_last_of('/') + 1);
            std::cout<<filename<<std::endl;
            sendMessage(filename,sounds[i].point.x,sounds[i].point.y,sounds[i].point.z);


        }
    }
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {
}
void ofApp::sendMessage(string file,float x,float y,float z){
    ofxOscMessage m;
    m.setAddress("/test");
    m.addStringArg(file);
    m.addFloatArg(x);
    m.addFloatArg(y);
    m.addFloatArg(z);
    std::cout<<z<<std::endl;
    oscSender.sendMessage(m, false);
}
//--------------------------------------------------------------
set<string> ofApp::extractUniqueSpeciesTypes(const ofJson &json) {
    set<string> uniqueSpeciesSet;
    
    for (const auto& entry : json) {
        if (!entry.empty()) {
            string species = entry["sp"];
            uniqueSpeciesSet.insert(species);
        }
    }
    return uniqueSpeciesSet;
    // Convert set to vector
    // speciesTypes.assign(uniqueSpeciesSet.begin(), uniqueSpeciesSet.end());
}

// //--------------------------------------------------------------
void ofApp::onDropdownChange(string &selectedItem) {
    selectedSpecies = selectedItem;
    ofLog() << "Selected species: " << selectedSpecies;
}
