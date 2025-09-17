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
#include "ofApp.h"

#include "iostream"

#define MIDI_MAGIC 63.50f

#define CONTROL_THRESHOLD .035f

//flip this switch to try different scalings
//0 is 320 1 is 640
//if you reduce scale to 320 you can up the total delay time to
//about 4 seconds/ 120 frames
//so try that out sometime and see how that feels!
bool scaleswitch=1;
const int framebufferLength=60;
//const int framebufferLength=120;

//0 is picaputre, 1 is usbinput
bool inputswitch=1;

//0 is wet (framebuffer fed from final output, internal
//feedback mode
//1 is dry (framebuffer fed direct from camera input,
//traditional video delay mode
bool wet_dry_switch=1;

//0 is sd aspect ratio
//use definitely with all of the VSERPI devices 
//and anything else doing like 480i/p over hdmi
//1 is corner cropping to fill the screen
int hdmi_aspect_ratio_switch=0;

float az = 1.0;
float sx = 0;
float dc = 0;
float fv = 1;
float gb = 1;
float hn = 1;
float jm = 0.0;
float kk = 0.0;
float ll = 0.0;
float qw = 0.0;
float er = 1.0;
float ty = 0.0;
float ui = 0.0;
float op = 0.0;

float fb1_brightx=0.0;
bool toroidSwitch=0;
bool y_skew_switch=FALSE;
bool x_skew_switch=FALSE;
bool lumakeyInvertSwitch=FALSE;
bool x_mirrorSwitch=FALSE;
bool y_mirrorSwitch=FALSE;
float y_skew=0;
float x_skew=0;
bool mirrorSwitch=FALSE;

int fb0_delayamount=0;

//dummy variables for midi control

int width=640;
int height=480;


//dummy variables for midi to audio attenuatiors
//0 is direct midi, 1 is low_x, 2 is mid_x, 3 is high_x
int audioReactiveControlSwitch=0;

float lowC1=0.0;
float lowC2=0.0;
float lowC3=0.0;
float lowC4=0.0;
float lowC5=0.0;
float lowC6=0.0;
float lowC7=0.0;
float lowC8=0.0;
float lowC9=0.0;
float lowC10=0.0;
float lowC11=0.0;
float lowC12=0.0;
float lowC13=0.0;
float lowC14=0.0;
float lowC15=0.0;
float lowC16=0.0;

float midC1=0.0;
float midC2=0.0;
float midC3=0.0;
float midC4=0.0;
float midC5=0.0;
float midC6=0.0;
float midC7=0.0;
float midC8=0.0;
float midC9=0.0;
float midC10=0.0;
float midC11=0.0;
float midC12=0.0;
float midC13=0.0;
float midC14=0.0;
float midC15=0.0;
float midC16=0.0;

float highC1=0.0;
float highC2=0.0;
float highC3=0.0;
float highC4=0.0;
float highC5=0.0;
float highC6=0.0;
float highC7=0.0;
float highC8=0.0;
float highC9=0.0;
float highC10=0.0;
float highC11=0.0;
float highC12=0.0;
float highC13=0.0;
float highC14=0.0;
float highC15=0.0;
float highC16=0.0;


bool clear_switch=0;
//toggle switch variables
bool hueInvert=FALSE;
bool saturationInvert=FALSE;
bool brightInvert=FALSE;

bool cam1_h_invert=FALSE;
bool cam1_s_invert=FALSE;
bool cam1_b_invert=FALSE;

bool horizontalMirror=FALSE;
bool verticalMirror=FALSE;

bool clear_flip=FALSE;

bool x_2=FALSE;
bool x_5=FALSE;
bool x_10=FALSE;

bool y_2=FALSE;
bool y_5=FALSE;
bool y_10=FALSE;

bool z_2=FALSE;
bool z_5=FALSE;
bool z_10=FALSE;

bool r_2=FALSE;
bool r_5=FALSE;
bool r_10=FALSE;

bool huexx_0=FALSE;
bool huexx_1=FALSE;
bool huexx_2=FALSE;

bool huexy_0=FALSE;
bool huexy_1=FALSE;
bool huexy_2=FALSE;

bool huexz_0=FALSE;
bool huexz_1=FALSE;
bool huexz_2=FALSE;

//framebuffer management biziness

//framebufferLength is the total number of frames that will be stored.  setting framebufferLength to
//30 while fps is set to 30 means 1 second of video will be stored in memory  
//75 seems to work ok with usb cams but the capture device might need a little more memory

int framedelayoffset=0;  // this is used as an index to the circular framebuffers eeettt
unsigned int framecount=0; // framecounter used to calc offset eeettt

//this is an array of framebuffer objects
ofFbo pastFrames[framebufferLength];


void incIndex()  // call this every frame to calc the offset eeettt
{
    framecount++;
    framedelayoffset=framecount % framebufferLength;
}

//p_lock biz
//maximum total size of the plock array
const int p_lock_size=240;

bool p_lock_switch=0;

bool p_lock_erase=0;

//maximum number of p_locks available...maybe there can be one for every knob
//for whatever wacky reason the last member of this array of arrays has a glitch
//so i guess just make an extra array and forget about it for now
const int p_lock_number=17;

//so how we will organize the p_locks is in multidimensional arrays
//to access the data at timestep x for p_lock 2 (remember counting from 0) we use p_lock[2][x]
float p_lock[p_lock_number][p_lock_size];

//smoothing parameters(i think for all of these we can array both the arrays and the floats
//for now let us just try 1 smoothing parameter for everything.
float p_lock_smooth=.5;

//and then lets try an array of floats for storing the smoothed values
float p_lock_smoothed[p_lock_number];

//turn on and off writing to the array
bool p_lock_0_switch=1;

//global counter for all the locks
int p_lock_increment=0;

float my_normalize=0;

float low_value_smoothed=0.0;
float mid_value_smoothed=0.0;
float high_value_smoothed=0.0;
float smoothing_rate=.8;

ofFbo aspect_fix_fbo;
ofFbo dry_framebuffer;

bool videoReactiveSwitch=FALSE;

float vLumakeyValue=0.0;
float vMix=0.0;
float vHue=0.0;
float vSaturation=0.0;
float vBright=0.0;
float vTemporalFilterMix=0.0;
float vTemporalFilterResonance=0.0;
float vSharpenAmount=0.0;
float vX=0.0;
float vY=0.0;
float vZ=0.0;
float vRotate=0.0;
float vHuexMod=0.0;
float vHuexOff=0.0;
float vHuexLfo=0.0;

//latching
bool midiActiveFloat[17];
bool vmidiActiveFloat[17];
bool midiLowActiveFloat[17];
bool midiMidActiveFloat[17];
bool midiHighActiveFloat[17];

float range=200;
float fftLow;
float fftMid;
float fftHigh;

float c_lumakey_value=1.01;
float c_mix=2;
float c_hue=.25;
float c_sat=.2;
float c_bright=.2;
float c_temporalFilterMix=1.1;
float c_sharpenAmount=.5;
float c_x=.01;
float c_y=.01;
float c_z=.05;
float c_rotate=.314159265;
float c_huex_off=.25;
float c_huex_lfo=.25;

