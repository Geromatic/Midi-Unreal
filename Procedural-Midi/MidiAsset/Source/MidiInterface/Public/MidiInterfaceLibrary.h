// Copyright -> Scott Bishel

#pragma once

#include "RtMidi.h"

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

	//UFUNCTION(BlueprintCallable, Category = "MIDI|Interface")
	static bool OpenMidiInput(uint8 port = 0);
	// Opens a MIDI output device (ex. computer sound card)
	UFUNCTION(BlueprintCallable, Category = "MIDI|Interface")
	static bool OpenMidiOutput(uint8 port = 0);
	
	//UFUNCTION(BlueprintCallable, Category = "MIDI|Interface")
	static void CloseMidiInput();
	// Closes the MIDI output device
	UFUNCTION(BlueprintCallable, Category = "MIDI|Interface")
	static void CloseMidiOutput();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MIDI|Interface", meta=(DisplayName="IsMidiOutputOpen"))
	static bool isOutputOpen();

	UFUNCTION(BlueprintCallable, Category = "MIDI|Interface")
	static void SendMidiMessage(const TArray<uint8>& message);
};
