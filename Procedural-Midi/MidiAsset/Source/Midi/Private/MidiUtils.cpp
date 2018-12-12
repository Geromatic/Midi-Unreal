// Copyright -> Scott Bishel

#include "MidiUtils.h"
#include "MidiPrivatePCH.h"


const int CENTER_NOTE = 69;
const float CENTER_FREQUENCY = 440.0;

#define FreqToOct(f)  (FMath::Log2(f / CENTER_FREQUENCY))
#define SemiToFreq(s) (CENTER_FREQUENCY * FMath::Pow(2.0, s / 12.0))
#define FreqToSemi(f) (12 * FreqToOct(f))

float UMidiUtils::DefaultNoteToFrequency(TEnumAsByte<ENoteEnum> note) {
	return NoteToFrequency((uint8)note + 60);
}

float UMidiUtils::NoteToFrequency(uint8 note) {
	//	Hertz = 440.0 * pow(2.0, (midi note - 69) / 12);
	// Diatonic Scale
	float Frequency = 440.0 * FMath::Pow(2.0, (note - 69) / 12.0);
	return Frequency;
}

uint8 UMidiUtils::FrequencyToNote(float Frequency) {
	// midi note = log(Hertz/440.0)/log(2) * 12 + 69;
	int32 MidiNote = 69 + 12 * FMath::Log2(Frequency / 440.0);
	return MidiNote;
}

float UMidiUtils::SemitoneToFrequency(int32 semitone) {
	// frequency = 2^(semitone/12) * 440 Hz
	float Frequency = FMath::Pow(2.0, semitone / 12.0) * 440.0;
	return Frequency;
}

int32 UMidiUtils::FrequencyToSemitone(float Frequency) {
	// n = 12*(log (Frequency/440) / log(2)) 
	int32 Semitone = 12 * FMath::Log2(Frequency / 440.0);
	return Semitone;
}

int32 UMidiUtils::FrequencyToCent(float Frequency) {
	// cent = log(Hertz/440.0)/log(2) * 12;
	int32 Cent = FMath::Log2(Frequency / 440.0) * 1200.0;
	return Cent;
}

int32 UMidiUtils::FrequencyToOctave(float Frequency) {
	// octave = log(Hertz/440.0)/log(2);
	int32 Octave = FMath::Log2(Frequency / 440.0);
	return Octave;
}

ENoteEnum UMidiUtils::NoteToChord(uint8 note) {
	return (ENoteEnum)(note % 12);
}

int32 UMidiUtils::GetPitchBendAmount(uint8 data1, uint8 data2) {
	int y = ((int)data2 & 0x7F) << 7;
	int x = ((int)data1);

	return y + x;
}

void UMidiUtils::ConvertPitchBendAmount(int32 amount, uint8& data1, uint8& data2) {
	amount = amount & 0x3FFF;
	data1 = (amount & 0x7F);
	data2 = amount >> 7;
}