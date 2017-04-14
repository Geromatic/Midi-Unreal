// RtMidi: Copyright (c) 2003-2016 Gary P. Scavone

#pragma once

#include "RtMidi.h"

#include "MidiUtils.h"

#include "GameFramework/Actor.h"
#include "MidiInterfaceComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEventReceive, FMidiEvent, Event, float, deltaTime);

/*
* A component that lets you open and receive data from a MIDI device
*/
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent), meta = (DisplayName = "MIDI Interface Component"))
class MIDIINTERFACE_API UMidiInterfaceComponent : public UActorComponent
{
	GENERATED_BODY()

	RtMidiIn midiIn;
	RtMidiOut midiOut;
	
public:	
	// Sets default values for this actor's properties
	UMidiInterfaceComponent();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Opens a MIDI input device (ex. MIDI keyboard)
	UFUNCTION(BlueprintCallable, Category = "MIDI|Interface")
	bool OpenInput(uint8 port = 0);
	// Opens a MIDI output device (ex, Computer Speakers, Synth)
	UFUNCTION(BlueprintCallable, Category = "MIDI|Interface|Local")
	bool OpenOutput(uint8 port = 0);

	// Close the MIDI input device
	UFUNCTION(BlueprintCallable, Category = "MIDI|Interface")
	void CloseInput();
	UFUNCTION(BlueprintCallable, Category = "MIDI|Interface|Local")
	void CloseOutput();
	// Sends MIDI event a a MIDI output device
	UFUNCTION(BlueprintCallable, Category = "MIDI|Interface|Local")
	void Send(const FMidiEvent& Event);

	//  Called when a device sends a Midi Event to the computer
	UPROPERTY(BlueprintAssignable, Category = "MIDI|Interface")
	FEventReceive OnReceiveEvent;
};
