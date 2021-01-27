// Credit -> Scott Bishel

#include "SoundUtils.h"
#include "ProceduralAudioPrivatePCH.h"

#include "Sound/SoundWaveProcedural.h"
#include "SoundWaveStaticMidi.h"
#include "SoundWaveProceduralTest.h"

#include "SoundWaveDynamicMidi.h"

#include "tsf.h"

#include <sstream>      // std::stringstream, std::stringbuf

namespace little_endian_io
{
	template <typename Word>
	void write_word(std::stringstream& outs, Word value, unsigned size = sizeof(Word))
	{
		for (; size; --size, value >>= 8)
			outs.put(static_cast <char> (value & 0xFF));
	}
}
using namespace little_endian_io;

USoundWave* USoundUtils::CreateSoundWave(uint8* buffer, int32 size)
{
	USoundWaveProcedural* sw = NewObject<USoundWaveProcedural>(USoundWaveProcedural::StaticClass());

	if (!sw)
		return nullptr;

	sw->SetSampleRate(44100);
	sw->NumChannels = 2;

	const int32 BIT_RATE = 16;

	int32 DurationDiv = sw->NumChannels * BIT_RATE * sw->GetSampleRateForCurrentPlatform();
	if (DurationDiv)
		sw->Duration = size * 8.0f / DurationDiv;
	else
		sw->Duration = 0.0f;

	sw->RawPCMDataSize = size;
	sw->SoundGroup = SOUNDGROUP_Default;
	sw->QueueAudio(buffer, size);

	return sw;
}


USoundWave* USoundUtils::CreateSoundWave(const TArray<int>& buffer)
{
	std::stringstream writer;

	for (long i = 0; i < buffer.Num(); i++) {
		write_word(writer, buffer[i], 2);
	}

	const std::string& tmp = writer.str();
	return CreateSoundWave((uint8*)tmp.c_str(), tmp.size());
}

USoundWaveDynamicMidi* GlobalSynthesizer;

void USoundUtils::CloseSynthesizer() {

	if(GlobalSynthesizer)
		GlobalSynthesizer->ConditionalBeginDestroy();
	GlobalSynthesizer = NULL;

	tsf_close(g_TinySoundFont);
	g_TinySoundFont = NULL;
 }

USoundWave* USoundUtils::OpenSynthesizer()
{
	if (g_TinySoundFont == NULL) return NULL;
	if (GlobalSynthesizer)
		return GlobalSynthesizer;

	GlobalSynthesizer = NewObject<USoundWaveDynamicMidi>(USoundWaveDynamicMidi::StaticClass());
	GlobalSynthesizer->LoadFont(g_TinySoundFont);

	return GlobalSynthesizer;
}

void USoundUtils::SynthesizeEvent(const FMidiEvent& Event)
{
	if(GlobalSynthesizer)
		GlobalSynthesizer->QueueEvent(Event);
}

USoundWave* USoundUtils::AudioNote(float amplitude, float frequency, float seconds)
{
	std::stringstream writer;

	double hz = 44100;    // samples per second

	int N = hz * seconds;  // total number of samples
	for (int s = 0; s < N; s++) {
		int value = amplitude * FMath::Sin((2 * PI * frequency) * ((double)s / hz));
		if (value < 0)
			value = 0;
		write_word(writer, value, 2);
	}

	const std::string& tmp = writer.str();
	return CreateSoundWave((uint8*)tmp.c_str(), tmp.size());
}

USoundWave* USoundUtils::ConstantNote(float amplitude, float frequency) {
	USoundWaveProceduralTest* sw = NewObject<USoundWaveProceduralTest>(USoundWaveProceduralTest::StaticClass());
	sw->Amplitude = amplitude;
	sw->Frequency = frequency;

	return sw;
}

