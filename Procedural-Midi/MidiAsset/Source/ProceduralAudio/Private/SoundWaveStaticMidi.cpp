// Credit -> Scott Bishel

#include "SoundWaveStaticMidi.h"
#include "ProceduralAudioPrivatePCH.h"

USoundWaveStaticMidi::USoundWaveStaticMidi(const FObjectInitializer& PCIP)
	: Super(PCIP)
	, g_Msec(0.0)
{
	SampleRate = 44100;
	NumChannels = 2;
	Duration = INDEFINITELY_LOOPING_DURATION;
	bProcedural = true;
	bLooping = false;
}

int32 USoundWaveStaticMidi::GeneratePCMData(uint8* PCMData, const int32 SamplesNeeded)
{
	//Number of samples to process
	int SampleBlock, SampleCount = (SamplesNeeded / (2 * sizeof(short))); //2 output channels
	int BytesProvided = 0;
	for (SampleBlock = 64; SampleCount; SampleCount -= SampleBlock, PCMData += (SampleBlock * (2 * sizeof(short))))
	{
		//We progress the MIDI playback and then process TSF_RENDER_EFFECTSAMPLEBLOCK samples at once
		if (SampleBlock > SampleCount) SampleBlock = SampleCount;

		//Loop through all MIDI messages which need to be played up until the current playback time
		for (g_Msec += SampleBlock * (1000.0 / 44100.0); g_MidiMessage && g_Msec >= g_MidiMessage->time; g_MidiMessage = g_MidiMessage->next)
		{
			switch (g_MidiMessage->type)
			{
			case TML_PROGRAM_CHANGE: //channel program (preset) change (special handling for 10th MIDI channel with drums)
				tsf_channel_set_presetnumber(g_TinySoundFont, g_MidiMessage->channel, g_MidiMessage->program, (g_MidiMessage->channel == 9));
				break;
			case TML_NOTE_ON: //play a note
				if(g_MidiMessage->velocity > 0)
					tsf_channel_note_on(g_TinySoundFont, g_MidiMessage->channel, g_MidiMessage->key, g_MidiMessage->velocity / 127.0f);
				else
					tsf_channel_note_off(g_TinySoundFont, g_MidiMessage->channel, g_MidiMessage->key);
				break;
			case TML_NOTE_OFF: //stop a note
				tsf_channel_note_off(g_TinySoundFont, g_MidiMessage->channel, g_MidiMessage->key);
				break;
			case TML_PITCH_BEND: //pitch wheel modification
				tsf_channel_set_pitchwheel(g_TinySoundFont, g_MidiMessage->channel, g_MidiMessage->pitch_bend);
				break;
			case TML_CONTROL_CHANGE: //MIDI controller messages
				tsf_channel_midi_control(g_TinySoundFont, g_MidiMessage->channel, g_MidiMessage->control, g_MidiMessage->control_value);
				break;
			}
		}

		// Render the block of audio samples in float format
		tsf_render_short(g_TinySoundFont, (short*)PCMData, SampleBlock, 0);
		BytesProvided += (SampleBlock * (2 * sizeof(short)));
	}
	return BytesProvided;
}

int32 USoundWaveStaticMidi::GetResourceSizeForFormat(FName Format)
{
	return 0;
}

void USoundWaveStaticMidi::GetAssetRegistryTags(TArray<FAssetRegistryTag>& OutTags) const
{
	// should never be in asset registry
	check(false);
}

FByteBulkData* USoundWaveStaticMidi::GetCompressedData(FName Format, const FPlatformAudioCookOverrides* CompressionOverrides)
{
	return NULL;
}

void USoundWaveStaticMidi::Serialize(FArchive& Ar)
{
	// do not call the USoundWave version of serialize
	USoundBase::Serialize(Ar);
}

void USoundWaveStaticMidi::InitAudioResource(FByteBulkData& CompressedData)
{
	// should never be pushing compressed data
	check(false);
}

bool USoundWaveStaticMidi::InitAudioResource(FName Format)
{
	//nothing to be done to initialize
	return true;
}


void USoundWaveStaticMidi::BeginDestroy()
{
	USoundWave::BeginDestroy();
	tml_free(g_TinyMidiLoader);
	tsf_close(g_TinySoundFont);
}


void USoundWaveStaticMidi::LoadMidi(FString Path, tsf* soundFont)
{
	if (soundFont == NULL)
		return;
	tml_free(g_TinyMidiLoader);
	g_TinySoundFont = tsf_copy(soundFont);

	std::string path = std::string(TCHAR_TO_UTF8(*Path));
	g_TinyMidiLoader = tml_load_filename(path.c_str());

	//Set up the global MidiMessage pointer to the first MIDI message
	g_MidiMessage = g_TinyMidiLoader;
	unsigned int length  = 0;
	tml_get_info(g_MidiMessage, NULL, NULL, NULL, NULL, &length);

	Duration = ((float)length / 1000.0f);
	g_MidiMessage = g_TinyMidiLoader;

}


void USoundWaveStaticMidi::LoadMidi(TArray<uint8> data, tsf* soundFont)
{
	if (soundFont == NULL)
		return;
	tml_free(g_TinyMidiLoader);
	g_TinySoundFont = tsf_copy(soundFont);

	g_TinyMidiLoader = tml_load_memory(data.GetData() , data.Num());

	//Set up the global MidiMessage pointer to the first MIDI message
	g_MidiMessage = g_TinyMidiLoader;
	unsigned int length = 0;
	tml_get_info(g_MidiMessage, NULL, NULL, NULL, NULL, &length);

	Duration = ((float)length / 1000.0f);
	g_MidiMessage = g_TinyMidiLoader;

}