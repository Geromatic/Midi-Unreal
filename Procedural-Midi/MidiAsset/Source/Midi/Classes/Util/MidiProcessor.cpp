// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#include "MidiPrivatePCH.h"
#include "MidiProcessor.h"

#include "../Event/Meta/Tempo.h"
#include "../Event/Meta/TimeSignature.h"
#include "../Event/MidiEvent.h"
#include "../Util/MidiUtil.h"

MidiProcessor::MidiProcessor() : PlaySpeed(1.0) {
	mMidiFile = NULL;
	mMetronome = NULL;

	mRunning = false;
	mTicksElapsed = 0;
	mMsElapsed = 0;
}

MidiProcessor::~MidiProcessor()
{
	if (mMetronome)
		delete mMetronome;
	mMetronome = NULL;
}

void MidiProcessor::load(MidiFile & file) {
	if (mMetronome)
		delete mMetronome;
	mMetronome = NULL;

	mMidiFile = &file;

	mRunning = false;
	mTicksElapsed = 0;
	mMsElapsed = 0;

	mMPQN = Tempo::DEFAULT_MPQN;
	mPPQ = mMidiFile->getResolution();

	mMetronome = new MetronomeTick(&sig, mPPQ);

	mCurrEvents.Empty();
	TArray<MidiTrack*>& tracks = mMidiFile->getTracks();
	for (int i = 0; i < tracks.Num(); i++) {
		mCurrEvents.Add(tracks[i]->getEvents().CreateIterator());
	}
}

void MidiProcessor::start() {
	if (mRunning) return;
	
	mLastMs = FPlatformTime::Cycles();
	mRunning = true;

	mListener->onStart(mMsElapsed == 0);
}

void MidiProcessor::stop() {
	mRunning = false;

	mListener->onStop(false);
}

void MidiProcessor::reset() {
	mRunning = false;

	// makes sure thread is stopped
	mListener->onStop(false);

	mTicksElapsed = 0;
	mMsElapsed = 0;

	if (mMetronome)
		mMetronome->setTimeSignature(&sig);

	for (int i = 0; i < mCurrEvents.Num(); i++) {
		mCurrEvents[i].Reset();
	}
}

bool MidiProcessor::isStarted() {
	return mTicksElapsed > 0;
}
bool MidiProcessor::isRunning() {
	return mRunning;
}

void MidiProcessor::setListener(MidiEventListener* listener) {
	this->mListener = listener;
}

void MidiProcessor::dispatch(MidiEvent * _event) {

	// Tempo and Time Signature events are always needed by the processor
	if (_event->getType() == MetaEvent::TEMPO) {
		mMPQN = (static_cast<Tempo*>(_event))->getMpqn();
	}

	else if (_event->getType() == MetaEvent::TIME_SIGNATURE) {
		bool shouldDispatch = mMetronome->getBeatNumber() != 1;
		mMetronome->setTimeSignature(static_cast<TimeSignature*>(_event));

		if (shouldDispatch)
		{
			dispatch(mMetronome);
		}
	}
	mListener->onEvent(_event);
}

void MidiProcessor::update(double deltaTime = -1) {
	if (!mRunning)
		return;

	double now = deltaTime;
	double msElapsed = now - mLastMs;
	if (deltaTime < 0) {
		now = FPlatformTime::Cycles();
		msElapsed = FPlatformTime::ToMilliseconds(now - mLastMs);
	}

	double ticksElapsed = (((msElapsed * 1000.0) * mPPQ) / mMPQN) * PlaySpeed;
	if (ticksElapsed < 1) {
		return;
	}

	if (mMetronome->update(ticksElapsed))
	{
		dispatch(mMetronome);
	}

	mLastMs = now;
	mMsElapsed += msElapsed;
	mTicksElapsed += ticksElapsed;

	process();

}

void MidiProcessor::process() {

	for (int i = 0; i < mCurrEvents.Num(); i++) {
		while (mCurrEvents[i]) {
			MidiEvent * _event = *mCurrEvents[i];
			if (_event->getTick() <= mTicksElapsed) {
				dispatch(_event);
				mCurrEvents[i]++;
			}
			else
				break;
		}
	}

	for (int i = 0; i < mCurrEvents.Num(); i++) {
		if (mCurrEvents[i])
		{
			return;
		}
	}

	mRunning = false;
	mListener->onStop(true);

}