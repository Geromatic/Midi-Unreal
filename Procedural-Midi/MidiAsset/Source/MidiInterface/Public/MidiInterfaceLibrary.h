// Credit -> Scott Bishel

#pragma once

#include "RtMidi.h"
#include "MidiStruct.h"

#include "Kismet/BlueprintFunctionLibrary.h"
#include "MidiInterfaceLibrary.generated.h"

enum EPortType
{
	PT_INPUT = 0 	UMETA(DisplayName = "Input"),
	PT_OUTPUT 	UMETA(DisplayName = "Output"),
};

USTRUCT(BlueprintType)
struct FMidiDevice {

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MIDI|Device")
	FString Name;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MIDI|Device")
	uint8 Port;

	//Constructor
	FMidiDevice()
	{
		this->Name = "Unknown";
		this->Port = 0;
	}

	//Constructor
	FMidiDevice(FString name, uint8 port)
	{
		this->Name = name;
		this->Port = port;
	}
};

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

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MIDI|Interface", meta=(DisplayName="Is Midi Output Open"))
	static bool isOutputOpen();
	
	// Sends a MIDI Event to a Device
	UFUNCTION(BlueprintCallable, Category = "MIDI|Interface")
	static void SendMidiEvent(const FMidiEvent& Event);

	// Sends Raw MIDI data to a MIDI output device
	UFUNCTION(BlueprintCallable, Category = "MIDI|Interface")
	static void SendMidiRaw(const TArray<uint8>& Data);
	
	// Gets all the Input and Output devices
	UFUNCTION(BlueprintCallable, Category = "MIDI|Interface")
	static void GetMidiDevices(TArray<FMidiDevice>& Input, TArray<FMidiDevice>& Output);
};
