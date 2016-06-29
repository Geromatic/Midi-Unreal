// Copyright -> Scott Bishel

#pragma once

#include "MidiFile.h"
#include "Event/MidiEvent.h"
#include "Util/MetronomeTick.h"
#include "Util/MidiProcessor.h"

#include "Components/ActorComponent.h"
#include "MidiComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEventStart, bool, beginning);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEventStop, bool, finished);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FEventMidi, int32, track,int32, note,int32, velocity,int32, elapsed);

/*
* A component that gives an actor the ability to manipulate a midi asset or to create
* their own asset
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), meta=(DisplayName = "MidiComponent") )
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
	
	// loads the Midi Asset Data
	UFUNCTION(BlueprintCallable, Category = "Midi")
	void LoadAsset(class UMidiAsset* MidiAsset);
	
	UFUNCTION(BlueprintCallable, Category = "Midi")
	void LoadFile(FString path);

// Other
//-----------------------

private:
	MidiFile* mMidiFile;
	MidiProcessor mProcessor;

	// Midi Event Listener

	void onEvent(MidiEvent* _event, int track);
	void onStart(bool fromBeginning);
	void onStop(bool finish);

public:
	UFUNCTION(BlueprintCallable, Category = "Midi")
	void start();
	UFUNCTION(BlueprintCallable, Category = "Midi")
	void stop();
	UFUNCTION(BlueprintCallable, Category = "Midi")
	void reset();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Midi")
	bool isStarted();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Midi")
	bool isRunning();

protected:
	UPROPERTY(BlueprintAssignable, Category = "Midi")
	FEventStart OnStart;
	UPROPERTY(BlueprintAssignable, Category = "Midi")
	FEventStop OnStop;
	// Called when a Note On/Off is called
	UPROPERTY(BlueprintAssignable, Category = "Midi")
	FEventMidi OnEvent;
};
