// Copyright -> Scott Bishel

#include "SoundUtils.h"
#include "ProceduralAudioPrivatePCH.h"

#include "Sound/SoundWaveProcedural.h"
#include "SoundWaveProceduralTest.h"

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