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
	// MIDI handling structures for optimized control
	struct MidiControlConfig {
		int ccNumber;
		int paramIndex;
		float* videoReactivePtr;
		float* lowPtr;
		float* midPtr;
		float* highPtr;
		bool isBipolar;  // true = use MIDI_MAGIC offset
		float multiplier; // for x_2, x_5, etc.
	};

	void midibiz();
	void midibizOptimized();
	void processMidiControl(ofxMidiMessage& msg, MidiControlConfig& ctrl);
	void newMidiMessage(ofxMidiMessage& eventArgs);
	void midiSetup();
	ofxMidiIn midiIn;
	std::vector<ofxMidiMessage> midiMessages;
	std::size_t maxMessages = 2; // OPTIMIZATION: Reduced from 10 to 2

	ofShader shader_mixer;
	ofShader shaderSharpen;

	// OPTIMIZATION: Shader uniform caching to avoid redundant updates
	struct ShaderUniforms {
		float lumakey, fbMix, fbHue, fbSat, fbBright;
		float temporalFilterMix, temporalFilterResonance;
		float sharpenAmount;
		float x, y, z, rotate;
		float huexMod, huexOff, huexLfo;
		int toroidSwitch, mirrorSwitch, brightInvert, hueInvert, saturationInvert;
		int horizontalMirror, verticalMirror, lumakeyInvertSwitch;
	};
	ShaderUniforms currentUniforms;
	ShaderUniforms previousUniforms;
	bool forceUniformUpdate;

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
