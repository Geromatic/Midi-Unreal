// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#pragma once

#include "../MidiFile.h"
#include "../Event/MidiEvent.h"
#include "MetronomeTick.h"
#include "MidiEventListener.h"

/**
*	Loads and plays back a MIDI file
*/
class MIDI_API MidiProcessor
{
	static const int PROCESS_RATE_MS = 8;

	MidiFile* mMidiFile;
	bool mRunning;
	double mTicksElapsed;
	double mMsElapsed;
	bool mUseRealClock;

	int mMPQN;
	int mPPQ;

	MetronomeTick* mMetronome;
	TimeSignature sig;

public:
	MidiProcessor();
	~MidiProcessor();

	void load(MidiFile & file);

	void start();
	void stop();
	void reset();

	bool isStarted();
	bool isRunning();

	void setListener(MidiEventListener* listener);
	void setWorld(UWorld *world);
	void setUseRealClock(bool realClock);

	void process();
	
	//Variable that can fix playback speed
	double PlaySpeed;

protected:
	void dispatch(MidiEvent * _event);

private:
	TArray<TArray<MidiEvent*>::TIterator> mCurrEvents;
	uint32 mLastMs;
	MidiEventListener* mListener;
	UWorld *mWorld;
	UWorld *getWorld();
	
	class MidiTrackEventQueue
	{
	private:
		MidiTrack* mTrack;
		TArray<MidiEvent*>::TIterator mIterator;
		TArray<MidiEvent*> mEventsToDispatch;
		MidiEvent* mNext;

	public:
		MidiTrackEventQueue(MidiTrack* track): mIterator(track->getEvents().CreateIterator()), mNext(NULL)
		{
			mTrack = track;

			if (mIterator)
			{
				mNext = *mIterator;
			}
		}

		TArray<MidiEvent*>& getNextEventsUpToTick(double tick)
		{
			mEventsToDispatch.Empty();

			while (mNext != NULL)
			{

				if (mNext->getTick() <= tick)
				{
					mEventsToDispatch.Add(mNext);

					if (++mIterator)
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

		bool hasMoreEvents()
		{
			return mNext != NULL;
		}

		void Reset() {
			mIterator.Reset();
			if (mIterator)
			{
				mNext = *mIterator;
			}
		}
	};
};
