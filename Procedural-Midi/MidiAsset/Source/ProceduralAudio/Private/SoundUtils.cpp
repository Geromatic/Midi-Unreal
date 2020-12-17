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

void USoundUtils::CloseSynthesizer() {
	tsf_close(g_TinySoundFont);
	g_TinySoundFont = NULL;
 }

USoundWaveDynamicMidi* GlobalSynthesizer;


USoundWave* USoundUtils::OpenSynthesizer()
{
//	if (!GlobalSW) {
	GlobalSynthesizer = NewObject<USoundWaveDynamicMidi>(USoundWaveDynamicMidi::StaticClass());
//	}
	GlobalSynthesizer->LoadFont(g_TinySoundFont);

	return GlobalSynthesizer;
}

void USoundUtils::SynthesizeEvent(const FMidiEvent& Event)
{
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
		std::string path = std::string(TCHAR_TO_UTF8(*Path));

		g_TinySoundFont = tsf_load_filename(path.c_str());
		if (!g_TinySoundFont)
		{
			fprintf(stderr, "Could not load SoundFont\n");
			return;
		}

		//Initialize preset on special 10th MIDI channel to use percussion sound bank (128) if available
		tsf_channel_set_bank_preset(g_TinySoundFont, 9, 128, 0);

		// Set the SoundFont rendering output mode
		tsf_set_output(g_TinySoundFont, TSF_STEREO_INTERLEAVED, 44100, 0);

	}
}

USoundWave* USoundUtils::CreateStaticMidiFromPath(const FString& Path) {
	//if (g_TinySoundFont == NULL)
	//	LoadSoundFont("gm.sf2");

	USoundWaveStaticMidi* sw = NewObject<USoundWaveStaticMidi>(USoundWaveStaticMidi::StaticClass());
	sw->LoadMidi(Path, g_TinySoundFont);

	return sw;
}

struct membuf : std::streambuf
{
	membuf(char* begin, char* end) {
		this->setg(begin, begin, end);
	}
};

USoundWave* USoundUtils::CreateStaticMidiFromAsset(class UMidiAsset* MidiAsset) {
	if (!MidiAsset) return nullptr;
	const TArray<uint8>& data = MidiAsset->Data;
	if (data.Num() == 0)
		return nullptr;

	USoundWaveStaticMidi* sw = NewObject<USoundWaveStaticMidi>(USoundWaveStaticMidi::StaticClass());
	sw->LoadMidi(data, g_TinySoundFont);

	return sw;
}

USoundWave* USoundUtils::CreateMidiWave(const FString& Path) {

	std::string path = std::string(TCHAR_TO_UTF8(*Path));
	tml_message* g_TinyMidiLoader = tml_load_filename(path.c_str());


	//Set up the global MidiMessage pointer to the first MIDI message
	tml_message* g_MidiMessage = g_TinyMidiLoader;

	std::stringstream writer;
	double g_Msec = 0;

	const int SamplesNeeded = 4096;
	const short stream[SamplesNeeded] = { 0 };
	uint8* PCMData = NULL;

	while (g_MidiMessage) {
//		int BytesProvided = 0;
		// start at begining of stream
		PCMData = (uint8*)stream;
		int SampleBlock, SampleCount = SamplesNeeded  / 2; //2 output channels
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
					tsf_channel_note_on(g_TinySoundFont, g_MidiMessage->channel, g_MidiMessage->key, g_MidiMessage->velocity / 127.0f);
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
//			BytesProvided += (SampleBlock * (2 * sizeof(short)));
		}
		for (int i = 0; i < SamplesNeeded; i++) {
			write_word(writer, stream[i], 2);
		}

	}

	tml_free(g_TinyMidiLoader);

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