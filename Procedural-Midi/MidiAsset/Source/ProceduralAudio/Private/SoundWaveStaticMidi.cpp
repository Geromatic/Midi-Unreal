// Credit -> Scott Bishel

#include "SoundWaveStaticMidi.h"
#include "ProceduralAudioPrivatePCH.h"

USoundWaveStaticMidi::USoundWaveStaticMidi(const FObjectInitializer& PCIP)
	: Super(PCIP)
	, mSec(0.0)
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
		for (mSec += SampleBlock * (1000.0 / 44100.0); midiMessage && mSec >= midiMessage->time; midiMessage = midiMessage->next)
		{
			switch (midiMessage->type)
			{
			case TML_PROGRAM_CHANGE: //channel program (preset) change (special handling for 10th MIDI channel with drums)
				tsf_channel_set_presetnumber(tinySoundFont, midiMessage->channel, midiMessage->program, (midiMessage->channel == 9));
				break;
			case TML_NOTE_ON: //play a note
				if(midiMessage->velocity > 0)
					tsf_channel_note_on(tinySoundFont, midiMessage->channel, midiMessage->key, midiMessage->velocity / 127.0f);
				else
					tsf_channel_note_off(tinySoundFont, midiMessage->channel, midiMessage->key);
				break;
			case TML_NOTE_OFF: //stop a note
				tsf_channel_note_off(tinySoundFont, midiMessage->channel, midiMessage->key);
				break;
			case TML_PITCH_BEND: //pitch wheel modification
				tsf_channel_set_pitchwheel(tinySoundFont, midiMessage->channel, midiMessage->pitch_bend);
				break;
			case TML_CONTROL_CHANGE: //MIDI controller messages
				tsf_channel_midi_control(tinySoundFont, midiMessage->channel, midiMessage->control, midiMessage->control_value);
				break;
			}
		}

		// Render the block of audio samples in float format
		tsf_render_short(tinySoundFont, (short*)PCMData, SampleBlock, 0);
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
	tml_free(tinyMidiLoader);
	tsf_close(tinySoundFont);
}

void USoundWaveStaticMidi::LoadMidi(TArray<uint8> data, tsf* soundFont)
{
	if (soundFont == NULL)
		return;
	tml_free(tinyMidiLoader);
	tinySoundFont = tsf_copy(soundFont);

	//Initialize preset on special 10th MIDI channel to use percussion sound bank (128) if available
	tsf_channel_set_bank_preset(tinySoundFont, 9, 128, 0);

	tinyMidiLoader = tml_load_memory(data.GetData() , data.Num());

	//Set up the global MidiMessage pointer to the first MIDI message
	midiMessage = tinyMidiLoader;
	unsigned int length = 0;
	tml_get_info(midiMessage, NULL, NULL, NULL, NULL, &length);

	Duration = ((float)length / 1000.0f);
	midiMessage = tinyMidiLoader;

}