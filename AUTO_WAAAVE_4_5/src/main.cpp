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
#include "ofMain.h"
#include "ofApp.h"
#include <string>

int main(int argc, char *argv[]){
	//ofGLWindowSettings settings;
	//settings.setGLVersion(3,2);

	// Parse command-line arguments for MIDI latching
	bool midiLatchingEnabled = true; // Default is enabled

	for(int i = 1; i < argc; i++){
		std::string arg = argv[i];
		if(arg == "--midi-latching-on" || arg == "-l"){
			midiLatchingEnabled = true;
			cout << "Command line: MIDI latching ENABLED" << endl;
		}
		else if(arg == "--midi-latching-off" || arg == "-L"){
			midiLatchingEnabled = false;
			cout << "Command line: MIDI latching DISABLED" << endl;
		}
		else if(arg == "--help" || arg == "-h"){
			cout << "AUTO_WAAAVE_4_5 - Video Synthesis Application" << endl;
			cout << "Usage: " << argv[0] << " [options]" << endl;
			cout << "Options:" << endl;
			cout << "  -l, --midi-latching-on   Enable MIDI latching (default)" << endl;
			cout << "  -L, --midi-latching-off  Disable MIDI latching" << endl;
			cout << "  -h, --help               Show this help message" << endl;
			return 0;
		}
	}

	ofGLESWindowSettings settings;
	settings.glesVersion=2;
	
	//here is where you change output resolutions
	
	//CEA mode 2
	settings.setSize(720,480);
	
	//CEA mode 4
	//settings.setSize(1280,720);
	
	//CEA mode 16
	//settings.setSize(1920,1080);
	
	
	ofCreateWindow(settings);
    
	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofApp *app = new ofApp();
	app->setMidiLatching(midiLatchingEnabled);
	ofRunApp(app);
}
