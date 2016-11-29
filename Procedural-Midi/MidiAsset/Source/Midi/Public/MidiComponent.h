// Copyright -> Scott Bishel

#pragma once

#include "MidiFile.h"
#include "Event/MidiEvent.h"
#include "Util/MetronomeTick.h"
#include "Util/MidiProcessor.h"

#include "MidiUtils.h"

#include "Components/ActorComponent.h"
#include "MidiComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEventStart, bool, beginning);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEventStop, bool, finished);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEventMidiEvent, struct FMidiEvent, Event);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FEventMidi, int32, track, int32, note, int32, velocity, int32, elapsed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEventDevice, TArray<uint8>, msg, int32, elapsed);

/*
* A component that loads/plays a MIDI Asset or file
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), meta=(DisplayName = "MIDI Processor Component") )
class MIDI_API UMidiComponent : public UActorComponent, public MidiEventListener
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMidiComponent();
	~UMidiComponent();

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;
	
// MIDI

	//Variable that can fix playback speed [recommended 1.04]
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MIDI|Processor")
	float PlaySpeed = 1;
	
	// loads the Midi Asset Data
	UFUNCTION(BlueprintCallable, Category = "MIDI|Processor")
	void LoadAsset(class UMidiAsset* MidiAsset);
	
	UFUNCTION(BlueprintCallable, Category = "MIDI|Processor")
	void LoadFile(FString path);

// Other
//-----------------------

private:
	MidiFile* mMidiFile;
	MidiProcessor mProcessor;

	// MIDI Event Listener

	void onEvent(MidiEvent* _event);
	void onStart(bool fromBeginning);
	void onStop(bool finish);

public:
	// start MIDI playback
	UFUNCTION(BlueprintCallable, Category = "MIDI|Processor")
	void start();
	// stop MIDI playback
	UFUNCTION(BlueprintCallable, Category = "MIDI|Processor")
	void stop();
	// restart MIDI playback
	UFUNCTION(BlueprintCallable, Category = "MIDI|Processor")
	void reset();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MIDI|Processor")
	bool isStarted();
	// check if MIDI is playing
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MIDI|Processor")
	bool isRunning();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MIDI|Processor")
	int GetResolution();

protected:
	UPROPERTY(BlueprintAssignable, Category = "MIDI|Processor")
	FEventStart OnStart;
	UPROPERTY(BlueprintAssignable, Category = "MIDI|Processor")
	FEventStop OnStop;
	// Called when a Midi Event is received
	UPROPERTY(BlueprintAssignable, Category = "MIDI|Processor")
	FEventMidiEvent OnMidiEvent;
	
	// Obsolete. Use OnMidiEvent
	UPROPERTY(BlueprintAssignable, Category = "MIDI|Processor", meta = (DeprecatedEvent, DeprecationMessage = "This event is deprecated, please use OnMidiEvent instead."))
		FEventMidi OnEvent;

	// Obsolete. Use OnMidiEvent
	UPROPERTY(BlueprintAssignable, Category = "MIDI|Processor", meta = (DeprecatedEvent, DeprecationMessage = "This event is deprecated, please use OnMidiEvent instead."))
		FEventDevice OnSend;
};
