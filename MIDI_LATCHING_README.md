# MIDI Latching Command-Line Control

## Overview

This update adds command-line control for the MIDI latching feature in AUTO_WAAAVE_4_5.

## Usage

### Running the Application

**With MIDI Latching ENABLED (default):**
```bash
cd AUTO_WAAAVE_4_5/bin
./AUTO_WAAAVE_4_5
```

or explicitly:
```bash
./AUTO_WAAAVE_4_5 --midi-latching-on
./AUTO_WAAAVE_4_5 -l
```

**With MIDI Latching DISABLED:**
```bash
./AUTO_WAAAVE_4_5 --midi-latching-off
./AUTO_WAAAVE_4_5 -L
```

**Show Help:**
```bash
./AUTO_WAAAVE_4_5 --help
./AUTO_WAAAVE_4_5 -h
```

## What is MIDI Latching?

MIDI latching is a feature that prevents unwanted parameter jumps when moving a MIDI controller. 

- **With Latching ON**: When you move a knob/fader, it won't affect the parameter until the controller value gets close to the current parameter value (within a threshold). This prevents sudden jumps.

- **With Latching OFF**: Moving a knob/fader immediately updates the parameter, regardless of its current value. This provides direct, immediate control but may cause sudden jumps.

## Changes Made

### Files Modified:

1. **src/main.cpp**
   - Added command-line argument parsing
   - Added argc/argv parameters to main()
   - Stores app instance to call setMidiLatching()

2. **src/ofApp.h**
   - Added `void setMidiLatching(bool enabled);` method declaration

3. **src/ofApp.cpp**
   - Added global `bool midiLatchingEnabled = true;` variable
   - Implemented `setMidiLatching()` method
   - Wrapped MIDI control latching logic with `if(midiLatchingEnabled)` checks
   - Added else branches for direct control mode (latching disabled)

### Pattern Applied:

The latching code follows this pattern:

**Original (always latches):**
```cpp
if(abs(message.value/127.0f - currentValue) < CONTROL_THRESHOLD){
    midiActiveFloat[index]=1;
}
if(midiActiveFloat[index]==1){
    currentValue = message.value/127.0f;
}
```

**Updated (latching controlled by flag):**
```cpp
if(midiLatchingEnabled){
    if(abs(message.value/127.0f - currentValue) < CONTROL_THRESHOLD){
        midiActiveFloat[index]=1;
    }
    if(midiActiveFloat[index]==1){
        currentValue = message.value/127.0f;
    }
} else {
    // Direct control without latching
    midiActiveFloat[index]=1;
    currentValue = message.value/127.0f;
}
```

## Status

✅ **Completed:**
- Command-line argument parsing
- Help message
- setMidiLatching() method
- MIDI controls 16 and 17 (lumakey and mix) fully updated with latching checks

⚠️ **Partial:**
- Controls 18-127 still need latching checks applied
- Pattern is established and can be applied to remaining controls

## Applying to Remaining Controls

The same pattern used for controls 16 and 17 needs to be applied to:
- Control 18 (hue) - index 2
- Control 19 (saturation) - index 3  
- Control 20 (brightness) - index 4
- Control 21 (temporal filter) - index 5
- Control 22 (temporal filter resonance) - index 6
- Control 23 (sharpen) - index 7
- Controls 120-127 (various parameters) - indices 8-15

Each control has multiple audio reactive modes (low, mid, high) and video reactive modes that all need the wrapping applied.

## Building

To rebuild after changes:
```bash
cd AUTO_WAAAVE_4_5
make clean
make
```

## Testing

1. Run with latching ON (default) and verify knobs don't cause jumps until threshold is crossed
2. Run with latching OFF (-L) and verify knobs provide immediate control
3. Verify the setting is announced on startup: "MIDI Latching is now ENABLED/DISABLED"

