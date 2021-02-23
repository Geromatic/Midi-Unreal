// Credit -> Scott Bishel

#pragma once

#include "RtMidi.h"
#include "MidiStruct.h"

#include "Containers/Queue.h"
#include "GameFramework/Actor.h"
#include "MidiInterfaceComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEventReceive, FMidiEvent, Event, float, deltaTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSysExEventReceive,const TArray<uint8>&, data, float, deltaTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FClockEventReceive, FMidiClockEvent, Event, float, deltaTime);
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
	/////////////////////////////////////////////////

	// internal but public as called from callback proc
	static const bool queueCallbacks = true;

	bool getInSysEx() { return inSysEx; }
	void startSysEx();
	void stopSysEx(float deltaTime);
	void appendSysEx(int data);

	void postCallback(double deltatime, std::vector< unsigned char > *message);
	void handleCallback(double deltatime, std::vector< unsigned char > *message);

	/////////////////////////////////////////////////

	// Sets default values for this actor's properties
	UMidiInterfaceComponent();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/**
	* Opens a MIDI input device (ex. MIDI keyboard)
	* @param port - a port number
	* @param ignoreSysEx - whether to ignore system exclusive events
	* @param ignoreTiming - whether to ignore the Midi clock event - high data rate
	*/
	UFUNCTION(BlueprintCallable, Category = "MIDI|Interface")
	bool OpenInput(uint8 port = 0, bool ignoreSysEx = true, bool ignoreTiming = true);
	// Opens a MIDI output device (ex, Computer Speakers, Synth)
	UFUNCTION(BlueprintCallable, Category = "MIDI|Interface|Local")
	bool OpenOutput(uint8 port = 0);

	// Close the MIDI input device
	UFUNCTION(BlueprintCallable, Category = "MIDI|Interface")
	void CloseInput();
	UFUNCTION(BlueprintCallable, Category = "MIDI|Interface|Local")
	void CloseOutput();
	// Sends MIDI event to a MIDI output device
	UFUNCTION(BlueprintCallable, Category = "MIDI|Interface|Local")
	void Send(const FMidiEvent& Event);

	// Sends Raw MIDI data to a MIDI output device
	UFUNCTION(BlueprintCallable, Category = "MIDI|Interface|Local")
	void SendRaw(const TArray<uint8>& Data);


	//  Called when a device sends a MIDI Event to the computer
	UPROPERTY(BlueprintAssignable, Category = "MIDI|Interface")
	FEventReceive OnReceiveEvent;

	//  Called when a device sends a MIDI SysEx Event to the computer
	UPROPERTY(BlueprintAssignable, Category = "MIDI|Interface")
	FSysExEventReceive OnReceiveSysExEvent;


	//  Called when a device sends a MIDI clock Event to the computer
	UPROPERTY(BlueprintAssignable, Category = "MIDI|Interface")
	FClockEventReceive OnReceiveClockEvent;

private:
	/////////////////////////////////

	bool inSysEx;

	class CallbackMessage
	{
	public:
		float deltaTime;
		std::vector< unsigned char > message;
	};
	TArray<uint8> sysExArray;
	TQueue<CallbackMessage, EQueueMode::Spsc> messageQueue;

	void setInSysEx(bool isInSysEx) { inSysEx = isInSysEx; }

	/////////////////////////////////
};
