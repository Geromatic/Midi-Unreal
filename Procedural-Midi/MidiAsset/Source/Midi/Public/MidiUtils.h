// Copyright -> Scott Bishel

#pragma once

#include "MidiUtils.generated.h"

// Middle Key Notes
UENUM(BlueprintType)
enum  ENoteEnum
{
	NE_C 	UMETA(DisplayName = "C"),
	NE_CS 	UMETA(DisplayName = "C#"),
	NE_D 	UMETA(DisplayName = "D"),
	NE_DS 	UMETA(DisplayName = "D#"),
	NE_E 	UMETA(DisplayName = "E"),
	NE_F 	UMETA(DisplayName = "F"),
	NE_FS 	UMETA(DisplayName = "F#"),
	NE_G 	UMETA(DisplayName = "G"),
	NE_GS 	UMETA(DisplayName = "G#"),
	NE_A 	UMETA(DisplayName = "A"),
	NE_AS 	UMETA(DisplayName = "A#"),
	NE_B 	UMETA(DisplayName = "B")
};

/**
 * MIDI Frequency Conversion Utility
 */
UCLASS()
class MIDI_API UMidiUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	// Default Middle key note frequencies
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MIDI|Utilities")
	static float DefaultNoteToFrequency(TEnumAsByte<ENoteEnum> note);

	// Gets the frequency of a note
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MIDI|Utilities")
	static float NoteToFrequency(int32 note);
	// Gets the note of a frequency
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MIDI|Utilities")
	static int32 FrequencyToNote(float Frequency);

	static float SemitoneToFrequency(int32 semitone);
	static int32 FrequencyToSemitone(float Frequency);

	// Number of cents from A4
	static int32 FrequencyToCent(float Frequency);
	static int32 FrequencyToOctave(float Frequency);
};
