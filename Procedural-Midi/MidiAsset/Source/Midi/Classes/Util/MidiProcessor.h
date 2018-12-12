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
};
