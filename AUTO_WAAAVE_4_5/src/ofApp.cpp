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
#include "ofxJSON.h"
#include <fstream>
#include <sstream>

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
    //ofSetVerticalSync(true);
    ofSetFrameRate(30);
    ofBackground(0);
    //ofToggleFullscreen();
    ofHideCursor();
    //omx_settings();
    inputSetup();
    midiSetup();
    loadMidiMapping(); // Add this line
    loadKeyMapping(); // Add this line
    fbDeclareAndAllocate();
    shader_mixer.load("shadersES2/shader_mixer");
    shaderSharpen.load("shadersES2/shaderSharpen");
    //fft biz
    fft.setup();
    fft.setNormalize(false);
    //fft.setVolumeRange(1.0f);
    
    p_lockClear();
    midiLatchClear();
}
//-------------------------------------------------------------
void ofApp::midiLatchClear(){
    for(int i=0;i<17;i++){
        vmidiActiveFloat[i]=0;
        midiActiveFloat[i]=0;
        midiLowActiveFloat[i]=0;
        midiMidActiveFloat[i]=0;
        midiHighActiveFloat[i]=0;
    }
}
//--------------------------------------------------------------
void ofApp::update() {
    
    fft.update();
    inputUpdate();
    midibiz();
    p_lockUpdate();
    
    if(fft.getLowVal()>my_normalize){
        my_normalize=fft.getLowVal();
    }
}

