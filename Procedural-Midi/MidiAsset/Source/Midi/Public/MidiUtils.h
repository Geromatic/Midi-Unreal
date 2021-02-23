// Credit -> Scott Bishel

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "MidiStruct.h"
#include "MidiUtils.generated.h"


/**
 * MIDI Frequency Conversion Utility
 */
UCLASS(meta=(DisplayName="MIDI Conversion Utilities"))
class MIDI_API UMidiUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/* Default Middle key note frequencies */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MIDI|Utilities")
	static float DefaultNoteToFrequency(TEnumAsByte<ENoteEnum> note);

	/* Gets the frequency of a note */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MIDI|Utilities")
	static float NoteToFrequency(uint8 note);

	/* Gets a note from a frequency */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MIDI|Utilities")
	static uint8 FrequencyToNote(float Frequency);

	static float SemitoneToFrequency(int32 semitone);
	static int32 FrequencyToSemitone(float Frequency);

	// Number of cents from A4
	static int32 FrequencyToCent(float Frequency);
	static int32 FrequencyToOctave(float Frequency);

	/* Returns the visual representation of a note */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MIDI|Utilities")
	static ENoteEnum NoteToChord(uint8 note);

	/* get Pitch Bend amount */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MIDI|Utilities")
	static int32 GetPitchBendAmount(uint8 data1, uint8 data2);

	/* convert Pitch Bend amount to data values */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MIDI|Utilities")
	static void ConvertPitchBendAmount(int32 amount, uint8& data1, uint8& data2);
};
