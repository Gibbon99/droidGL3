#include <SDL2/SDL_rwops.h>
#include "hdr/system/sys_main.h"

#include <hdr/io/io_fileSystem.h>
#include "hdr/system/sys_audio.h"


_sounds sound[] = {{nullptr, "collosion1.wav",       false},
                   {nullptr, "endTransmission1.wav", false},
                   {nullptr, "greenAlert.wav",       false},
                   {nullptr, "lift1.wav",            false},
                   {nullptr, "scrollBeeps.wav",      false},
                   {nullptr, "transferdeadlock.wav", false},
                   {nullptr, "yellowAlert.wav",      false},
                   {nullptr, "console1.wav",         false},
                   {nullptr, "endTransmission2.wav", false},
                   {nullptr, "keypressBad.wav",      false},
                   {nullptr, "lift2.wav",            false},
                   {nullptr, "start1.wav",           false},
                   {nullptr, "transferMove.wav",     false},
                   {nullptr, "console2.wav",         false},
                   {nullptr, "energyHeal.wav",       false},
                   {nullptr, "keyPressGood.wav",     false},
                   {nullptr, "lift3.wav",            false},
                   {nullptr, "start2.wav",           false},
                   {nullptr, "transferStage1.wav",   false},
                   {nullptr, "damage.wav",           false},
                   {nullptr, "explode1.wav",         false},
                   {nullptr, "laser.wav",            false},
                   {nullptr, "lowEnergy.wav",        false},
                   {nullptr, "startAll.wav",         false},
                   {nullptr, "transferStage2.wav",   false},
                   {nullptr, "disruptor.wav",        false},
                   {nullptr, "explode2.wav",         false},
                   {nullptr, "levelShutdown.wav",    false},
                   {nullptr, "redAlert.wav",         false},
                   {nullptr, "transfer1.wav",        false},
                   {nullptr, "transferStart.wav",    false},
                   {nullptr, "door.wav",             false}};

int as_audioVolume;
int as_numAudioSamples;         // Set from script
bool as_useSound;                // Set from script
int as_numMultiSamples;        // Set from script
bool pauseSound = false;
bool audioAvailable = false;

//-------------------------------------------------------------------------
//
// Load each sound sample
void aud_loadSoundSamples ()
//-------------------------------------------------------------------------
{
	int *soundfileMemory = nullptr;
	int soundfileSize = -1;
	SDL_RWops *filePtr;

	if ((!audioAvailable) || (!as_useSound))
		return;

	for ( int i = 0; i != NUM_SOUNDS; i++ )
	{
		soundfileSize = io_getFileSize (sound[i].fileName);

		soundfileMemory = (int *)malloc (sizeof (int) * soundfileSize);
		if ( soundfileMemory == nullptr )
		{
			con_print (CON_ERROR, true, "Audio error: Could not get memory to load sound file [ %s [", sound[i].fileName);
			return;
		}
		io_getFileIntoMemory (sound[i].fileName, (void *) soundfileMemory);

		filePtr = SDL_RWFromMem (soundfileMemory, soundfileSize);

		sound[i].sample = Mix_LoadWAV_RW (filePtr, 0);

		free (soundfileMemory);

		if ( !sound[i].sample )
		{
			con_print (CON_ERROR, true, "Error: Failed to load sample [ %s ]", sound[i].fileName);
			con_print (CON_ERROR, true, "Error: [ %s ]", Mix_GetError ());
			sound[i].loadedOk = false;
		}
		else
		{
			con_print (CON_ERROR, true, "Loaded sample [ %s ]", sound[i].fileName);
			sound[i].loadedOk = true;
		}
	}
}

//-------------------------------------------------------------
//
// Pause the sound system
void aud_pauseSoundSystem ( int newState )
//-------------------------------------------------------------
{
	if ((!audioAvailable) || (!as_useSound))
		return;

	pauseSound = static_cast<bool>(newState);
}

//-------------------------------------------------------------------------
//
// Set the global volume to a new level
void aud_setAudioGain ( int newLevel )
//-------------------------------------------------------------------------
{
	if ( newLevel < 0 )
		newLevel = 0;

	if ( newLevel > MIX_MAX_VOLUME )
		newLevel = MIX_MAX_VOLUME;

	as_audioVolume = newLevel;
}

//-------------------------------------------------------------------------
//
// Check if a sample is still playing - return bool
bool aud_isSoundPlaying ( int whichSound )
//-------------------------------------------------------------------------
{
	if ((!audioAvailable) || (!as_useSound))
		return false;

	return static_cast<bool>(Mix_Playing (sound[whichSound].channel));
}

//-------------------------------------------------------------------------
//
// Stop playing a sound - pass in sound index to get SAMPLE_ID
void aud_stopSound ( int whichSound )
//-------------------------------------------------------------------------
{
	if ((!audioAvailable) || (!as_useSound))
		return;

	Mix_HaltChannel (sound[whichSound].channel);
}

