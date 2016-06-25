// Copyright -> Scott Bishel

#pragma once

#include "MidiFile.h"
#include "Event/MidiEvent.h"
#include "Util/MetronomeTick.h"

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
class MIDI_API UMidiComponent : public UActorComponent
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
	static const int PROCESS_RATE_MS = 8;

	MidiFile* mMidiFile;
	bool mRunning;
	double mTicksElapsed;
	long mMsElapsed;

	int mMPQN;
	int mPPQ;

	MetronomeTick* mMetronome;
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
	FEventStart onStart;
	UPROPERTY(BlueprintAssignable, Category = "Midi")
	FEventStop onStop;
	// Called when a Note On/Off is called
	UPROPERTY(BlueprintAssignable, Category = "Midi")
	FEventMidi onEvent;

	void dispatch(MidiEvent * _event);

private:
	long long milliseconds_now();

	void process();

	TArray<TArray<MidiEvent*>::TIterator> mCurrEvents;
	long mLastMs;
//-----------------------------------------
	static TimeSignature sig;
	int CurrentTrack;

	class MidiTrackEventQueue
	{
		MidiTrack *mTrack;
		TArray<MidiEvent*>::TIterator mIterator;
		TArray<MidiEvent*> mEventsToDispatch;
		MidiEvent* mNext;
	public:
		MidiTrackEventQueue(MidiTrack *track): mIterator(mTrack->getEvents().CreateIterator())
		{

			mTrack = track;

	//		mIterator = mTrack->getEvents().CreateConstIterator();

			if (mIterator)
			{
				mNext = *mIterator;
			}
		}

		TArray<MidiEvent*> getNextEventsUpToTick(double tick)
		{

			mEventsToDispatch.Empty();

			while (mNext != NULL)
			{

				if (mNext->getTick() <= tick)
				{
					mEventsToDispatch.Add(mNext);

					if (mIterator++)
					{
						mNext = *mIterator;
					}
					else
					{
						mNext = NULL;
					}
				}
				else
				{
					break;
				}
			}

			return mEventsToDispatch;
		}


		void Reset() {
			mIterator.Reset();
		}

		bool hasMoreEvents()
		{
			return mNext != NULL;
		}
	};
};
