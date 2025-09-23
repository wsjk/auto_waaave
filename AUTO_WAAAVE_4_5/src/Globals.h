#pragma once
#include "ofMain.h"
#include "ofxMidi.h"

// moved from ofApp.cpp
extern bool scaleswitch;
extern const int framebufferLength;
extern bool inputswitch;
extern bool wet_dry_switch;
extern int hdmi_aspect_ratio_switch;

extern float az, sx, dc, fv, gb, hn, jm, kk, ll, qw, er, ty, ui, op;
extern float fb1_brightx;
extern bool toroidSwitch, y_skew_switch, x_skew_switch, lumakeyInvertSwitch;
extern bool x_mirrorSwitch, y_mirrorSwitch;
extern float y_skew, x_skew;
extern bool mirrorSwitch;

extern int fb0_delayamount;

extern int width, height;

extern int audioReactiveControlSwitch;

extern float lowC1,lowC2,lowC3,lowC4,lowC5,lowC6,lowC7,lowC8,lowC9,lowC10,lowC11,lowC12,lowC13,lowC14,lowC15,lowC16;
extern float midC1,midC2,midC3,midC4,midC5,midC6,midC7,midC8,midC9,midC10,midC11,midC12,midC13,midC14,midC15,midC16;
extern float highC1,highC2,highC3,highC4,highC5,highC6,highC7,highC8,highC9,highC10,highC11,highC12,highC13,highC14,highC15,highC16;

extern bool clear_switch;

extern bool hueInvert, saturationInvert, brightInvert;
extern bool cam1_h_invert, cam1_s_invert, cam1_b_invert;
extern bool horizontalMirror, verticalMirror;
extern bool clear_flip;

extern bool x_2, x_5, x_10;
extern bool y_2, y_5, y_10;
extern bool z_2, z_5, z_10;
extern bool r_2, r_5, r_10;

extern bool huexx_0, huexx_1, huexx_2;
extern bool huexy_0, huexy_1, huexy_2;
extern bool huexz_0, huexz_1, huexz_2;

extern int framedelayoffset;
extern unsigned int framecount;
extern ofFbo pastFrames[];

// p_lock
extern const int p_lock_size;
extern bool p_lock_switch;
extern bool p_lock_erase;
extern const int p_lock_number;
extern float p_lock[][240];
extern float p_lock_smooth;
extern float p_lock_smoothed[];
extern bool p_lock_0_switch;
extern int p_lock_increment;

// video reactive + midi latch arrays
extern bool videoReactiveSwitch;

extern float vLumakeyValue, vMix, vHue, vSaturation, vBright;
extern float vTemporalFilterMix, vTemporalFilterResonance, vSharpenAmount;
extern float vX, vY, vZ, vRotate, vHuexMod, vHuexOff, vHuexLfo;

extern bool midiActiveFloat[17];
extern bool vmidiActiveFloat[17];
extern bool midiLowActiveFloat[17];
extern bool midiMidActiveFloat[17];
extern bool midiHighActiveFloat[17];

extern float range;
extern float fftLow, fftMid, fftHigh;

extern float c_lumakey_value, c_mix, c_hue, c_sat, c_bright, c_temporalFilterMix;
extern float c_sharpenAmount, c_x, c_y, c_z, c_rotate, c_huex_off, c_huex_lfo;

extern int d_delay;
extern float d_lumakey_value, d_mix, d_hue, d_sat, d_bright;
extern float d_temporalFilterMix, d_temporalFilterResonance, d_sharpenAmount;
extern float d_x, d_y, d_z, d_rotate, d_huex_mod, d_huex_off, d_huex_lfo;

