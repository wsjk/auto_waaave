/*
 * Copyright (c) 2013 Dan Wilcox <danomatika@gmail.com>
 *
 * BSD Simplified License.
 * For information on usage and redistribution, and for a DISCLAIMER OF ALL
 * WARRANTIES, see the file, "LICENSE.txt," in this distribution.
 *
 * See https://github.com/danomatika/ofxMidi for documentation
 *
 */
#pragma once

#include "ofMain.h"
#include "ofxMidi.h"
//#include "ofxOMXVideoGrabber.h"
#include "ofxProcessFFT.h"
#include <map>
#include "ofxJSON.h" // Add this for JSON parsing
#include <unordered_map>
#include <string>

class ofApp : public ofBaseApp, public ofxMidiListener {
	
public:
	
	void setup();
	void update();
	void draw();
	void exit();
	
	void keyPressed(int key);
	void keyReleased(int key);
	/*
	void omx_settings();
	void omx_updates();
	ofxOMXCameraSettings settings;
    ofxOMXVideoGrabber videoGrabber;
	*/
	void midibiz();
	void midibizOld();
	void newMidiMessage(ofxMidiMessage& eventArgs);
	void midiSetup();
	ofxMidiIn midiIn;
	std::vector<ofxMidiMessage> midiMessages;
	std::size_t maxMessages = 10; //< max number of messages to keep track of

	ofShader shader_mixer;
	ofShader shaderSharpen;

	void fbDeclareAndAllocate();
    ofFbo framebuffer0;
    ofFbo framebuffer1;
    ofFbo sharpenFramebuffer;
    
    void inputSetup();
    void inputUpdate();
    ofVideoGrabber cam1;
    
    void p_lockClear();
    void p_lockUpdate();

    ProcessFFT fft;
    void fftAssignValues();
    
    void parametersAssign();
    void midiLatchClear();

	// MIDI mapping support
	void loadMidiMapping();
	void reloadMidiMappingIfChanged();
	std::string midiMappingPath = "midi-mapping.json";
	ofxJSONElement midiMappingJson;
	uint64_t midiMappingTimestamp = 0;

	struct MidiAction {
		std::string type; // "toggle", "set", "value", etc.
		std::string target; // variable name or action
		float scale = 1.0f;
		float offset = 0.0f;
		bool bipolar = false;
	};

	std::map<int, MidiAction> midiCCMap;
	std::map<int, MidiAction> midiButtonMap;
	uint64_t nextMidiMappingCheckMillis = 0;

	// Key mapping support
	void loadKeyMapping();
	void reloadKeyMappingIfChanged();
	std::string keyMappingPath = "key-mapping.json";
	ofxJSONElement keyMappingJson;
	uint64_t keyMappingTimestamp = 0;

	struct KeyAction {
		std::string type;   // "action"
		std::string target; // action name
	};

	std::unordered_map<int, KeyAction> keyActionMap;
	uint64_t nextKeyMappingCheckMillis = 0;
};
