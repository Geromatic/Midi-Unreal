/// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#pragma once

#include "../MidiFile.h"
#include "../Event/MidiEvent.h"
#include "MetronomeTick.h"
#include "MidiEventListener.h"

// system processor clock
#include <time.h>       /* clock_t, clock, CLOCKS_PER_SEC */

/**
*	Loads and plays back a MIDI file
*/
class MidiProcessor
{
	static const int PROCESS_RATE_MS = 8;

	MidiFile* mMidiFile;
	bool mRunning;
	double mTicksElapsed;
	double mMsElapsed;

	int mMPQN;
	int mPPQ;

	MetronomeTick* mMetronome;
	TimeSignature mSig;

public:
	MidiProcessor();
	~MidiProcessor();

	void load(MidiFile & file);

	void start(const double& deltaTime = clock());
	void stop();
	void reset();

	bool isStarted();
	bool isRunning();

	void setListener(MidiEventListener* listener);

	// Sets the play back rate
	double PlayRate;
	/*TODO set time if using user based clock*/
	void setStartClock(double time) {
		mLastMs = time;
	}
	// TODO temp expose to get current track
	int _trackID;
	
	/* allows custom time parser (Unreal engine workaround)
	 ex
	// Funtion
	float customMilliParser(const unsigned int elapsed) {
		return elapsed / 1000;
	}
	// Inside a Function
	mProcessor.milliFunction = customMilliParser;
	*/
	float (*milliFunction)(const unsigned int);

	void update(const double& deltaTime = clock());

protected:
	void dispatch(MidiEvent * _event);

private:
	void process();
	vector<vector<MidiEvent*>::iterator > mCurrEvents;
	vector<vector<MidiEvent*>::iterator > mCurrEventsEnd;

	double mLastMs;
	MidiEventListener* mListener;

	/*class MidiTrackEventIterator
	{
	private:
		MidiTrack* mTrack;
		vector<MidiEvent*>::iterator mIterator, mEnd;
		MidiEvent* mEvent;
		MidiProcessor * mProccessor;

	public:
		MidiTrackEventIterator(MidiTrack* track, MidiProcessor* p) :mEvent(NULL)
		{
			mTrack = track;
			mProccessor = p;

			this->Reset();
		}

		void parseNextEventsUpToTick(double tick)
		{

			while (mEvent != NULL)
			{

				if (mEvent->getTick() <= tick)
				{
					if (mIterator != mEnd)
					{
						mEvent = *mIterator;
						mProccessor->dispatch(mEvent);
						mIterator++;
					}
					else
					{
						mEvent = NULL;
					}
				}
				else
				{
					break;
				}
			}
		}

		bool hasMoreEvents() const
		{
			return mEvent != NULL;
		}

		void Reset() {
			mEvent = NULL;

			mIterator = mTrack->getEvents().begin();
			mEnd = mTrack->getEvents().end();

			if (mIterator != mEnd)
			{
				mEvent = *mIterator;
			}
		}
	};*/
};