//--------------------------------------------------------------
void ofApp::draw() {
    
    
    //the range changes depending on what yr using for sound input.  just usb cams have shitter ranges
    //so 50 works
    //float range=200;
    
    fftAssignValues();
    
    parametersAssign();
    
    framebuffer0.begin();
    shader_mixer.begin();
    
    
    //videoGrabber.getTextureReference().draw(0, 0, 320, 640);
    if(scaleswitch==0){
        if(inputswitch==0){
            //videoGrabber.draw(0,0,320,240);
        }
        if(inputswitch==1){
            cam1.draw(0,0,320,240);
        }
    }
    if(scaleswitch==1){
        if(inputswitch==0){
            //videoGrabber.draw(0,0);
        }
        if(inputswitch==1){
            if(hdmi_aspect_ratio_switch==0){
                cam1.draw(0,0,640,480);
            }
            if(hdmi_aspect_ratio_switch==1){
                aspect_fix_fbo.draw(0,0,640,480);
            }
        }
    }
    /*
     //textures
     shader_mixer.setUniformTexture("fb0", pastFrames[d_delay].getTexture(),1);
     
     if(wet_dry_switch==1){
     shader_mixer.setUniformTexture("fb1", pastFrames[(abs(framedelayoffset-framebufferLength)-1)%framebufferLength].getTexture(),2);
     }
     if(wet_dry_switch==0){
     shader_mixer.setUniformTexture("fb1", dry_framebuffer.getTexture(),2);
     }
     //continuous variables
     shader_mixer.setUniform1f("fb0_lumakey_value",d_lumakey_value);
     shader_mixer.setUniform1f("fb0_mix",d_mix);
     shader_mixer.setUniform3f("fb0_hsbx",ofVec3f(d_hue,d_sat,d_bright));
     shader_mixer.setUniform1f("temporalFilterMix",d_temporalFilterMix);
     shader_mixer.setUniform1f("fb1_brightx",d_temporalFilterResonance );
     shader_mixer.setUniform1f("cam1_brightx",d_cam1_x);
     shader_mixer.setUniform1f("fb0_xdisplace",d_x);
     shader_mixer.setUniform1f("fb0_ydisplace",d_y);
     shader_mixer.setUniform1f("fb0_zdisplace",d_z);
     shader_mixer.setUniform1f("fb0_rotate",d_rotate);
     shader_mixer.setUniform3f("fb0_huex",ofVec3f(d_huex_mod,d_huex_off,d_huex_lfo));
     
     if(wet_dry_switch==1){
     shader_mixer.setUniform1i("toroidSwitch",toroidSwitch);
     shader_mixer.setUniform1i("mirrorSwitch",0);
     }
     
     if(wet_dry_switch==0){
     shader_mixer.setUniform1i("toroidSwitch",0);
     shader_mixer.setUniform1i("mirrorSwitch",toroidSwitch);
     }
     
     
     shader_mixer.setUniform1i("brightInvert",brightInvert);
     shader_mixer.setUniform1i("hueInvert",hueInvert);
     shader_mixer.setUniform1i("saturationInvert",saturationInvert);
     
     shader_mixer.setUniform1i("horizontalMirror",horizontalMirror);
     shader_mixer.setUniform1i("verticalMirror",verticalMirror);
     
     
     shader_mixer.setUniform1i("lumakeyInvertSwitch",lumakeyInvertSwitch);
     */
    //send the textures
    shader_mixer.setUniformTexture("fb", pastFrames[d_delay].getTexture(),1);
    if(wet_dry_switch==1){
        shader_mixer.setUniformTexture("temporalFilter", pastFrames[(abs(framedelayoffset-framebufferLength)-1)%framebufferLength].getTexture(),2);
    }
    if(wet_dry_switch==0){
        shader_mixer.setUniformTexture("temporalFilter", dry_framebuffer.getTexture(),2);
    }
    //send the continuous variables
    shader_mixer.setUniform1f("lumakey",d_lumakey_value);
    shader_mixer.setUniform1f("fbMix",d_mix);
    shader_mixer.setUniform1f("fbHue",d_hue);
    shader_mixer.setUniform1f("fbSaturation",d_sat);
    shader_mixer.setUniform1f("fbBright",d_bright);
    shader_mixer.setUniform1f("temporalFilterMix",d_temporalFilterMix);
    shader_mixer.setUniform1f("temporalFilterResonance",d_temporalFilterResonance );
    shader_mixer.setUniform1f("fbXDisplace",d_x);
    shader_mixer.setUniform1f("fbYDisplace",d_y);
    shader_mixer.setUniform1f("fbZDisplace",d_z);
    shader_mixer.setUniform1f("fbRotate",d_rotate);
    shader_mixer.setUniform3f("fb_huex",ofVec3f(d_huex_mod,d_huex_off,d_huex_lfo));
    shader_mixer.setUniform1f("fbHuexMod",d_huex_mod);
    shader_mixer.setUniform1f("fbHuexOff",d_huex_off);
    shader_mixer.setUniform1f("fbHuexLfo",d_huex_lfo);
    
    //send the switches
    shader_mixer.setUniform1i("toroidSwitch",toroidSwitch);
    shader_mixer.setUniform1i("mirrorSwitch",mirrorSwitch);
    shader_mixer.setUniform1i("brightInvert",brightInvert);
    shader_mixer.setUniform1i("hueInvert",hueInvert);
    shader_mixer.setUniform1i("saturationInvert",saturationInvert);
    shader_mixer.setUniform1i("horizontalMirror",horizontalMirror);
    shader_mixer.setUniform1i("verticalMirror",verticalMirror);
    shader_mixer.setUniform1i("lumakeyInvertSwitch",lumakeyInvertSwitch);
    
    //send the videoreactive controls
    shader_mixer.setUniform1f("vLumakey",vLumakeyValue*c_lumakey_value);
    shader_mixer.setUniform1f("vMix",vMix*c_mix);
    shader_mixer.setUniform1f("vHue",2.0f*vHue*c_hue);
    shader_mixer.setUniform1f("vSat",2.0f*vSaturation*c_sat);
    shader_mixer.setUniform1f("vBright",2.0f*vBright*c_bright);
    shader_mixer.setUniform1f("vtemporalFilterMix",vTemporalFilterMix*c_temporalFilterMix);
    shader_mixer.setUniform1f("vFb1X",vTemporalFilterResonance);
    shader_mixer.setUniform1f("vX",.01*vX);
    shader_mixer.setUniform1f("vY",.01*vY);
    shader_mixer.setUniform1f("vZ",vZ*c_z);
    shader_mixer.setUniform1f("vRotate",vRotate*c_rotate);
    shader_mixer.setUniform1f("vHuexMod",1.0f-vHuexMod);
    shader_mixer.setUniform1f("vHuexOff",vHuexOff*c_huex_off);
    shader_mixer.setUniform1f("vHuexLfo",vHuexLfo*c_huex_lfo);
    shader_mixer.end();
    framebuffer0.end();
    
    /**sharpening mode**/
    sharpenFramebuffer.begin();
    shaderSharpen.begin();
    framebuffer0.draw(0,0);
    shaderSharpen.setUniform1f("sharpenAmount",d_sharpenAmount);
    shaderSharpen.setUniform1f("vSharpenAmount",vSharpenAmount*c_sharpenAmount);
    shaderSharpen.end();
    sharpenFramebuffer.end();
    
    //framebuffer0.draw(0,0,ofGetWidth(),ofGetHeight());
    sharpenFramebuffer.draw(0,0,ofGetWidth(),ofGetHeight());
    
    pastFrames[abs(framebufferLength-framedelayoffset)-1].begin(); //eeettt
    if(wet_dry_switch==0){
        if(inputswitch==0){
            //videoGrabber.draw(0,0,framebuffer0.getWidth(),framebuffer0.getHeight());
        }
        if(inputswitch==1){
            if(hdmi_aspect_ratio_switch==0){
                cam1.draw(0,0,640,480);
            }
            if(hdmi_aspect_ratio_switch==1){
                aspect_fix_fbo.draw(0,0,640,480);
            }
        }
        
        dry_framebuffer.begin();
        //framebuffer0.draw(0,0);
        sharpenFramebuffer.draw(0,0);
        dry_framebuffer.end();
        
    }//endifwetdry0
    
    if(wet_dry_switch==1){
        //framebuffer0.draw(0,0);
        sharpenFramebuffer.draw(0,0);
    }//endifwetdry1
    
    pastFrames[abs(framebufferLength-framedelayoffset)-1].end(); //eeettt
    incIndex();
    
    //p_lock biz
    if(p_lock_switch==1){
        p_lock_increment++;
        p_lock_increment=p_lock_increment%p_lock_size;
    }
    
    //ofDrawBitmapString("fps =" + ofToString(ofGetFrameRate())+"fft low =" + ofToString(fftLow)+"fft low_smoothed =" + ofToString(low_value_smoothed)+"fft mid =" + ofToString(fftLow)+"fft mid_smoothed =" + ofToString(mid_value_smoothed), 10, ofGetHeight() - 5 );
    /*ofDrawBitmapString("fft high =" + ofToString(fftHigh,2)+
     "fft high_smoothed =" + ofToString(high_value_smoothed,2)+
     "fft mid =" + ofToString(fftMid,2)+
     "fft mid_smoothed =" + ofToString(mid_value_smoothed,2)+
     "fft low =" + ofToString(fftLow,2)+
     "fft low_smoothed =" + ofToString(low_value_smoothed,2)
     , 10, ofGetHeight() - 5 );
     */
    
    /*
     ofDrawBitmapString(
     "fft high =" + ofToString(high_value_smoothed,2)+
     "fft mid =" + ofToString(mid_value_smoothed,2)+
     "fft low =" + ofToString(low_value_smoothed,2)
     , 10, ofGetHeight() - 5 );
     */
    
    
    //i use this block of code to print out like useful information for debugging various things and/or just to keep the
    //framerate displayed to make sure i'm not losing any frames while testing out new features.  uncomment the ofDrawBitmap etc etc
    //to print the stuff out on screen
    ofSetColor(255);
    string msg="fps="+ofToString(ofGetFrameRate(),2)+" clear switch"+ofToString(clear_switch,5);//+" z="+ofToString(az,5);
    //ofDrawBitmapString(msg,10,10);
}

