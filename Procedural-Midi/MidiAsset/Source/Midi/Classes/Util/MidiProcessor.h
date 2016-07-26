// Copyright -> Scott Bishel

#pragma once

#include "../MidiFile.h"
#include "../Event/MidiEvent.h"
#include "MetronomeTick.h"
#include "MidiEventListener.h"

/**
 * 
 */
class MIDI_API MidiProcessor
{
	static const int PROCESS_RATE_MS = 8;

	MidiFile* mMidiFile;
	bool mRunning;
	double mTicksElapsed;
	long mMsElapsed;

	int mMPQN;
	int mPPQ;

	MetronomeTick* mMetronome;
	TimeSignature sig;

public:
	MidiProcessor();
	~MidiProcessor();

	void load(MidiFile & input);

	void start();
	void stop();
	void reset();

	bool isStarted();
	bool isRunning();

	void setListener(MidiEventListener* listener);

	void process();
protected:
	void dispatch(MidiEvent * _event);

private:
	TArray<TArray<MidiEvent*>::TIterator> mCurrEvents;
	uint32 mLastMs;
	int mCurrentTrack;
	MidiEventListener* mListener;
};
