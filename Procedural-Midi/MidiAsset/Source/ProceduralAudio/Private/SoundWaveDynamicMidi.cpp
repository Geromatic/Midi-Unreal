// Credit -> Scott Bishel

#include "SoundWaveDynamicMidi.h"
#include "ProceduralAudioPrivatePCH.h"

USoundWaveDynamicMidi::USoundWaveDynamicMidi(const FObjectInitializer& PCIP)
	: Super(PCIP)
{
	SampleRate = 44100;
	NumChannels = 2;
	Duration = INDEFINITELY_LOOPING_DURATION;
	bProcedural = true;
	bLooping = false;
}

int32 USoundWaveDynamicMidi::GeneratePCMData(uint8* PCMData, const int32 SamplesNeeded)
{
	//Number of samples to process
	int SampleBlock, SampleCount = (SamplesNeeded / (2 * sizeof(short))); //2 output channels
	int BytesProvided = 0;
	for (SampleBlock = 64; SampleCount; SampleCount -= SampleBlock, PCMData += (SampleBlock * (2 * sizeof(short))))
	{
		//We progress the MIDI playback and then process TSF_RENDER_EFFECTSAMPLEBLOCK samples at once
		if (SampleBlock > SampleCount) SampleBlock = SampleCount;

		//Loop through all MIDI messages which need to be played up until the current playback time
		FMidiEvent message;
		while(messageQueue.Dequeue(message))
		{
			switch (message.Type)
			{
			case EMidiTypeEnum::MTE_PROGRAM_CHANGE: //channel program (preset) change (special handling for 10th MIDI channel with drums)
				tsf_channel_set_presetnumber(tinySoundFont, message.Channel, message.Data1, (message.Channel == 9));
				break;
			case EMidiTypeEnum::MTE_NOTE: //play a note
				if(message.Data2 > 0)
					tsf_channel_note_on(tinySoundFont, message.Channel, message.Data1, message.Data2 / 127.0f);
				else
					tsf_channel_note_off(tinySoundFont, message.Channel, message.Data1);
				break;
			case EMidiTypeEnum::MTE_NOTE_OFF: //stop a note
				tsf_channel_note_off(tinySoundFont, message.Channel, message.Data1);
				break;
			case EMidiTypeEnum::MTE_PITCH_BEND: //pitch wheel modification
			{
				int y = ((int)message.Data2 & 0x7F) << 7;
				int x = ((int)message.Data1);
				tsf_channel_set_pitchwheel(tinySoundFont, message.Channel, (y + x));
			}
			break;
			case EMidiTypeEnum::MTE_CONTROLLER: //MIDI controller messages
				tsf_channel_midi_control(tinySoundFont, message.Channel, message.Data1, message.Data2);
				break;
			}
		}

		// Render the block of audio samples in float format
		tsf_render_short(tinySoundFont, (short*)PCMData, SampleBlock, 0);
		BytesProvided += (SampleBlock * (2 * sizeof(short)));
		if (messageQueue.IsEmpty())
			break;
	}

	//Loop through all MIDI messages which need to be played up until the current playback time



	//FMidiEvent message;
	//while (messageQueue.Dequeue(message))
	//{	
	//	switch (message.Type)
	//	{
	//	case EMidiTypeEnum::MTE_PROGRAM_CHANGE: //channel program (preset) change (special handling for 10th MIDI channel with drums)
	//		tsf_channel_set_presetnumber(tinySoundFont, message.Channel, message.Data1, (message.Channel == 9));
	//		break;
	//	case EMidiTypeEnum::MTE_NOTE: //play a note
	//		if(message.Data2 > 0)
	//			tsf_channel_note_on(tinySoundFont, message.Channel, message.Data1, message.Data2 / 127.0f);
	//		else
	//			tsf_channel_note_off(tinySoundFont, message.Channel, message.Data1);
	//		break;
	//	case EMidiTypeEnum::MTE_NOTE_OFF: //stop a note
	//		tsf_channel_note_off(tinySoundFont, message.Channel, message.Data1);
	//		break;
	//	case EMidiTypeEnum::MTE_PITCH_BEND: //pitch wheel modification
	//	{
	//		int y = ((int)message.Data2 & 0x7F) << 7;
	//		int x = ((int)message.Data1);
	//		tsf_channel_set_pitchwheel(tinySoundFont, message.Channel, (y + x));
	//	}
	//	break;
	//	case EMidiTypeEnum::MTE_CONTROLLER: //MIDI controller messages
	//		tsf_channel_midi_control(tinySoundFont, message.Channel, message.Data1, message.Data2);
	//		break;
	//	}

	//}

	//// Render the block of audio samples in float format
	//tsf_render_short(tinySoundFont, (short*)PCMData, 64, 0);
	//int BytesProvided = (64 * (2 * sizeof(short)));

	return BytesProvided;
}

int32 USoundWaveDynamicMidi::GetResourceSizeForFormat(FName Format)
{
	return 0;
}

void USoundWaveDynamicMidi::GetAssetRegistryTags(TArray<FAssetRegistryTag>& OutTags) const
{
	// should never be in asset registry
	check(false);
}

FByteBulkData* USoundWaveDynamicMidi::GetCompressedData(FName Format, const FPlatformAudioCookOverrides* CompressionOverrides)
{
	return NULL;
}

void USoundWaveDynamicMidi::Serialize(FArchive& Ar)
{
	// do not call the USoundWave version of serialize
	USoundBase::Serialize(Ar);
}

void USoundWaveDynamicMidi::InitAudioResource(FByteBulkData& CompressedData)
{
	// should never be pushing compressed data
	check(false);
}

bool USoundWaveDynamicMidi::InitAudioResource(FName Format)
{
	//nothing to be done to initialize
	return true;
}


void USoundWaveDynamicMidi::BeginDestroy()
{
	USoundWave::BeginDestroy();
	tsf_close(tinySoundFont);
	tinySoundFont = NULL;
}


void USoundWaveDynamicMidi::LoadFont(tsf* soundFont)
{
	if (soundFont == NULL)
		return;
	tinySoundFont = tsf_copy(soundFont);

	//Initialize preset on special 10th MIDI channel to use percussion sound bank (128) if available
	tsf_channel_set_bank_preset(tinySoundFont, 9, 128, 0);

}

void USoundWaveDynamicMidi::QueueEvent(const FMidiEvent& e)
{
	messageQueue.Enqueue(e);
}