//--------------------------------------------------------------
void ofApp::exit() {
    
    // clean up
    midiIn.closePort();
    midiIn.removeListener(this);
}
//--------------------------------------------------------------

//--------------------------------------------------------------
void ofApp::fftAssignValues(){
    fftLow=fft.getLowVal();
    fftLow=ofClamp(fftLow,1.0f,range);
    fftLow=fftLow/range;
    low_value_smoothed=fftLow*smoothing_rate+(1.0f-smoothing_rate)*low_value_smoothed;
    
    fftMid=fft.getMidVal();
    fftMid=ofClamp(fftMid,1.0f,range);
    fftMid=fftMid/range;
    mid_value_smoothed=fftMid*smoothing_rate+(1.0f-smoothing_rate)*mid_value_smoothed;
    
    fftHigh=fft.getHighVal();
    fftHigh=ofClamp(fftHigh,1.0f,range);
    fftHigh=fftHigh/range;
    high_value_smoothed=fftHigh*smoothing_rate+(1.0f-smoothing_rate)*high_value_smoothed;
}
//-------------------------------------------------------------
void ofApp::parametersAssign(){
    if(wet_dry_switch==0){
        c_hue=1.0;
        c_sat=1.0;
        c_bright=1.0;
        c_x=.1;
        c_y=.1;
        c_z=.5;
    }
    
    d_delay=(abs(framedelayoffset-framebufferLength-fb0_delayamount-
                 int((p_lock_smoothed[15]+lowC16*low_value_smoothed+midC16*mid_value_smoothed+highC16*high_value_smoothed)*(framebufferLength-1.0))
                 )-1)%framebufferLength;
    d_lumakey_value=kk+c_lumakey_value*p_lock_smoothed[0]+4.0f*(
                                                                lowC1*low_value_smoothed+
                                                                midC1*mid_value_smoothed+
                                                                highC1*high_value_smoothed);
    d_mix=jm+c_mix*p_lock_smoothed[1]+4.0f*(
                                            lowC2*low_value_smoothed+
                                            midC2*mid_value_smoothed+
                                            highC2*high_value_smoothed);
    d_hue=fv*(1.0f+c_hue*p_lock_smoothed[2])+4.0f*(
                                                   lowC3*low_value_smoothed+
                                                   midC3*mid_value_smoothed+
                                                   highC3*high_value_smoothed);
    d_sat=gb*(1.0f+c_sat*p_lock_smoothed[3])+4.0f*(
                                                   lowC4*low_value_smoothed+
                                                   midC4*mid_value_smoothed+
                                                   highC4*high_value_smoothed);
    d_bright=hn*(1.0f+c_bright*p_lock_smoothed[4])+4.0f*(
                                                         lowC5*low_value_smoothed+
                                                         midC5*mid_value_smoothed+
                                                         highC5*high_value_smoothed);
    d_temporalFilterMix=op+c_temporalFilterMix*p_lock_smoothed[5]+4.0f*(
                                                                        lowC6*low_value_smoothed+
                                                                        midC6*mid_value_smoothed+
                                                                        highC6*high_value_smoothed);
    d_temporalFilterResonance=fb1_brightx+p_lock_smoothed[6]+4.0*(
                                                                  lowC7*low_value_smoothed+
                                                                  midC7*mid_value_smoothed+
                                                                  highC7*high_value_smoothed);
    d_sharpenAmount=ll+c_sharpenAmount*p_lock_smoothed[7]+8.0f*(
                                                                lowC8*low_value_smoothed+
                                                                midC8*mid_value_smoothed+
                                                                highC8*high_value_smoothed);
    d_x=sx+c_x*p_lock_smoothed[8]+4.0f*(
                                        c_x*(lowC9*low_value_smoothed+
                                             midC9*low_value_smoothed+
                                             highC9*high_value_smoothed));
    d_y=dc+c_y*p_lock_smoothed[9]+4.0f*(
                                        c_y*(lowC10*low_value_smoothed+
                                             midC10*mid_value_smoothed+
                                             highC10*high_value_smoothed));
    d_z=az*(1.0f+c_z*p_lock_smoothed[10])+4.0f*(
                                                c_z*(lowC11*low_value_smoothed+
                                                     midC11*mid_value_smoothed+
                                                     highC11*high_value_smoothed));
    d_rotate=qw+c_rotate*p_lock_smoothed[11]+4.0*(
                                                  c_rotate*(lowC12*low_value_smoothed+
                                                            midC12*mid_value_smoothed+
                                                            highC12*high_value_smoothed));
    d_huex_mod=er*(1.0f-p_lock_smoothed[12])+4.0f*(
                                                   lowC13*low_value_smoothed+
                                                   midC13*mid_value_smoothed+
                                                   highC13*high_value_smoothed);
    d_huex_off=ty+c_huex_off*p_lock_smoothed[13]+4.0f*(
                                                       c_huex_off*(lowC14*low_value_smoothed+
                                                                   midC14*mid_value_smoothed+
                                                                   highC14*high_value_smoothed));
    d_huex_lfo=ui+c_huex_lfo*p_lock_smoothed[14]+4.0f*(
                                                       c_huex_lfo*(lowC15*low_value_smoothed+
                                                                   midC15*mid_value_smoothed+
                                                                   highC15*high_value_smoothed));
}	
//--------------------------------------------------------------
void ofApp::newMidiMessage(ofxMidiMessage& msg) {
    // add the latest message to the message queue
    midiMessages.push_back(msg);
    // remove any old messages if we have too many
    while(midiMessages.size() > 2) {
        midiMessages.erase(midiMessages.begin());
    }
}
//------------------------------------------------------------
void ofApp::p_lockUpdate(){
    for(int i=0;i<p_lock_number;i++){
        p_lock_smoothed[i]=p_lock[i][p_lock_increment]*(1.0f-p_lock_smooth)+p_lock_smoothed[i]*p_lock_smooth;
        if(abs(p_lock_smoothed[i])<.05){p_lock_smoothed[i]=0;}
    }
}
//-------------------------------------------------------------
void ofApp::inputUpdate(){
    if(inputswitch==1){
        cam1.update();
        //corner crop and stretch to preserve hd aspect ratio
        if(hdmi_aspect_ratio_switch==1){
            aspect_fix_fbo.begin();
            cam1.draw(0,0,853,480);
            aspect_fix_fbo.end();
        }
    }
    if(inputswitch==0){
        //omx_updates();
    }
}
//------------------------------------------------------------
void ofApp::p_lockClear(){
    for(int i=0;i<p_lock_number;i++){
        for(int j=0;j<p_lock_size;j++){
            p_lock[i][j]=0;
        }
    }
}
//------------------------------------------------------------
void ofApp::midiSetup(){
    // print input ports to console
    midiIn.listInPorts();
    
    // open port by number (you may need to change this)
    midiIn.openPort(1);
    //midiIn.openPort("IAC Pure Data In");	// by name
    //midiIn.openVirtualPort("ofxMidiIn Input"); // open a virtual port
    
    // don't ignore sysex, timing, & active sense messages,
    // these are ignored by default
    midiIn.ignoreTypes(false, false, false);
    
    // add ofApp as a listener
    midiIn.addListener(this);
    
    // print received messages to the console
    midiIn.setVerbose(true);
}
//-------------------------------------------------------------
void ofApp::fbDeclareAndAllocate(){
    framebuffer0.allocate(width,height);
    framebuffer0.begin();
    ofClear(0,0,0,255);
    framebuffer0.end();
    
    aspect_fix_fbo.allocate(width,height);
    aspect_fix_fbo.begin();
    ofClear(0,0,0,255);
    aspect_fix_fbo.end();
    
    dry_framebuffer.allocate(width,height);
    dry_framebuffer.begin();
    ofClear(0,0,0,255);
    dry_framebuffer.end();
    
    sharpenFramebuffer.allocate(width,height);
    sharpenFramebuffer.begin();
    ofClear(0,0,0,255);
    sharpenFramebuffer.end();
    
    for(int i=0;i<framebufferLength;i++){
        pastFrames[i].allocate(width, height);
        pastFrames[i].begin();
        ofClear(0,0,0,255);
        pastFrames[i].end();
    }
}
//-------------------------------------------------------------
void ofApp::inputSetup(){
    //pass in the settings and it will start the camera
    if(inputswitch==0){
        //videoGrabber.setup(settings);
    }
    if(inputswitch==1){
        cam1.setDesiredFrameRate(30);
        cam1.initGrabber(width,height);
    }
}
//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	reloadKeyMappingIfChanged();

	auto clampInc = [](float &v, float inc, float min, float max) { v = ofClamp(v + inc, min, max); };
	auto inc = [](float &v, float inc) { v += inc; };
	auto dec = [](float &v, float dec) { v -= dec; };

	auto it = keyActionMap.find(key);
	if(it == keyActionMap.end()) return;
	const KeyAction& action = it->second;

	if(action.target == "fftNormalize") {
		fft.setNormalize(true);
	} else if(action.target == "fb0_delayamount_inc") {
		fb0_delayamount += 1;
	} else if(action.target == "fb0_delayamount_dec") {
		fb0_delayamount -= 1;
		if (fb0_delayamount < 0) {
			fb0_delayamount = framebufferLength - fb0_delayamount;
		}
	} else if(action.target == "op_inc") {
		inc(op, .01f);
	} else if(action.target == "op_dec") {
		dec(op, .01f);
	} else if(action.target == "az_inc") {
		inc(az, .0001f);
	} else if(action.target == "az_dec") {
		dec(az, .0001f);
	} else if(action.target == "sx_inc") {
		inc(sx, .0001f);
	} else if(action.target == "sx_dec") {
		dec(sx, .0001f);
	} else if(action.target == "dc_inc") {
		inc(dc, .0001f);
	} else if(action.target == "dc_dec") {
		dec(dc, .0001f);
	} else if(action.target == "fv_inc") {
		inc(fv, .001f);
	} else if(action.target == "fv_dec") {
		dec(fv, .001f);
	} else if(action.target == "gb_inc") {
		inc(gb, .001f);
	} else if(action.target == "gb_dec") {
		dec(gb, .001f);
	} else if(action.target == "hn_inc") {
		inc(hn, .001f);
	} else if(action.target == "hn_dec") {
		dec(hn, .001f);
	} else if(action.target == "jm_inc") {
		inc(jm, .01f);
	} else if(action.target == "jm_dec") {
		dec(jm, .01f);
	} else if(action.target == "kk_inc") {
		clampInc(kk, .01f, 0.0f, 1.0f);
	} else if(action.target == "kk_dec") {
		clampInc(kk, -.01f, 0.0f, 1.0f);
	} else if(action.target == "ll_inc") {
		inc(ll, .01f);
	} else if(action.target == "ll_dec") {
		dec(ll, .01f);
	} else if(action.target == "fb1_brightx_inc") {
		inc(fb1_brightx, .01f);
	} else if(action.target == "fb1_brightx_dec") {
		dec(fb1_brightx, .01f);
	} else if(action.target == "qw_inc") {
		inc(qw, .0001f);
	} else if(action.target == "qw_dec") {
		dec(qw, .0001f);
	} else if(action.target == "er_inc") {
		inc(er, .001f);
	} else if(action.target == "er_dec") {
		dec(er, .001f);
	} else if(action.target == "ty_inc") {
		inc(ty, .01f);
	} else if(action.target == "ty_dec") {
		dec(ty, .01f);
	} else if(action.target == "ui_inc") {
		inc(ui, .01f);
	} else if(action.target == "ui_dec") {
		dec(ui, .01f);
	} else if(action.target == "clear_framebuffers") {
		framebuffer0.begin();
		ofClear(0, 0, 0, 255);
		framebuffer0.end();
		for (int i = 0; i < framebufferLength; i++) {
			pastFrames[i].begin();
			ofClear(0, 0, 0, 255);
			pastFrames[i].end();
		}
	} else if(action.target == "brightInvert_toggle") {
		brightInvert = !brightInvert;
	} else if(action.target == "hueInvert_toggle") {
		hueInvert = !hueInvert;
	} else if(action.target == "saturationInvert_toggle") {
		saturationInvert = !saturationInvert;
	} else if(action.target == "verticalMirror_toggle") {
		verticalMirror = !verticalMirror;
	} else if(action.target == "horizontalMirror_toggle") {
		horizontalMirror = !horizontalMirror;
	} else if(action.target == "toroidSwitch_toggle") {
		toroidSwitch = !toroidSwitch;
	} else if(action.target == "y_skew_inc") {
		inc(y_skew, .01f);
	} else if(action.target == "y_skew_dec") {
		dec(y_skew, .01f);
	} else if(action.target == "x_skew_inc") {
		inc(x_skew, .01f);
	} else if(action.target == "x_skew_dec") {
		dec(x_skew, .01f);
	} else if(action.target == "reset_all") {
		az = 1.0;
		sx = 0;
		dc = 0;
		fv = 1;
		gb = 1;
		hn = 1;
		jm = 0.0;
		kk = 0.0;
		ll = 0.0;
		qw = 0.0;
		er = 1.0;
		ty = 0.0;
		ui = 0.0;
		op = 0.0;
		fb0_delayamount = 0;
		brightInvert = 0;
		hueInvert = 0;
		saturationInvert = 0;
		verticalMirror = 0;
		horizontalMirror = 0;
		x_skew = 0;
		y_skew = 0;
		framebuffer0.begin();
		ofClear(0, 0, 0, 255);
		framebuffer0.end();
		for (int i = 0; i < framebufferLength; i++) {
			pastFrames[i].begin();
			ofClear(0, 0, 0, 255);
			pastFrames[i].end();
		}
	}
}