int d_delay;
float d_lumakey_value;
float d_mix;
float d_hue;
float d_sat;
float d_bright;
float d_temporalFilterMix;
float d_temporalFilterResonance;
float d_sharpenAmount;
float d_x;
float d_y;
float d_z;
float d_rotate;
float d_huex_mod;
float d_huex_off;
float d_huex_lfo;
//--------------------------------------------------------------
void ofApp::setup() {
    ofSetFrameRate(30);
    ofBackground(0);
    ofHideCursor();
    inputSetup();
    midiSetup();
    loadMidiConfig(); // Load MIDI configuration
    fbDeclareAndAllocate();
    shader_mixer.load("shadersES2/shader_mixer");
    shaderSharpen.load("shadersES2/shaderSharpen");
    fft.setup();
    fft.setNormalize(false);
    p_lockClear();
    midiLatchClear();
}

//--------------------------------------------------------------
void ofApp::loadMidiConfig() {
	ofFile f(midiConfigPath);
	if(!f.exists()) {
		ofLogError("MIDI") << "Config missing: " << midiConfigPath;
		return;
	}
	if(!midiConfig.open(midiConfigPath)) {
		ofLogError("MIDI") << "Failed to parse JSON: " << midiConfigPath;
		return;
	}
	midiConfigTimestamp = f.getPocoFile().getLastModified().epochMicroseconds();
	rebuildMidiMaps();
	ofLogNotice("MIDI") << "Loaded mappings: continuous=" << continuousMap.size()
	                    << " buttons=" << buttonMap.size();
}

// New: rebuild maps from JSON (supports flexible schema)
void ofApp::rebuildMidiMaps() {
	continuousMap.clear();
	buttonMap.clear();

	if(midiConfig.isMember("continuous") && midiConfig["continuous"].isArray()) {
		for(auto &c : midiConfig["continuous"]) {
			MidiContinuousMap m;
			m.cc      = c.get("cc",-1).asInt();
			m.index   = c.get("index",-1).asInt();
			m.name    = c.get("name","").asString();
			m.bipolar = c.get("bipolar", false).asBool();
			m.minVal  = c.get("min", 0.0).asFloat();
			m.maxVal  = c.get("max", 1.0).asFloat();
			if(m.cc >= 0 && m.index >= 0) {
				continuousMap[m.cc] = m;
			}
		}
	}

	if(midiConfig.isMember("buttons") && midiConfig["buttons"].isArray()) {
		for(auto &b : midiConfig["buttons"]) {
			MidiButtonMap m;
			m.cc     = b.get("cc",-1).asInt();
			m.action = b.get("action","").asString();
			if(m.cc >= 0 && !m.action.empty()) {
				buttonMap[m.cc] = m;
			}
		}
	}
}

// Throttled hot-reload
void ofApp::reloadMidiConfigIfChanged() {
	uint64_t now = ofGetElapsedTimeMillis();
	if(now < nextMidiConfigCheckMillis) return;
	nextMidiConfigCheckMillis = now + 500;

	ofFile f(midiConfigPath);
	if(!f.exists()) return;
	uint64_t ts = f.getPocoFile().getLastModified().epochMicroseconds();
	if(ts != midiConfigTimestamp) {
		ofLogNotice("MIDI") << "Config changed on disk, reloading";
		loadMidiConfig();
	}
}

// Apply continuous mapping generically
void ofApp::applyContinuous(int cc, int value) {
	auto it = continuousMap.find(cc);
	if(it == continuousMap.end()) return;
	const MidiContinuousMap &m = it->second;
	if(m.index < 0 || m.index >= 16) return;

	float norm = m.bipolar
		? ( (value - MIDI_MAGIC) / MIDI_MAGIC )
		: ( value / 127.0f );

	float scaled = ofMap(norm,
	                     m.bipolar ? -1.0f : 0.0f,
	                     1.0f,
	                     m.minVal,
	                     m.maxVal,
	                     true);

	if(audioReactiveControlSwitch == 0 && p_lock_0_switch == 1) {
		if(abs(scaled - p_lock[m.index][p_lock_increment]) < CONTROL_THRESHOLD) {
			midiActiveFloat[m.index] = true;
		}
		if(midiActiveFloat[m.index]) {
			p_lock[m.index][p_lock_increment] = scaled;
		}
	}
	if(videoReactiveSwitch == 1) {
		switch(m.index) {
			case 0:  vLumakeyValue = scaled; break;
			case 1:  vMix = scaled; break;
			case 2:  vHue = scaled; break;
			case 3:  vSaturation = scaled; break;
			case 4:  vBright = scaled; break;
			case 5:  vTemporalFilterMix = scaled; break;
			case 6:  vTemporalFilterResonance = scaled; break;
			case 7:  vSharpenAmount = scaled; break;
			case 8:  vX = scaled; break;
			case 9:  vY = scaled; break;
			case 10: vZ = scaled; break;
			case 11: vRotate = scaled; break;
			case 12: vHuexMod = scaled; break;
			case 13: vHuexOff = scaled; break;
			case 14: vHuexLfo = scaled; break;
			default: break;
		}
	}
}

// Apply button/action mapping
void ofApp::applyButton(int cc, int value) {
	auto it = buttonMap.find(cc);
	if(it == buttonMap.end()) return;
	const std::string &action = it->second.action;

	bool pressed = (value > 0);
	if(action == "videoReactiveToggle") {
		if(pressed) { videoReactiveSwitch = !videoReactiveSwitch; p_lock_0_switch = videoReactiveSwitch ? 0 : 1; }
	}
	else if(action == "pLockEnable") {
		p_lock_switch = pressed;
		if(pressed) {
			for(int i=0;i<p_lock_number;i++){
				p_lock_smoothed[i]=0;
				for(int j=0;j<p_lock_size;j++){
					p_lock[i][j]=p_lock[i][p_lock_increment];
				}
			}
		}
	}
	else if(action == "clearFrames" && pressed) {
		framebuffer0.begin(); ofClear(0,0,0,255); framebuffer0.end();
		for(int i=0;i<framebufferLength;i++){ pastFrames[i].begin(); ofClear(0,0,0,255); pastFrames[i].end(); }
	}
	else if(action == "resetAll" && pressed) {
		vLumakeyValue=vMix=vHue=vSaturation=vBright=vTemporalFilterMix=vTemporalFilterResonance=vSharpenAmount=
		vX=vY=vZ=vRotate=vHuexMod=vHuexOff=vHuexLfo=0.0f;
		for(int i=0;i<p_lock_number;i++){
			for(int j=0;j<p_lock_size;j++){ p_lock[i][j]=0.0f; }
			midiActiveFloat[i]=vmidiActiveFloat[i]=midiLowActiveFloat[i]=midiMidActiveFloat[i]=midiHighActiveFloat[i]=0;
		}
	}
	else if(action == "wetDryToggle" && pressed) {
		wet_dry_switch = !wet_dry_switch;
	}
	else if(action == "brightInvertToggle") {
		brightInvert = pressed;
	}
	else if(action == "satInvertToggle") {
		saturationInvert = pressed;
	}
	else if(action == "aspectRatioToggle" && pressed) {
		hdmi_aspect_ratio_switch = !hdmi_aspect_ratio_switch;
	}
}

