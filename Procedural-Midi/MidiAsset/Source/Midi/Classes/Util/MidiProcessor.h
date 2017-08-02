/// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#pragma once

#include "../MidiFile.h"
#include "../Event/MidiEvent.h"
#include "MetronomeTick.h"
#include "MidiEventListener.h"

class FMidiProcessorWorker;

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

	void process();

	//Variable that can fix playback speed
	double PlaySpeed;

	bool processInBackground;

protected:
	void dispatch(MidiEvent * _event);

private:
	TArray<TArray<MidiEvent*>::TIterator> mCurrEvents;
	uint32 mLastMs;
	MidiEventListener* mListener;

	class MidiTrackEventQueue
	{
	private:
		MidiTrack* mTrack;
		TArray<MidiEvent*>::TIterator mIterator;
		TArray<MidiEvent*> mEventsToDispatch;
		MidiEvent* mNext;

	public:
		MidiTrackEventQueue(MidiTrack* track) : mIterator(track->getEvents().CreateIterator()), mNext(NULL)
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

	FMidiProcessorWorker* mRunnable;
};

//////////////////////////////// Threading ///////////////////////////////////////////

//~~~~~ Multi Threading ~~~
class FMidiProcessorWorker : public FRunnable
{
	/** Thread to run the worker FRunnable on */
	FRunnableThread* Thread;

	/** The PC */
	MidiProcessor* ThePC;

public:

	//Done?
	bool IsFinished() const
	{
		return !ThePC->isRunning();
	}

	//~~~ Thread Core Functions ~~~

	//Constructor / Destructor
	FMidiProcessorWorker(MidiProcessor* IN_PC) : ThePC(IN_PC)
	{
		Thread = FRunnableThread::Create(this, TEXT("FMidiProcessorWorker"), 0, TPri_BelowNormal); //windows default = 8mb for thread, could specify more
	}
	virtual ~FMidiProcessorWorker() {
		delete Thread;
		Thread = NULL;
	}

	// Begin FRunnable interface.
	virtual bool Init() {
		return true;
	}
	virtual uint32 Run() {
		while (!IsFinished())
		{
			ThePC->process();
			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			//prevent thread from using too many resources
			FPlatformProcess::Sleep(0.008f);
			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		}
		return 0;
	}
	virtual void Stop() {
		Thread->WaitForCompletion();
	}
	// End FRunnable interface

};