#pragma once

#include "SDL_mixer.h"
#include "hdr/system/sys_events.h"

#define AUDIO_PAN_LEFT      0x00
#define AUDIO_PAN_CENTER    0x01
#define AUDIO_PAN_RIGHT     0x02

#define AUDIO_INIT_ENGINE   0x00
#define AUDIO_PLAY_SAMPLE   0x02
#define AUDIO_STOP_SAMPLE   0x03
#define AUDIO_STOP_ALL      0x04
#define AUDIO_SET_GAIN      0x05
#define AUDIO_PAUSE_STATE   0x06
#define AUDIO_LOAD_ALL      0x07
#define AUDIO_STOP_ENGINE   0x08

extern bool pauseSound;
extern bool audioAvailable;
extern bool as_useSound;
extern int as_audioVolume;
extern int as_numAudioSamples;

struct _sounds
{
	Mix_Chunk *sample;
	const char *fileName;
	bool loadedOk;
	int channel;
};

extern _sounds sound[];

// Handle an audio user event - called from Audio Thread
int aud_processAudioEventQueue ( void *ptr );

// Play a sample
bool aud_playSound ( int whichSound, float distance, int loops, float angle );

// Set the global volume to a new level
void aud_setAudioGain ( int newLevel );