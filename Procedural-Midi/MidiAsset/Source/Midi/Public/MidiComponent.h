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

	//Changes the Speed of MIDI playback
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MIDI|Processor")
	float PlaySpeed = 1;


	// Use the platform clock, or game time?
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MIDI|Processor")
	bool UseRealClock = true;

	// Ignores Note OFF events and replaces with Note ON with Velocity = 0
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MIDI|Processor")
	bool SimplifyNote = true;

	
	// loads the Midi Asset Data
	UFUNCTION(BlueprintCallable, Category = "MIDI|Processor")
	void LoadAsset(class UMidiAsset* MidiAsset);
	
	UFUNCTION(BlueprintCallable, Category = "MIDI|Processor")
	void LoadFile(FString path);

	// Experimental
	// google tinymml
	UFUNCTION(BlueprintCallable, Category = "MIDI|Processor")
	void LoadMML(FString path);

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
	
		// Returns midi file duration in seconds
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MIDI|Processor")
	float GetDuration();

protected:
	UPROPERTY(BlueprintAssignable, Category = "MIDI|Processor")
	FEventStart OnStart;
	UPROPERTY(BlueprintAssignable, Category = "MIDI|Processor")
	FEventStop OnStop;
	// Called when a Midi Event is received
	UPROPERTY(BlueprintAssignable, Category = "MIDI|Processor")
	FEventMidiEvent OnMidiEvent;
};
