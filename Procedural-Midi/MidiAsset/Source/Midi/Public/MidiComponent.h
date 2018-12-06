// Copyright -> Scott Bishel

#pragma once

#include "MidiFile.h"
#include "Event/MidiEvent.h"
#include "Util/MetronomeTick.h"
#include "Util/MidiProcessor.h"

#include "MidiUtils.h"

#include "Components/ActorComponent.h"
#include "MidiComponent.generated.h"

class FMidiProcessorWorker;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEventStart, bool, beginning);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEventStop, bool, finished);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FEventMidiEvent, struct FMidiEvent, Event, int32, time, int, TrackID);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FSysExEventReceive, const TArray<uint8>&, data, int32, time, int, TrackID);

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

	/* Changes the Speed of MIDI playback */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MIDI|Processor")
	float PlaySpeed = 1.0;

	/* Ignores Note OFF events and replaces with Note ON with Velocity = 0 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MIDI|Processor")
	bool SimplifyNote = false;

	/**
	* loads a Midi Asset Data
	* @param MidiAsset - The UMidiAsset Object
	*/
	UFUNCTION(BlueprintCallable, Category = "MIDI|Processor")
	void LoadAsset(class UMidiAsset* MidiAsset);
	
	/**
	* Loads a MIDI file
	* @param path - The path to the file
	*/
	UFUNCTION(BlueprintCallable, Category = "MIDI|Processor")
	void LoadFile(FString path);

	/**
	* Load a MML Script/String - Experimental
	* google tinymml
	* @param sheet - The MML script in string format 
	*/
	UFUNCTION(BlueprintCallable, Category = "MIDI|Processor")
	void LoadMML(FString sheet);

// Other
//-----------------------

private:
	MidiFile* mMidiFile;
	MidiProcessor mProcessor;
	
	bool canInit();

	// MIDI Event Listener

	void onEvent(MidiEvent* _event, long ms);
	void onStart(bool fromBeginning);
	void onStop(bool finish);

public:
	/**
	* start MIDI playback
	* @param background - Plays the MIDI without game affecting
	* @param UseGameTime - use real time or game time to process MIDI
	*/
	UFUNCTION(BlueprintCallable, Category = "MIDI|Processor")
	void start(bool background = false, bool UseGameTime = false);

	/* stop MIDI playback */
	UFUNCTION(BlueprintCallable, Category = "MIDI|Processor")
	void stop();

	/* restart MIDI playback */
	UFUNCTION(BlueprintCallable, Category = "MIDI|Processor")
	void reset();

	/* Did the MIDI alreay started? */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MIDI|Processor")
	bool isStarted();

	/* check if MIDI is playing */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MIDI|Processor")
	bool isRunning();
	
	/* The resolution in pulses, or ticks, per quarter note */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MIDI|Processor")
	int GetResolution();
	
	/* Returns MIDI file duration in seconds
	* Performance issue
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MIDI|Processor")
	float GetDuration();

protected:
	UPROPERTY(BlueprintAssignable, Category = "MIDI|Processor")
	FEventStart OnStart;
	UPROPERTY(BlueprintAssignable, Category = "MIDI|Processor")
	FEventStop OnStop;

	/* Called when a Midi Event is received 
	* @param MidiEvent - The Event
	* @param MS - time of event occured in milliseconds
	* @param Track ID - Which track the event happened
	*/
	UPROPERTY(BlueprintAssignable, Category = "MIDI|Processor")
	FEventMidiEvent OnMidiEvent;

	//UPROPERTY(BlueprintAssignable, Category = "MIDI|Processor")
	//FSysExEventReceive OnSysExEvent;

	//UPROPERTY(BlueprintAssignable, Category = "MIDI|Processor")
	//	FSysExEventReceive OnMetaEvent;

private:

	// Thread
	FMidiProcessorWorker* mWorker = NULL;
	
	/* Get Running in Background */
	bool InBackground = false;
	bool isGameTime;
	
	class MidiCallbackMessage
	{
	public:
		FMidiEvent Event;
		long ms;
		int trackID;
	};

		// Handle Data Racing 
	TQueue<MidiCallbackMessage> mQueue;
};