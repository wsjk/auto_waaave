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
#include "ofxJSON.h"
//#include "ofxOMXVideoGrabber.h"
#include "ofxProcessFFT.h"
#include <unordered_map>

struct MidiContinuousMap {
	int cc;
	int index;
	std::string name;
	bool bipolar = false;
	float minVal = 0.0f;
	float maxVal = 1.0f;
};

struct MidiButtonMap {
	int cc;
	std::string action;
};

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

	// MIDI configuration
	ofxJSON midiConfig;

	// New dynamic MIDI mapping data
	std::unordered_map<int, MidiContinuousMap> continuousMap;
	std::unordered_map<int, MidiButtonMap> buttonMap;

	// Config management
	void loadMidiConfig();              // (re)load file
	void rebuildMidiMaps();             // parse JSON into maps
	void reloadMidiConfigIfChanged();   // check timestamp
	uint64_t midiConfigTimestamp = 0;
	uint64_t nextMidiConfigCheckMillis = 0;
	const std::string midiConfigPath = "midi-config.json";

	// Handlers
	void applyContinuous(int cc, int value);
	void applyButton(int cc, int value);

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
};