//-------------------------------------------------------------------------
//
// Stop playing all sounds
void aud_stopAllSounds ()
//--------------------------------------------------------------------------
{
	if ((!audioAvailable) || (!as_useSound))
		return;

	Mix_HaltChannel (-1);    // stop all channels playing
}

//-------------------------------------------------------------------------
//
// Release the sound systems and samples
void aud_releaseSound ()
//--------------------------------------------------------------------------
{
	if ((!audioAvailable) || (!as_useSound))
		return;

	aud_stopAllSounds ();

	Mix_CloseAudio ();
}

//-------------------------------------------------------------------------
//
// Play a sample
bool aud_playSound ( int whichSound, float pan, int loops )
//-------------------------------------------------------------------------
{
	if ((!audioAvailable) || (pauseSound) || (!as_useSound))
		return false;

	if ( !sound[whichSound].loadedOk )
		return false;

	sound[whichSound].channel = Mix_PlayChannel (-1, sound[whichSound].sample, loops);
	if ( -1 == sound[whichSound].channel )
	{
		printf ("Mix_PlayChannel error [ %s ]", Mix_GetError ());
		return false;
	}

	// pan channel 1 halfway to the left
	if ( !Mix_SetPanning (sound[whichSound].channel, 255, 127))
	{
		printf ("Mix_SetPanning: %s\n", Mix_GetError ());
		// no panning, is it ok?
	}

	return true;
}

//-------------------------------------------------------------------------
//
// Set volume (gain) for a sample
void aud_setVolume ( int whichSound, int volLevel )
//-------------------------------------------------------------------------
{
	if ((!audioAvailable) || (!as_useSound))
		return;

	if ( !aud_isSoundPlaying (whichSound))
		return;

	Mix_VolumeChunk (sound[whichSound].sample, volLevel);
}

//----------------------------------------------------------------
//
// Setup the audio engine and audio thread
bool aud_setupAudioEngine ()
//----------------------------------------------------------------
{
	if ( !as_useSound )
		return false;

	// open 44.1KHz, signed 16bit, system byte order, stereo audio, using 1024 byte chunks
	if ( Mix_OpenAudio (44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1 )
	{
		audioAvailable = false;
		con_print (CON_ERROR, true, "Error installing the audio system. Sound is NOT available.");
		con_print (CON_ERROR, true, "Audio error [ %s ]", Mix_GetError ());
		return false;
	}

	Mix_AllocateChannels (as_numAudioSamples);

	con_print (CON_INFO, true, "Audio engine started.");

	SDL_version compile_version;
	const SDL_version *link_version = Mix_Linked_Version ();
	SDL_MIXER_VERSION (&compile_version);

	con_print (CON_INFO, true, "Compiled with SDL_mixer version [ %d.%d.%d ]", compile_version.major, compile_version.minor, compile_version.patch);

	con_print (CON_INFO, true, "Running with SDL_mixer version [ %d.%d.%d ]", link_version->major, link_version->minor, link_version->patch);

	audioAvailable = true;
	pauseSound = false;     // Start in playing mode

	return true;    // Check return codes
}

//----------------------------------------------------------------
//
// Handle an audio user event - called from Audio Thread
int aud_processAudioEventQueue ( void *ptr )
//----------------------------------------------------------------
{
	_myEventData tempEventData;

	while ( runThreads )
	{
		SDL_Delay (THREAD_DELAY_MS);

		if ( !audioEventQueue.empty ())   // stuff in the queue to process
		{
			if ( SDL_LockMutex (audioMutex) == 0 )
			{
				tempEventData = audioEventQueue.front ();
				audioEventQueue.pop ();
				SDL_UnlockMutex (audioMutex);
			}

			switch ( tempEventData.eventAction )
			{
				case AUDIO_INIT_ENGINE:
					aud_setupAudioEngine ();
					evt_sendEvent (USER_EVENT_AUDIO, AUDIO_LOAD_ALL, 0, 0, 0, vec2(), vec2(), "");
					evt_sendEvent (USER_EVENT_AUDIO, AUDIO_SET_GAIN, SND_LASER, MIX_MAX_VOLUME, 0, glm::vec2 (), glm::vec2 (), "");
					break;

				case AUDIO_STOP_ENGINE:
					aud_releaseSound ();
					break;

				case AUDIO_PLAY_SAMPLE:
					aud_playSound (tempEventData.data1, tempEventData.data2, tempEventData.data3);
					break;

				case AUDIO_LOAD_ALL:
					aud_loadSoundSamples ();
					break;

				case AUDIO_STOP_SAMPLE:
					aud_stopSound (tempEventData.data1);
					break;

				case AUDIO_STOP_ALL:
					aud_stopAllSounds ();
					break;

				case AUDIO_SET_GAIN:
					aud_setVolume (tempEventData.data1, tempEventData.data2);
					break;

				case AUDIO_PAUSE_STATE:
					aud_pauseSoundSystem (tempEventData.data1);
					break;

				default:
					break;
			}
		}
	}
	printf ("AUDIO thread stopped.\n");
	return 0;
}