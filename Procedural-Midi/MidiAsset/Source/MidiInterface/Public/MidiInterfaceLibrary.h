// Copyright -> Scott Bishel

#pragma once

#include "RtMidi.h"

#include "MidiUtils.h"

#include "MidiInterfaceLibrary.generated.h"

/**
 * A library that that lets you send MIDI to a device (ex. computer sound card)
 */
UCLASS(meta=(DisplayName = "MIDI Interface Library"))
class MIDIINTERFACE_API UMidiInterface : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	static RtMidiIn midiIn;
	static RtMidiOut midiOut;

public:	

	static bool OpenMidiInput(uint8 port = 0);
	// Opens a MIDI output device (ex. computer sound card)
	UFUNCTION(BlueprintCallable, Category = "MIDI|Interface")
	static bool OpenMidiOutput(uint8 port = 0);
	
	static void CloseMidiInput();
	// Closes the MIDI output device
	UFUNCTION(BlueprintCallable, Category = "MIDI|Interface")
	static void CloseMidiOutput();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MIDI|Interface", meta=(DisplayName="IsMidiOutputOpen"))
	static bool isOutputOpen();
	
	// Sends a Midi Event to a Device
	UFUNCTION(BlueprintCallable, Category = "MIDI|Interface")
	static void SendMidiEvent(const FMidiEvent& Event);

	UFUNCTION(BlueprintCallable, Category = "MIDI|Interface", meta = (DeprecatedFunction, DeprecationMessage = "This function is deprecated, please use SendMidiEvent instead."))
	static void SendMidiMessage(const TArray<uint8>& message);
};