void USoundUtils::LoadSoundFont(const FString& Path) {
	tsf_close(g_TinySoundFont);
	g_TinySoundFont = NULL;

	if (!g_TinySoundFont) {

		TArray<uint8> data;
		bool result = FFileHelper::LoadFileToArray(data, Path.GetCharArray().GetData());
		if (result == 0 || data.Num() == 0) {
			UE_LOG(LogTemp, Error, TEXT("Could not load SoundFont"));
			return;
		}

		g_TinySoundFont = tsf_load_memory(data.GetData(), data.Num());
		if (!g_TinySoundFont)
		{
			UE_LOG(LogTemp, Error, TEXT("Could not load SoundFont"));
			return;
		}

		//Initialize preset on special 10th MIDI channel to use percussion sound bank (128) if available
//		tsf_channel_set_bank_preset(g_TinySoundFont, 9, 128, 0);

		// Set the SoundFont rendering output mode
		tsf_set_output(g_TinySoundFont, TSF_STEREO_INTERLEAVED, 44100, -10.0F);

	}
}

USoundWave* USoundUtils::CreateStaticMidiFromPath(const FString& Path) {
	if (g_TinySoundFont == NULL) return NULL;

	TArray<uint8> data;
	bool result = FFileHelper::LoadFileToArray(data, Path.GetCharArray().GetData());
	if (result == 0 || data.Num() == 0)
		return NULL;

	USoundWaveStaticMidi* sw = NewObject<USoundWaveStaticMidi>(USoundWaveStaticMidi::StaticClass());
	sw->LoadMidi(data, g_TinySoundFont);

	return sw;
}

struct membuf : std::streambuf
{
	membuf(char* begin, char* end) {
		this->setg(begin, begin, end);
	}
};

USoundWave* USoundUtils::CreateStaticMidiFromAsset(class UMidiAsset* MidiAsset) {
	if (g_TinySoundFont == NULL) return NULL;
	if (!MidiAsset) return nullptr;
	const TArray<uint8>& data = MidiAsset->Data;
	if (data.Num() == 0)
		return nullptr;

	USoundWaveStaticMidi* sw = NewObject<USoundWaveStaticMidi>(USoundWaveStaticMidi::StaticClass());
	sw->LoadMidi(data, g_TinySoundFont);

	return sw;
}

USoundWave* USoundUtils::CreateMidiWave(const FString& Path) {

	// TODO: this may leak
	TArray<uint8> data;
	bool result = FFileHelper::LoadFileToArray(data, Path.GetCharArray().GetData());
	if (result == 0 || data.Num() == 0)
		return NULL;

	tml_message* tinyMidiLoader = tml_load_memory(data.GetData(), data.Num());


	//Set up the MidiMessage pointer to the first MIDI message
	tml_message* midiMessage = tinyMidiLoader;

	tsf* tinySoundFont = tsf_copy(g_TinySoundFont);
	//Initialize preset on special 10th MIDI channel to use percussion sound bank (128) if available
	tsf_channel_set_bank_preset(tinySoundFont, 9, 128, 0);


	std::stringstream writer;
	double mSec = 0;

	const int SamplesNeeded = 4096;
	const short stream[SamplesNeeded] = { 0 };
	uint8* PCMData = NULL;

	while (midiMessage) {
//		int BytesProvided = 0;
		// start at begining of stream
		PCMData = (uint8*)stream;
		int SampleBlock, SampleCount = SamplesNeeded  / 2; //2 output channels
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
					tsf_channel_note_on(tinySoundFont, midiMessage->channel, midiMessage->key, midiMessage->velocity / 127.0f);
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
//			BytesProvided += (SampleBlock * (2 * sizeof(short)));
		}
		for (int i = 0; i < SamplesNeeded; i++) {
			write_word(writer, stream[i], 2);
		}

	}

	tml_free(tinyMidiLoader);
	tsf_close(tinySoundFont);

	const std::string& tmp = writer.str();
	uint8* dat = new uint8[tmp.size()];
	memcpy(dat, (uint8*)tmp.c_str(), tmp.size());

	// Create the SoundWave

	USoundWave* sw = NewObject<USoundWave>(USoundWave::StaticClass());

	if (!sw)
		return nullptr;

	sw->SetSampleRate(44100);
	sw->NumChannels = 2;

	const int32 BIT_RATE = 16;

	int32 DurationDiv = sw->NumChannels * BIT_RATE * sw->GetSampleRateForCurrentPlatform();
	if (DurationDiv)
		sw->Duration = tmp.size() * 8.0f / DurationDiv;
	else
		sw->Duration = 0.0f;

	sw->RawPCMDataSize = tmp.size();
	sw->SoundGroup = SOUNDGROUP_Default;

	sw->RawPCMData = dat;

	return sw;
}