#include "Globals.h"

bool scaleswitch=1;
const int framebufferLength=60;
bool inputswitch=1;
bool wet_dry_switch=1;
int hdmi_aspect_ratio_switch=0;

float az=1.0f,sx=0,dc=0,fv=1,gb=1,hn=1,jm=0.0f,kk=0.0f,ll=0.0f,qw=0.0f,er=1.0f,ty=0.0f,ui=0.0f,op=0.0f;
float fb1_brightx=0.0f;
bool toroidSwitch=0,y_skew_switch=FALSE,x_skew_switch=FALSE,lumakeyInvertSwitch=FALSE;
bool x_mirrorSwitch=FALSE,y_mirrorSwitch=FALSE;
float y_skew=0,x_skew=0;
bool mirrorSwitch=FALSE;

int fb0_delayamount=0;

int width=640,height=480;

int audioReactiveControlSwitch=0;

float lowC1=0,lowC2=0,lowC3=0,lowC4=0,lowC5=0,lowC6=0,lowC7=0,lowC8=0,lowC9=0,lowC10=0,lowC11=0,lowC12=0,lowC13=0,lowC14=0,lowC15=0,lowC16=0;
float midC1=0,midC2=0,midC3=0,midC4=0,midC5=0,midC6=0,midC7=0,midC8=0,midC9=0,midC10=0,midC11=0,midC12=0,midC13=0,midC14=0,midC15=0,midC16=0;
float highC1=0,highC2=0,highC3=0,highC4=0,highC5=0,highC6=0,highC7=0,highC8=0,highC9=0,highC10=0,highC11=0,highC12=0,highC13=0,highC14=0,highC15=0,highC16=0;

bool clear_switch=0;

bool hueInvert=FALSE,saturationInvert=FALSE,brightInvert=FALSE;
bool cam1_h_invert=FALSE,cam1_s_invert=FALSE,cam1_b_invert=FALSE;
bool horizontalMirror=FALSE,verticalMirror=FALSE;
bool clear_flip=FALSE;

bool x_2=FALSE,x_5=FALSE,x_10=FALSE;
bool y_2=FALSE,y_5=FALSE,y_10=FALSE;
bool z_2=FALSE,z_5=FALSE,z_10=FALSE;
bool r_2=FALSE,r_5=FALSE,r_10=FALSE;

bool huexx_0=FALSE,huexx_1=FALSE,huexx_2=FALSE;
bool huexy_0=FALSE,huexy_1=FALSE,huexy_2=FALSE;
bool huexz_0=FALSE,huexz_1=FALSE,huexz_2=FALSE;

int framedelayoffset=0;
unsigned int framecount=0;
ofFbo pastFrames[framebufferLength];

const int p_lock_size=240;
bool p_lock_switch=0;
bool p_lock_erase=0;
const int p_lock_number=17;
float p_lock[p_lock_number][p_lock_size];
float p_lock_smooth=.5f;
float p_lock_smoothed[p_lock_number];
bool p_lock_0_switch=1;
int p_lock_increment=0;

bool videoReactiveSwitch=FALSE;

float vLumakeyValue=0,vMix=0,vHue=0,vSaturation=0,vBright=0;
float vTemporalFilterMix=0,vTemporalFilterResonance=0,vSharpenAmount=0;
float vX=0,vY=0,vZ=0,vRotate=0,vHuexMod=0,vHuexOff=0,vHuexLfo=0;

bool midiActiveFloat[17]={0};
bool vmidiActiveFloat[17]={0};
bool midiLowActiveFloat[17]={0};
bool midiMidActiveFloat[17]={0};
bool midiHighActiveFloat[17]={0};

float range=200;
float fftLow=0,fftMid=0,fftHigh=0;

float c_lumakey_value=1.01f,c_mix=2,c_hue=.25f,c_sat=.2f,c_bright=.2f,c_temporalFilterMix=1.1f;
float c_sharpenAmount=.5f,c_x=.01f,c_y=.01f,c_z=.05f,c_rotate=.314159265f,c_huex_off=.25f,c_huex_lfo=.25f;

int d_delay=0;
float d_lumakey_value=0,d_mix=0,d_hue=0,d_sat=0,d_bright=0;
float d_temporalFilterMix=0,d_temporalFilterResonance=0,d_sharpenAmount=0;
float d_x=0,d_y=0,d_z=0,d_rotate=0,d_huex_mod=0,d_huex_off=0,d_huex_lfo=0;

