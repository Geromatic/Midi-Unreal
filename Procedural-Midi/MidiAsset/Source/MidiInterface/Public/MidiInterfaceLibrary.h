// Copyright -> Scott Bishel

#pragma once

#include "RtMidi.h"

#include "MidiInterfaceLibrary.generated.h"

/**
 * A library that acts like a Midi playback
 */
UCLASS()
class MIDIINTERFACE_API UMidiInterface : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	static RtMidiIn midiIn;
	static RtMidiOut midiOut;

public:	

	//UFUNCTION(BlueprintCallable, Category = "Midi|Interface")
	static bool OpenMidiInput(uint8 port = 0);
	UFUNCTION(BlueprintCallable, Category = "Midi|Interface")
	static bool OpenMidiOutput(uint8 port = 0);
	
	//UFUNCTION(BlueprintCallable, Category = "Midi|Interface")
	static void CloseMidiInput();
	UFUNCTION(BlueprintCallable, Category = "Midi|Interface")
	static void CloseMidiOutput();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Midi|Interface")
	static bool isOutputOpen();

	UFUNCTION(BlueprintCallable, Category = "Midi|Interface")
	static void SendMidiMessage(const TArray<uint8>& message);
};