// --- MIDI Mapping JSON Loader ---
void ofApp::loadMidiMapping() {
	ofFile f(midiMappingPath);
	if(!f.exists()) {
		ofLogError("MIDI") << "Mapping file missing: " << midiMappingPath;
		return;
	}
	if(!midiMappingJson.open(midiMappingPath)) {
		ofLogError("MIDI") << "Failed to parse JSON: " << midiMappingPath;
		return;
	}
	midiMappingTimestamp = f.getPocoFile().getLastModified().epochMicroseconds();
	midiCCMap.clear();
	midiButtonMap.clear();
	if(midiMappingJson.isMember("cc")) {
		for(auto& c : midiMappingJson["cc"]) {
			int cc = c.get("cc", -1).asInt();
			if(cc < 0) continue;
			MidiAction a;
			a.type = c.get("type", "value").asString();
			a.target = c.get("target", "").asString();
			a.scale = c.get("scale", 1.0f).asFloat();
			a.offset = c.get("offset", 0.0f).asFloat();
			a.bipolar = c.get("bipolar", false).asBool();
			midiCCMap[cc] = a;
		}
	}
	if(midiMappingJson.isMember("button")) {
		for(auto& b : midiMappingJson["button"]) {
			int cc = b.get("cc", -1).asInt();
			if(cc < 0) continue;
			MidiAction a;
			a.type = b.get("type", "toggle").asString();
			a.target = b.get("target", "").asString();
			midiButtonMap[cc] = a;
		}
	}
	ofLogNotice("MIDI") << "Loaded MIDI mapping: " << midiCCMap.size() << " CCs, " << midiButtonMap.size() << " buttons";
}

