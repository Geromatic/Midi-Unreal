// Copyright -> Scott Bishel

#include "MidiPrivatePCH.h"
#include "SoundUtils.h"

#include "Sound/SoundWaveProcedural.h"
#include "SoundWaveProceduralTest.h"

#include <sstream>      // std::stringstream, std::stringbuf

// Audio Formulas
//
// Basic Sin Wave
// f(T) = amplitude * sin((2*PI*Frequency)*Time) | Time = T / Sample Size
//
// Mathmatical Square Wave
// f(x, p) = sin(x / period * 2.0 * M_PI)>=0.0 ? 1.0:-1.0;

_inline float Sin_Wave(float frequency, float time) {
	return sin((2 * PI*frequency)*time);
}

_inline float Square_Wave(float x, float period) {
	return sin(x / period * 2.0 * PI) >= 0.0 ? 1.0 : -1.0;
}

// Mathmatical Saw Wave
const double Sample_Rate = 44100.0;
typedef struct Saw_Data{
	double _phasor = 0.0;
	double _tolerance = 1.0;
}Saw_Data;

double Saw_Wave(double* _frequency, Saw_Data* _data){
	double _val = _data->_phasor;
	_data->_phasor += 2.0 * (1.0 / (Sample_Rate / *_frequency));
	if (_data->_phasor > _data->_tolerance) {
		_data->_phasor -= 2.0;
	}
	return _val;
}

//double SamplingThread::value(double timeStamp) const
//{
//	const double period = 1.0 / d_frequency;
//
//	const double x = MathF::fmod(timeStamp, period);
//	const double v = d_amplitude * FMath::Sin(x / period * 2 * M_PI);
//
//	return v;
//}

namespace little_endian_io
{
	template <typename Word>
	void write_word(stringstream& outs, Word value, unsigned size = sizeof(Word))
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

	sw->SampleRate = 44100;
	sw->NumChannels = 2;

	const int32 BIT_RATE = 16;

	int32 DurationDiv = sw->NumChannels * BIT_RATE * sw->SampleRate;
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