//-----------------------------------------------------------
void ofApp::midibiz(){
	reloadMidiConfigIfChanged();

	for(unsigned int i = 0; i < midiMessages.size(); ++i) {
		ofxMidiMessage &message = midiMessages[i];
		if(message.status < MIDI_SYSEX && message.status == MIDI_CONTROL_CHANGE) {

			if(buttonMap.find(message.control) != buttonMap.end()) {
				applyButton(message.control, message.value);
				continue;
			}
			if(continuousMap.find(message.control) != continuousMap.end()) {
				applyContinuous(message.control, message.value);
				continue;
			}

			// 3. Fallback legacy handling (unchanged)
			//videoreactive
            if(message.control==39){
                if(message.value==127){
                    videoReactiveSwitch=1;
                    p_lock_0_switch=0;
                }
                if(message.value==0){
                    videoReactiveSwitch=0;
                    p_lock_0_switch=1;
                }
            }
            //audioreactive
            if(message.control==43){
                if(message.value==127){
                    audioReactiveControlSwitch=1;
                }
                if(message.value==0){
                    audioReactiveControlSwitch=0;
                }
            }
            if(message.control==44){
                if(message.value==127){
                    audioReactiveControlSwitch=2;
                }
                if(message.value==0){
                    audioReactiveControlSwitch=0;
                }
            }
            if(message.control==42){
                if(message.value==127){
                    audioReactiveControlSwitch=3;
                }
                if(message.value==0){
                    audioReactiveControlSwitch=0;
                }
            }
            if(message.control==55){
                if(message.value==127){
                    p_lock_switch=1;
                    for(int i=0;i<p_lock_number;i++){
                        p_lock_smoothed[i]=0;
                        for(int j=0;j<p_lock_size;j++){
                            p_lock[i][j]=p_lock[i][p_lock_increment];
                        }
                    }
                }
                if(message.value==0){
                    p_lock_switch=0;
                }
            }
            if(message.control==32){
                if(message.value==127){
                    x_2=TRUE;
                }
                if(message.value==0){
                    x_2=FALSE;
                }
            }
            if(message.control==48){
                if(message.value==127){
                    x_5=TRUE;
                }
                if(message.value==0){
                    x_5=FALSE;
                }
            }
            if(message.control==64){
                if(message.value==127){
                    x_10=TRUE;
                }
                if(message.value==0){
                    x_10=FALSE;
                }
            }
            if(message.control==33){
                if(message.value==127){
                    y_2=TRUE;
                }
                if(message.value==0){
                    y_2=FALSE;
                }
            }
            if(message.control==49){
                if(message.value==127){
                    y_5=TRUE;
                }
                if(message.value==0){
                    y_5=FALSE;
                }
            }
            if(message.control==65){
                if(message.value==127){
                    y_10=TRUE;
                }
                if(message.value==0){
                    y_10=FALSE;
                }
            }
            if(message.control==34){
                if(message.value==127){
                    z_2=TRUE;
                }
                if(message.value==0){
                    z_2=FALSE;
                }
            }
            if(message.control==50){
                if(message.value==127){
                    z_5=TRUE;
                }
                if(message.value==0){
                    z_5=FALSE;
                }
            }
            if(message.control==66){
                if(message.value==127){
                    z_10=TRUE;
                }
                if(message.value==0){
                    z_10=FALSE;
                }
            }
            if(message.control==35){
                if(message.value==127){
                    r_2=TRUE;
                }
                if(message.value==0){
                    r_2=FALSE;
                }
            }
            if(message.control==51){
                if(message.value==127){
                    r_5=TRUE;
                }
                if(message.value==0){
                    r_5=FALSE;
                }
            }
            if(message.control==67){
                if(message.value==127){
                    r_10=TRUE;
                }
                if(message.value==0){
                    r_10=FALSE;
                }
            }
            if(message.control==36){
                if(message.value==127){
                    huexx_0=TRUE;
                }
                if(message.value==0){
                    huexx_0=FALSE;
                }
            }
            if(message.control==52){
                if(message.value==127){
                    huexx_1=TRUE;
                }
                if(message.value==0){
                    huexx_1=FALSE;
                }
            }
            if(message.control==68){
                if(message.value==127){
                    huexx_2=TRUE;
                }
                if(message.value==0){
                    huexx_2=FALSE;
                }
            }
            if(message.control==46){
                if(message.value==127){
                    toroidSwitch=TRUE;
                }
                if(message.value==0){
                    toroidSwitch=FALSE;
                }
            }
            if(message.control==61){
                if(message.value==127){
                    mirrorSwitch=TRUE;
                }
                if(message.value==0){
                    mirrorSwitch=FALSE;
                }
            }
            if(y_skew_switch==TRUE){
                y_skew+=.00001;
            }
            if(x_skew_switch==TRUE){
                x_skew+=.00001;
            }
            if(message.control==71){
                if(message.value==127){
                    wet_dry_switch=FALSE;
                }
                if(message.value==0){
                    wet_dry_switch=TRUE;
                }
            }
            if(message.control==37){
                if(message.value==127){
                    huexy_0=TRUE;
                }
                if(message.value==0){
                    huexy_0=FALSE;
                }
            }
            if(message.control==53){
                if(message.value==127){
                    huexy_1=TRUE;
                }
                if(message.value==0){
                    huexy_1=FALSE;
                }
            }
            if(message.control==69){
                if(message.value==127){
                    huexy_2=TRUE;
                }
                if(message.value==0){
                    huexy_2=FALSE;
                }
            }
            if(message.control==38){
                if(message.value==127){
                    huexz_0=TRUE;
                }
                if(message.value==0){
                    huexz_0=FALSE;
                }
            }
            if(message.control==54){
                if(message.value==127){
                    huexz_1=TRUE;
                }
                if(message.value==0){
                    huexz_1=FALSE;
                }
            }
            if(message.control==70){
                if(message.value==127){
                    huexz_2=TRUE;
                }
                if(message.value==0){
                    huexz_2=FALSE;
                }
            }
            if(message.control==60){
                if(message.value==127){
                    lumakeyInvertSwitch=TRUE;
                }
                if(message.value==0){
                    lumakeyInvertSwitch=FALSE;
                }
            }
            //CONTINUOUS CONTROLS
            if(message.control==16){
                if(audioReactiveControlSwitch==0){
                    midiLowActiveFloat[0]=0;
                    midiMidActiveFloat[0]=0;
                    midiHighActiveFloat[0]=0;
                    if(p_lock_0_switch==1){
                        vmidiActiveFloat[0]=0;
                        if(abs(message.value/127.0f-p_lock[0][p_lock_increment])<CONTROL_THRESHOLD){
                            midiActiveFloat[0]=1;
                        }
                        if(midiActiveFloat[0]==1){
                            p_lock[0][p_lock_increment]=message.value/127.0f;
                        }
                    }
                    if(videoReactiveSwitch==1){
                        midiActiveFloat[0]=0;
                        if(abs(message.value/127.0f-vLumakeyValue)<CONTROL_THRESHOLD){
                            vmidiActiveFloat[0]=1;
                        }
                        if(vmidiActiveFloat[0]==1){
                            vLumakeyValue=message.value/127.0f;
                        }
                    }
                }
                if(audioReactiveControlSwitch!=0){
                    midiActiveFloat[0]=0;
                    vmidiActiveFloat[0]=0;
                }
                if(audioReactiveControlSwitch==1){
                    midiMidActiveFloat[0]=0;
                    midiHighActiveFloat[0]=0;
                    if(abs(message.value/127.0f-lowC1)<CONTROL_THRESHOLD){
                        midiLowActiveFloat[0]=1;
                    }
                    if(midiLowActiveFloat[0]==1){
                        lowC1=message.value/127.0f;
                    }
                }
                if(audioReactiveControlSwitch==2){
                    midiLowActiveFloat[0]=0;
                    midiHighActiveFloat[0]=0;
                    if(abs(message.value/127.0f-midC1)<CONTROL_THRESHOLD){
                        midiMidActiveFloat[0]=1;
                    }
                    if(midiMidActiveFloat[0]==1){
                        midC1=message.value/127.0f;
                    }
                }
                if(audioReactiveControlSwitch==3){
                    midiLowActiveFloat[0]=0;
                    midiMidActiveFloat[0]=0;
                    if(abs(message.value/127.0f-highC1)<CONTROL_THRESHOLD){
                        midiHighActiveFloat[0]=1;
                    }
                    if(midiHighActiveFloat[0]==1){
                        highC1=message.value/127.0f;
                    }
                }
            }
            if(message.control==17){
                if(audioReactiveControlSwitch==0){
                    midiLowActiveFloat[1]=0;
                    midiMidActiveFloat[1]=0;
                    midiHighActiveFloat[1]=0;
                    if(p_lock_0_switch==1){
                        vmidiActiveFloat[1]=0;
                        if(abs((message.value-MIDI_MAGIC)/MIDI_MAGIC-p_lock[1][p_lock_increment])<CONTROL_THRESHOLD){
                            midiActiveFloat[1]=1;
                        }
                        if(midiActiveFloat[1]==1){
                            p_lock[1][p_lock_increment]=(message.value-MIDI_MAGIC)/MIDI_MAGIC;
                        }
                    }
                    if(videoReactiveSwitch==1){
                        midiActiveFloat[1]=0;
                        if(abs((message.value-MIDI_MAGIC)/MIDI_MAGIC-vMix)<CONTROL_THRESHOLD){
                            vmidiActiveFloat[1]=1;
                        }
                        if(vmidiActiveFloat[1]==1){
                            vMix=(message.value-MIDI_MAGIC)/MIDI_MAGIC;
                        }
                    }
                }
                if(audioReactiveControlSwitch!=0){
                    midiActiveFloat[1]=0;
                    vmidiActiveFloat[1]=0;
                }
                if(audioReactiveControlSwitch==1){
                    midiMidActiveFloat[1]=0;
                    midiHighActiveFloat[1]=0;
                    if(abs((message.value-MIDI_MAGIC)/MIDI_MAGIC-lowC2)<CONTROL_THRESHOLD){
                        midiLowActiveFloat[1]=1;
                    }
                    if(midiLowActiveFloat[1]==1){
                        lowC2=(message.value-MIDI_MAGIC)/MIDI_MAGIC;
                    }
                }
                if(audioReactiveControlSwitch==2){
                    midiLowActiveFloat[1]=0;
                    midiHighActiveFloat[1]=0;
                    if(abs((message.value-MIDI_MAGIC)/MIDI_MAGIC-midC2)<CONTROL_THRESHOLD){
                        midiMidActiveFloat[1]=1;
                    }
                    if(midiMidActiveFloat[1]==1){
                        midC2=(message.value-MIDI_MAGIC)/MIDI_MAGIC;
                    }
                }
                if(audioReactiveControlSwitch==3){
                    midiLowActiveFloat[1]=0;
                    midiMidActiveFloat[1]=0;
                    if(abs((message.value-MIDI_MAGIC)/MIDI_MAGIC-highC2)<CONTROL_THRESHOLD){
                        midiHighActiveFloat[1]=1;
                    }
                    if(midiHighActiveFloat[1]==1){
                        highC2=(message.value-MIDI_MAGIC)/MIDI_MAGIC;
                    }
                }
            }

            //c3 maps to fb hue
            if(message.control==18){
                if(audioReactiveControlSwitch==0){
                    midiLowActiveFloat[2]=0;
                    midiMidActiveFloat[2]=0;
                    midiHighActiveFloat[2]=0;
                    if(p_lock_0_switch==1){
                        vmidiActiveFloat[2]=0;
                        if(abs((message.value-MIDI_MAGIC)/MIDI_MAGIC-p_lock[2][p_lock_increment])<CONTROL_THRESHOLD){
                            midiActiveFloat[2]=1;
                        }
                        if(midiActiveFloat[2]==1){
                            p_lock[2][p_lock_increment]=(message.value-MIDI_MAGIC)/MIDI_MAGIC;
                        }
                    }
                    if(videoReactiveSwitch==1){
                        midiActiveFloat[2]=0;
                        if(abs((message.value-MIDI_MAGIC)/MIDI_MAGIC-vHue)<CONTROL_THRESHOLD){
                            vmidiActiveFloat[2]=1;
                        }
                        if(vmidiActiveFloat[2]==1){
                            vHue=(message.value-MIDI_MAGIC)/MIDI_MAGIC;
                        }
                    }
                }
                if(audioReactiveControlSwitch!=0){
                    midiActiveFloat[2]=0;
                    vmidiActiveFloat[2]=0;
                }
                if(audioReactiveControlSwitch==1){
                    midiMidActiveFloat[2]=0;
                    midiHighActiveFloat[2]=0;
                    if(abs((message.value-MIDI_MAGIC)/MIDI_MAGIC-lowC3)<CONTROL_THRESHOLD){
                        midiLowActiveFloat[2]=1;
                    }
                    if(midiLowActiveFloat[2]==1){
                        lowC3=(message.value-MIDI_MAGIC)/MIDI_MAGIC;
                    }
                }
                if(audioReactiveControlSwitch==2){
                    midiLowActiveFloat[2]=0;
                    midiHighActiveFloat[2]=0;
                    if(abs((message.value-MIDI_MAGIC)/MIDI_MAGIC-midC3)<CONTROL_THRESHOLD){
                        midiMidActiveFloat[2]=1;
                    }
                    if(midiMidActiveFloat[2]==1){
                        midC3=(message.value-MIDI_MAGIC)/MIDI_MAGIC;
                    }
                }
                if(audioReactiveControlSwitch==3){
                    midiLowActiveFloat[2]=0;
                    midiMidActiveFloat[2]=0;
                    if(abs((message.value-MIDI_MAGIC)/MIDI_MAGIC-highC3)<CONTROL_THRESHOLD){
                        midiHighActiveFloat[2]=1;
                    }
                    if(midiHighActiveFloat[2]==1){
                        highC3=(message.value-MIDI_MAGIC)/MIDI_MAGIC;
                    }
                }
            }
            //c4 maps to fb satx
            if(message.control==19){
                if(audioReactiveControlSwitch==0){
                    midiLowActiveFloat[3]=0;
                    midiMidActiveFloat[3]=0;
                    midiHighActiveFloat[3]=0;
                    if(p_lock_0_switch==1){
                        vmidiActiveFloat[3]=0;
                        if(abs((message.value-MIDI_MAGIC)/MIDI_MAGIC-p_lock[3][p_lock_increment])<CONTROL_THRESHOLD){
                            midiActiveFloat[3]=1;
                        }
                        if(midiActiveFloat[3]==1){
                            p_lock[3][p_lock_increment]=(message.value-MIDI_MAGIC)/MIDI_MAGIC;
                        }
                    }
                    if(videoReactiveSwitch==1){
                        midiActiveFloat[3]=0;
                        if(abs((message.value-MIDI_MAGIC)/MIDI_MAGIC-vSaturation)<CONTROL_THRESHOLD){
                            vmidiActiveFloat[3]=1;
                        }
                        if(vmidiActiveFloat[3]==1){
                            vSaturation=(message.value-MIDI_MAGIC)/MIDI_MAGIC;
                        }
                    }
                }
                if(audioReactiveControlSwitch!=0){
                    midiActiveFloat[3]=0;
                    vmidiActiveFloat[3]=0;
                }
                if(audioReactiveControlSwitch==1){
                    midiMidActiveFloat[3]=0;
                    midiHighActiveFloat[3]=0;
                    if(abs((message.value-MIDI_MAGIC)/MIDI_MAGIC-lowC4)<CONTROL_THRESHOLD){
                        midiLowActiveFloat[3]=1;
                    }
                    if(midiLowActiveFloat[3]==1){
                        lowC4=(message.value-MIDI_MAGIC)/MIDI_MAGIC;
                    }
                }
                if(audioReactiveControlSwitch==2){
                    midiLowActiveFloat[3]=0;
                    midiHighActiveFloat[3]=0;
                    if(abs((message.value-MIDI_MAGIC)/MIDI_MAGIC-midC4)<CONTROL_THRESHOLD){
                        midiMidActiveFloat[3]=1;
                    }
                    if(midiMidActiveFloat[3]==1){
                        midC4=(message.value-MIDI_MAGIC)/MIDI_MAGIC;
                    }
                }
                if(audioReactiveControlSwitch==3){
                    midiLowActiveFloat[3]=0;
                    midiMidActiveFloat[3]=0;
                    if(abs((message.value-MIDI_MAGIC)/MIDI_MAGIC-highC4)<CONTROL_THRESHOLD){
                        midiHighActiveFloat[3]=1;
                    }
                    if(midiHighActiveFloat[3]==1){
                        highC4=(message.value-MIDI_MAGIC)/MIDI_MAGIC;
                    }
                }
            }
            //c5 maps to fb brightx
            if(message.control==20){
                if(audioReactiveControlSwitch==0){
                    midiLowActiveFloat[4]=0;
                    midiMidActiveFloat[4]=0;
                    midiHighActiveFloat[4]=0;
                    if(p_lock_0_switch==1){
                        vmidiActiveFloat[4]=0;
                        if(abs((message.value-MIDI_MAGIC)/MIDI_MAGIC-p_lock[4][p_lock_increment])<CONTROL_THRESHOLD){
                            midiActiveFloat[4]=1;
                        }
                        if(midiActiveFloat[4]==1){
                            p_lock[4][p_lock_increment]=(message.value-MIDI_MAGIC)/MIDI_MAGIC;
                        }
                    }
                    if(videoReactiveSwitch==1){
                        midiActiveFloat[4]=0;
                        if(abs((message.value-MIDI_MAGIC)/MIDI_MAGIC-vBright)<CONTROL_THRESHOLD){
                            vmidiActiveFloat[4]=1;
                        }
                        if(vmidiActiveFloat[4]==1){
                            vBright=(message.value-MIDI_MAGIC)/MIDI_MAGIC;
                        }
                    }
                }
                if(audioReactiveControlSwitch!=0){
                    midiActiveFloat[4]=0;
                    vmidiActiveFloat[4]=0;
                }
                if(audioReactiveControlSwitch==1){
                    midiMidActiveFloat[4]=0;
                    midiHighActiveFloat[4]=0;
                    if(abs((message.value-MIDI_MAGIC)/MIDI_MAGIC-lowC5)<CONTROL_THRESHOLD){
                        midiLowActiveFloat[4]=1;
                    }
                    if(midiLowActiveFloat[4]==1){
                        lowC5=(message.value-MIDI_MAGIC)/MIDI_MAGIC;
                    }
                }
                if(audioReactiveControlSwitch==2){
                    midiLowActiveFloat[4]=0;
                    midiHighActiveFloat[4]=0;
                    if(abs((message.value-MIDI_MAGIC)/MIDI_MAGIC-midC5)<CONTROL_THRESHOLD){
                        midiMidActiveFloat[4]=1;
                    }
                    if(midiMidActiveFloat[4]==1){
                        midC5=(message.value-MIDI_MAGIC)/MIDI_MAGIC;
                    }
                }
                if(audioReactiveControlSwitch==3){
                    midiLowActiveFloat[4]=0;
                    midiMidActiveFloat[4]=0;
                    if(abs((message.value-MIDI_MAGIC)/MIDI_MAGIC-highC5)<CONTROL_THRESHOLD){
                        midiHighActiveFloat[4]=1;
                    }
                    if(midiHighActiveFloat[4]==1){
                        highC5=(message.value-MIDI_MAGIC)/MIDI_MAGIC;
                    }
                }
            }
            //c6 maps to temporal filter
            if(message.control==21){
                if(audioReactiveControlSwitch==0){
                    midiLowActiveFloat[5]=0;
                    midiMidActiveFloat[5]=0;
                    midiHighActiveFloat[5]=0;
                    if(p_lock_0_switch==1){
                        vmidiActiveFloat[5]=0;
                        if(abs((message.value-MIDI_MAGIC)/MIDI_MAGIC-p_lock[5][p_lock_increment])<CONTROL_THRESHOLD){
                            midiActiveFloat[5]=1;
                        }
                        if(midiActiveFloat[5]==1){
                            p_lock[5][p_lock_increment]=(message.value-MIDI_MAGIC)/MIDI_MAGIC;
                        }
                    }
                    if(videoReactiveSwitch==1){
                        midiActiveFloat[5]=0;
                        if(abs((message.value-MIDI_MAGIC)/MIDI_MAGIC-vTemporalFilterMix)<CONTROL_THRESHOLD){
                            vmidiActiveFloat[5]=1;
                        }
                        if(vmidiActiveFloat[5]==1){
                            vTemporalFilterMix =(message.value-MIDI_MAGIC)/MIDI_MAGIC;
                        }
                    }
                }
                if(audioReactiveControlSwitch!=0){
                    midiActiveFloat[5]=0;
                    vmidiActiveFloat[5]=0;
                }
                if(audioReactiveControlSwitch==1){
                    midiMidActiveFloat[5]=0;
                    midiHighActiveFloat[5]=0;
                    if(abs((message.value-MIDI_MAGIC)/MIDI_MAGIC-lowC6)<CONTROL_THRESHOLD){
                        midiLowActiveFloat[5]=1;
                    }
                    if(midiLowActiveFloat[5]==1){
                        lowC6=(message.value-MIDI_MAGIC)/MIDI_MAGIC;
                    }
                }
                if(audioReactiveControlSwitch==2){
                    midiLowActiveFloat[5]=0;
                    midiHighActiveFloat[5]=0;
                    if(abs((message.value-MIDI_MAGIC)/MIDI_MAGIC-midC6)<CONTROL_THRESHOLD){
                        midiMidActiveFloat[5]=1;
                    }
                    if(midiMidActiveFloat[5]==1){
                        midC6=(message.value-MIDI_MAGIC)/MIDI_MAGIC;
                    }
                }
                if(audioReactiveControlSwitch==3){
                    midiLowActiveFloat[5]=0;
                    midiMidActiveFloat[5]=0;
                    if(abs((message.value-MIDI_MAGIC)/MIDI_MAGIC-highC6)<CONTROL_THRESHOLD){
                        midiHighActiveFloat[5]=1;
                    }
                    if(midiHighActiveFloat[5]==1){
                        highC6=(message.value-MIDI_MAGIC)/MIDI_MAGIC;
                    }
                }
            }
            //c7 maps to temporal filter resonance
            if(message.control==22){
                if(audioReactiveControlSwitch==0){
                    midiLowActiveFloat[6]=0;
                    midiMidActiveFloat[6]=0;
                    midiHighActiveFloat[6]=0;
                    if(p_lock_0_switch==1){
                        vmidiActiveFloat[6]=0;
                        if(abs(message.value/127.0f-p_lock[6][p_lock_increment])<CONTROL_THRESHOLD){
                            midiActiveFloat[6]=1;
                        }
                        if(midiActiveFloat[6]==1){
                            p_lock[6][p_lock_increment]=message.value/127.0f;
                        }
                    }
                    if(videoReactiveSwitch==1){
                        midiActiveFloat[6]=0;
                        if(abs(message.value/127.0f-vTemporalFilterResonance)<CONTROL_THRESHOLD){
                            vmidiActiveFloat[6]=1;
                        }
                        if(vmidiActiveFloat[6]==1){
                            vTemporalFilterResonance=message.value/127.0f;
                        }
                    }
                }
                if(audioReactiveControlSwitch!=0){
                    midiActiveFloat[6]=0;
                    vmidiActiveFloat[6]=0;
                }
                if(audioReactiveControlSwitch==1){
                    midiMidActiveFloat[6]=0;
                    midiHighActiveFloat[6]=0;
                    if(abs(message.value/127.0f-lowC7)<CONTROL_THRESHOLD){
                        midiLowActiveFloat[6]=1;
                    }
                    if(midiLowActiveFloat[6]==1){
                        lowC7=message.value/127.0f;
                    }
                }
                if(audioReactiveControlSwitch==2){
                    midiLowActiveFloat[6]=0;
                    midiHighActiveFloat[6]=0;
                    if(abs(message.value/127.0f-midC7)<CONTROL_THRESHOLD){
                        midiMidActiveFloat[6]=1;
                    }
                    if(midiMidActiveFloat[6]==1){
                        midC7=message.value/127.0f;
                    }
                }
                if(audioReactiveControlSwitch==3){
                    midiLowActiveFloat[6]=0;
                    midiMidActiveFloat[6]=0;
                    if(abs(message.value/127.0f-highC7)<CONTROL_THRESHOLD){
                        midiHighActiveFloat[6]=1;
                    }
                    if(midiHighActiveFloat[6]==1){
                        highC7=message.value/127.0f;
                    }
                }
            }
            //c8 maps to sharpen
            if(message.control==23){
                if(audioReactiveControlSwitch==0){
                    midiLowActiveFloat[7]=0;
                    midiMidActiveFloat[7]=0;
                    midiHighActiveFloat[7]=0;
                    if(p_lock_0_switch==1){
                        vmidiActiveFloat[7]=0;
                        if(abs(message.value/127.0f-p_lock[7][p_lock_increment])<CONTROL_THRESHOLD){
                            midiActiveFloat[7]=1;
                        }
                        if(midiActiveFloat[7]==1){
                            p_lock[7][p_lock_increment]=message.value/127.0f;
                        }
                    }
                    if(videoReactiveSwitch==1){
                        midiActiveFloat[7]=0;
                        if(abs(message.value/127.0f-vSharpenAmount)<CONTROL_THRESHOLD){
                            vmidiActiveFloat[7]=1;
                        }
                        if(vmidiActiveFloat[7]==1){
                            vSharpenAmount=message.value/127.0f;
                        }
                    }
                }
                if(audioReactiveControlSwitch!=0){
                    midiActiveFloat[7]=0;
                    vmidiActiveFloat[7]=0;
                }
                if(audioReactiveControlSwitch==1){
                    midiMidActiveFloat[7]=0;
                    midiHighActiveFloat[7]=0;
                    if(abs(message.value/127.0f-lowC8)<CONTROL_THRESHOLD){
                        midiLowActiveFloat[7]=1;
                    }
                    if(midiLowActiveFloat[7]==1){
                        lowC8=message.value/127.0f;
                    }
                }
                if(audioReactiveControlSwitch==2){
                    midiLowActiveFloat[7]=0;
                    midiHighActiveFloat[7]=0;
                    if(abs(message.value/127.0f-midC8)<CONTROL_THRESHOLD){
                        midiMidActiveFloat[7]=1;
                    }
                    if(midiMidActiveFloat[7]==1){
                        midC8=message.value/127.0f;
                    }
                }
                if(audioReactiveControlSwitch==3){
                    midiLowActiveFloat[7]=0;
                    midiMidActiveFloat[7]=0;
                    if(abs(message.value/127.0f-highC8)<CONTROL_THRESHOLD){
                        midiHighActiveFloat[7]=1;
                    }
                    if(midiHighActiveFloat[7]==1){
                        highC8=message.value/127.0f;
                    }
                }
            }
            //c9 maps to fb x displace
            if(message.control==120){
                if(audioReactiveControlSwitch==0){
                    p_lock[8][p_lock_increment]=(message.value-63)/63.0f;

                    if(x_2==TRUE){
                        p_lock[8][p_lock_increment]=2.0*(message.value-63.0)/63.0f;
                    }

                    if(x_5==TRUE){
                        p_lock[8][p_lock_increment]=5.0*(message.value-63.0)/63.0f;
                    }
                    if(x_10==TRUE){
                        p_lock[8][p_lock_increment]=10.0*(message.value-63.0)/63.0f;
                    }
                }

                if(audioReactiveControlSwitch==1){
                    lowC9=(message.value-63)/63.0f;

                    if(x_2==TRUE){
                        lowC9=2.0*(message.value-63.0)/63.0f;
                    }

                    if(x_5==TRUE){
                        lowC9=5.0*(message.value-63.0)/63.0f;
                    }
                    if(x_10==TRUE){
                        lowC9=10.0*(message.value-63.0)/63.0f;
                    }
                }

                if(audioReactiveControlSwitch==2){
                    midC9=(message.value-63)/63.0f;

                    if(x_2==TRUE){
                        midC9=2.0*(message.value-63.0)/63.0f;
                    }

                    if(x_5==TRUE){
                        midC9=5.0*(message.value-63.0)/63.0f;
                    }
                    if(x_10==TRUE){
                        midC9=10.0*(message.value-63.0)/63.0f;
                    }
                }

                if(audioReactiveControlSwitch==3){
                    highC9=(message.value-63)/63.0f;

                    if(x_2==TRUE){
                        highC9=2.0*(message.value-63.0)/63.0f;
                    }

                    if(x_5==TRUE){
                        highC9=5.0*(message.value-63.0)/63.0f;
                    }
                    if(x_10==TRUE){
                        highC9=10.0*(message.value-63.0)/63.0f;
                    }
                }

            }

            //c10 maps to fb y displace
            if(message.control==121){
                if(audioReactiveControlSwitch==0){
                    p_lock[9][p_lock_increment]=(message.value-63)/63.0f;

                    if(y_2==TRUE){
                        p_lock[9][p_lock_increment]=2.0*(message.value-63.0)/63.0f;
                    }

                    if(y_5==TRUE){
                        p_lock[9][p_lock_increment]=5.0*(message.value-63.0)/63.0f;
                    }
                    if(y_10==TRUE){
                        p_lock[9][p_lock_increment]=10.0*(message.value-63.0)/63.0f;
                    }
                }

                if(audioReactiveControlSwitch==1){
                    lowC10=(message.value-63)/63.0f;

                    if(y_2==TRUE){
                        lowC10=2.0*(message.value-63.0)/63.0f;
                    }

                    if(y_5==TRUE){
                        lowC10=5.0*(message.value-63.0)/63.0f;
                    }
                    if(y_10==TRUE){
                        lowC10=10.0*(message.value-63.0)/63.0f;
                    }
                }

                if(audioReactiveControlSwitch==2){
                    midC10=(message.value-63)/63.0f;

                    if(y_2==TRUE){
                        midC10=2.0*(message.value-63.0)/63.0f;
                    }

                    if(y_5==TRUE){
                        midC10=5.0*(message.value-63.0)/63.0f;
                    }
                    if(y_10==TRUE){
                        midC10=10.0*(message.value-63.0)/63.0f;
                    }
                }

                if(audioReactiveControlSwitch==3){
                    highC10=(message.value-63)/63.0f;

                    if(y_2==TRUE){
                        highC10=2.0*(message.value-63.0)/63.0f;
                    }

                    if(y_5==TRUE){
                        highC10=5.0*(message.value-63.0)/63.0f;
                    }
                    if(y_10==TRUE){
                        highC10=10.0*(message.value-63.0)/63.0f;
                    }
                }

            }


            if(message.control==122){
                if(audioReactiveControlSwitch==0){
                    p_lock[10][p_lock_increment]=(message.value-63.0)/63.0f;

                    if(z_2==TRUE){
                        p_lock[10][p_lock_increment]=2.0*(message.value-63.0)/63.0f;
                    }

                    if(z_5==TRUE){
                        p_lock[10][p_lock_increment]=5.0*(message.value-63.0)/63.0f;
                    }
                    if(z_10==TRUE){
                        p_lock[10][p_lock_increment]=10.0*(message.value-63.0)/63.0f;
                    }
                }

                if(audioReactiveControlSwitch==1){
                    lowC11=(message.value-63.0)/63.0f;

                    if(z_2==TRUE){
                        lowC11=2.0*(message.value-63.0)/63.0f;
                    }

                    if(z_5==TRUE){
                        lowC11=5.0*(message.value-63.0)/63.0f;
                    }
                    if(z_10==TRUE){
                        lowC11=10.0*(message.value-63.0)/63.0f;
                    }
                }

                if(audioReactiveControlSwitch==2){
                    midC11=(message.value-63.0)/63.0f;

                    if(z_2==TRUE){
                        midC11=2.0*(message.value-63.0)/63.0f;
                    }

                    if(z_5==TRUE){
                        midC11=5.0*(message.value-63.0)/63.0f;
                    }
                    if(z_10==TRUE){
                        midC11=10.0*(message.value-63.0)/63.0f;
                    }
                }

                if(audioReactiveControlSwitch==3){
                    highC11=(message.value-63.0)/63.0f;

                    if(z_2==TRUE){
                        highC11=2.0*(message.value-63.0)/63.0f;
                    }

                    if(z_5==TRUE){
                        highC11=5.0*(message.value-63.0)/63.0f;
                    }
                    if(z_10==TRUE){
                        highC11=10.0*(message.value-63.0)/63.0f;
                    }
                }
            }

            if(message.control==123){
                if(audioReactiveControlSwitch==0){
                    p_lock[11][p_lock_increment]=(message.value-63)/63.0f;

                    if(r_2==TRUE){
                        p_lock[11][p_lock_increment]=2*(message.value-63.0)/63.0f;
                    }

                    if(r_5==TRUE){
                        p_lock[11][p_lock_increment]=4*(message.value-63.0)/63.0f;
                    }
                    if(r_10==TRUE){
                        p_lock[11][p_lock_increment]=8*(message.value-63.0)/63.0f;
                    }
                }

                if(audioReactiveControlSwitch==1){
                    lowC12=(message.value-63)/63.0f;

                    if(r_2==TRUE){
                        lowC12=2*(message.value-63.0)/63.0f;
                    }

                    if(r_5==TRUE){
                        lowC12=4*(message.value-63.0)/63.0f;
                    }
                    if(r_10==TRUE){
                        lowC12=8*(message.value-63.0)/63.0f;
                    }
                }

                if(audioReactiveControlSwitch==2){
                    midC12=(message.value-63)/63.0f;

                    if(r_2==TRUE){
                        midC12=2*(message.value-63.0)/63.0f;
                    }

                    if(r_5==TRUE){
                        midC12=4*(message.value-63.0)/63.0f;
                    }
                    if(r_10==TRUE){
                        midC12=8*(message.value-63.0)/63.0f;
                    }
                }

                if(audioReactiveControlSwitch==3){
                    highC12=(message.value-63)/63.0f;

                    if(r_2==TRUE){
                        highC12=2*(message.value-63.0)/63.0f;
                    }

                    if(r_5==TRUE){
                        highC12=4*(message.value-63.0)/63.0f;
                    }
                    if(r_10==TRUE){
                        highC12=8*(message.value-63.0)/63.0f;
                    }
                }
            }


            if(message.control==124){
                if(audioReactiveControlSwitch==0){
                    p_lock[12][p_lock_increment]=(message.value)/32.0f;

                    if(huexx_0==TRUE){
                        p_lock[12][p_lock_increment]=message.value/64.0f;
                    }

                    if(huexx_1==TRUE){
                        p_lock[12][p_lock_increment]=message.value/96.0f;
                    }
                    if(huexx_2==TRUE){
                        p_lock[12][p_lock_increment]=message.value/127.0f;
                    }
                }

                if(audioReactiveControlSwitch==1){
                    lowC13=(message.value)/32.0f;

                    if(huexx_0==TRUE){
                        lowC13=message.value/64.0f;
                    }

                    if(huexx_1==TRUE){
                        lowC13=message.value/96.0f;
                    }
                    if(huexx_2==TRUE){
                        lowC13=message.value/127.0f;
                    }
                }

                if(audioReactiveControlSwitch==2){
                    midC13=(message.value)/32.0f;

                    if(huexx_0==TRUE){
                        midC13=message.value/64.0f;
                    }

                    if(huexx_1==TRUE){
                        midC13=message.value/96.0f;
                    }
                    if(huexx_2==TRUE){
                        midC13=message.value/127.0f;
                    }
                }

                if(audioReactiveControlSwitch==3){
                    highC13=(message.value)/32.0f;

                    if(huexx_0==TRUE){
                        highC13=message.value/64.0f;
                    }

                    if(huexx_1==TRUE){
                        highC13=message.value/96.0f;
                    }
                    if(huexx_2==TRUE){
                        highC13=message.value/127.0f;
                    }
                }
            }
            if(message.control==125){
                if(audioReactiveControlSwitch==0){
                    p_lock[13][p_lock_increment]=(message.value-63.0)/63.0f;

                    if(huexy_0==TRUE){
                        p_lock[13][p_lock_increment]=2*(message.value-63.0)/63.0f;
                    }

                    if(huexy_1==TRUE){
                        p_lock[13][p_lock_increment]=4*(message.value-63.0)/63.0f;
                    }
                    if(huexy_2==TRUE){
                        p_lock[13][p_lock_increment]=8*(message.value-63.0)/63.0f;
                    }
                }

                if(audioReactiveControlSwitch==1){
                    lowC14=(message.value-63.0)/63.0f;

                    if(huexy_0==TRUE){
                        lowC14=2*(message.value-63.0)/63.0f;
                    }

                    if(huexy_1==TRUE){
                        lowC14=4*(message.value-63.0)/63.0f;
                    }
                    if(huexy_2==TRUE){
                        lowC14=8*(message.value-63.0)/63.0f;
                    }
                }

                if(audioReactiveControlSwitch==2){
                    midC14=(message.value-63.0)/63.0f;

                    if(huexy_0==TRUE){
                        midC14=2*(message.value-63.0)/63.0f;
                    }

                    if(huexy_1==TRUE){
                        midC14=4*(message.value-63.0)/63.0f;
                    }
                    if(huexy_2==TRUE){
                        midC14=8*(message.value-63.0)/63.0f;
                    }
                }

                if(audioReactiveControlSwitch==3){
                    highC14=(message.value-63.0)/63.0f;

                    if(huexy_0==TRUE){
                        highC14=2*(message.value-63.0)/63.0f;
                    }

                    if(huexy_1==TRUE){
                        highC14=4*(message.value-63.0)/63.0f;
                    }
                    if(huexy_2==TRUE){
                        highC14=8*(message.value-63.0)/63.0f;
                    }
                }
            }
            if(message.control==126){
                if(audioReactiveControlSwitch==0){
                    p_lock[14][p_lock_increment]=(message.value-63.0)/63.0f;

                    if(huexz_0==TRUE){
                        p_lock[14][p_lock_increment]=2*(message.value-63.0)/63.0f;
                    }

                    if(huexz_1==TRUE){
                        p_lock[14][p_lock_increment]=4*(message.value-63.0)/63.0f;
                    }
                    if(huexz_2==TRUE){
                        p_lock[14][p_lock_increment]=8*(message.value-63.0)/63.0f;
                    }
                }

                if(audioReactiveControlSwitch==1){
                    lowC15=(message.value-63.0)/63.0f;

                    if(huexz_0==TRUE){
                        lowC15=2*(message.value-63.0)/63.0f;
                    }

                    if(huexz_1==TRUE){
                        lowC15=4*(message.value-63.0)/63.0f;
                    }
                    if(huexz_2==TRUE){
                        lowC15=8*(message.value-63.0)/63.0f;
                    }
                }

                if(audioReactiveControlSwitch==2){
                    midC15=(message.value-63.0)/63.0f;

                    if(huexz_0==TRUE){
                        midC15=2*(message.value-63.0)/63.0f;
                    }

                    if(huexz_1==TRUE){
                        midC15=4*(message.value-63.0)/63.0f;
                    }
                    if(huexz_2==TRUE){
                        midC15=8*(message.value-63.0)/63.0f;
                    }
                }

                if(audioReactiveControlSwitch==3){
                    highC15=(message.value-63.0)/63.0f;

                    if(huexz_0==TRUE){
                        highC15=2*(message.value-63.0)/63.0f;
                    }

                    if(huexz_1==TRUE){
                        highC15=4*(message.value-63.0)/63.0f;
                    }
                    if(huexz_2==TRUE){
                        highC15=8*(message.value-63.0)/63.0f;
                    }
                }
            }
            if(message.control==127){
                if(audioReactiveControlSwitch==0){
                    p_lock[15][p_lock_increment]=(message.value)/127.0;
                }

                if(audioReactiveControlSwitch==1){
                    lowC16=(message.value)/127.0;
                }

                if(audioReactiveControlSwitch==2){
                    midC16=(message.value)/127.0;
                }

                if(audioReactiveControlSwitch==3){
                    highC16=(message.value)/127.0;
                }
            }




            //gots to remap these to 60-61
            //cc43 maps to brightInvert
            if(message.control==60){
                if(message.value==127){
                    brightInvert=TRUE;
                }

                if(message.value==0){
                    brightInvert=FALSE;
                }
            }

            //cc44 maps to saturationInvert
            if(message.control==61){
                if(message.value==127){
                    saturationInvert=TRUE;
                }

                if(message.value==0){
                    saturationInvert=FALSE;
                }
            }

            if(message.control==62){
                if(message.value==127){
                    //hueInvert=TRUE;
                    hdmi_aspect_ratio_switch=TRUE;
                }

                if(message.value==0){
                    //hueInvert=FALSE;
                    hdmi_aspect_ratio_switch=FALSE;
                }
            }


            //cc41 maps to horizontalMirror
            if(message.control==41){
                if(message.value==127){
                    horizontalMirror=TRUE;
                }

                if(message.value==0){
                    horizontalMirror=FALSE;
                }
            }
            //cc45 maps to horizontalMirror
            if(message.control==45){
                if(message.value==127){
                    verticalMirror=TRUE;
                }

                if(message.value==0){
                    verticalMirror=FALSE;
                }
            }
            if(message.control==59){

                x_skew=y_skew=0.0;
                lowC1=lowC2=lowC3=lowC4=lowC5=lowC6=lowC7=lowC8=lowC9=lowC10=lowC11=lowC12=lowC13=lowC14=lowC15=lowC16=0.0f;
                midC1=midC2=midC3=midC4=midC5=midC6=midC7=midC8=midC9=midC10=midC11=midC12=midC13=midC14=midC15=midC16=0.0f;
                highC1=highC2=highC3=highC4=highC5=highC6=highC7=highC8=highC9=highC10=highC11=highC12=highC13=highC14=highC15=highC16=0.0f;
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
}