void ofApp::reloadMidiMappingIfChanged() {
	uint64_t now = ofGetElapsedTimeMillis();
	if(now < nextMidiMappingCheckMillis) return;
	nextMidiMappingCheckMillis = now + 500;
	ofFile f(midiMappingPath);
	if(!f.exists()) return;
	uint64_t ts = f.getPocoFile().getLastModified().epochMicroseconds();
	if(ts != midiMappingTimestamp) {
		ofLogNotice("MIDI") << "Mapping changed on disk, reloading";
		loadMidiMapping();
	}
}

// --- Key Mapping Loader ---
void ofApp::loadKeyMapping() {
	ofFile f(keyMappingPath);
	keyActionMap.clear();
	if(!f.exists()) {
		ofLogError("KEY") << "Key mapping file missing: " << keyMappingPath;
		return;
	}
	if(!keyMappingJson.open(keyMappingPath)) {
		ofLogError("KEY") << "Failed to parse JSON: " << keyMappingPath;
		return;
	}
	keyMappingTimestamp = f.getPocoFile().getLastModified().epochMicroseconds();
	if(keyMappingJson.isMember("key")) {
		for(auto& k : keyMappingJson["key"]) {
			int key = k.get("key", -1).asInt();
			if(key < 0) continue;
			KeyAction a;
			a.type = k.get("type", "action").asString();
			a.target = k.get("target", "").asString();
			keyActionMap[key] = a;
		}
	}
	ofLogNotice("KEY") << "Loaded key mapping: " << keyActionMap.size() << " keys";
}

void ofApp::reloadKeyMappingIfChanged() {
	uint64_t now = ofGetElapsedTimeMillis();
	if(now < nextKeyMappingCheckMillis) return;
	nextKeyMappingCheckMillis = now + 500;
	ofFile f(keyMappingPath);
	if(!f.exists()) return;
	uint64_t ts = f.getPocoFile().getLastModified().epochMicroseconds();
	if(ts != keyMappingTimestamp) {
		ofLogNotice("KEY") << "Key mapping changed on disk, reloading";
		loadKeyMapping();
	}
}
