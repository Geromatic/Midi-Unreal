// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#include "MidiProcessor.h"

#include "../Event/Meta/Tempo.h"
#include "../Event/Meta/TimeSignature.h"
#include "../Event/MidiEvent.h"
#include "../Util/MidiUtil.h"

MidiProcessor::MidiProcessor() : PlayRate(1.0), milliFunction( NULL ){
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

	milliFunction = NULL;
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

	//reset metronome
	mMetronome = new MetronomeTick(&mSig, mPPQ);

	mCurrEvents.clear();
	mCurrEventsEnd.clear();
	vector<MidiTrack*>& tracks = mMidiFile->getTracks();
	for (int i = 0; i < (int)tracks.size(); i++) {
		mCurrEvents.push_back(tracks[i]->getEvents().begin());
		mCurrEventsEnd.push_back(tracks[i]->getEvents().end());
	}
}

void MidiProcessor::start(const double& deltaTime /*= clock()*/) {
	if (mRunning) return;
	
	mLastMs = deltaTime;// clock();
	mRunning = true;

	mListener->onStart(mMsElapsed == 0);
}

void MidiProcessor::stop() {
	if (!mRunning) return;

	mRunning = false;
	mListener->onStop(false);
}

void MidiProcessor::reset() {
	mRunning = false;

	mTicksElapsed = 0;
	mMsElapsed = 0;

	//reset metronome
	if (mMetronome)
		mMetronome->setTimeSignature(&mSig);

	vector<MidiTrack*>& tracks = mMidiFile->getTracks();
	for (int i = 0; i < (int)mCurrEvents.size(); i++) {
		mCurrEvents[i] = tracks[i]->getEvents().begin();
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
	mListener->onEvent(_event, (long)mMsElapsed);
}
// Processes the MIDI file every tick
void MidiProcessor::update(const double& deltaTime /*= clock()*/) {
	if (!mRunning)
		return;

	double now = deltaTime;
	double msElapsed = now - mLastMs;

	// workaround to allow custom timer
	if(milliFunction != NULL)
		msElapsed = milliFunction((unsigned int)msElapsed);

	double ticksElapsed = MidiUtil::msToTicks(msElapsed, mMPQN, mPPQ) * PlayRate;
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

	for (int i = 0; i < (int)mCurrEvents.size(); i++) {
		//TODO re-expose track
		_trackID = i;
		while (mCurrEvents[i] != mCurrEventsEnd[i]) {
			MidiEvent * _event = *mCurrEvents[i];
			if (_event->getTick() <= mTicksElapsed) {
				dispatch(_event);
				++mCurrEvents[i];
			}
			else
				break;
		}
	}

	for (int i = 0; i < (int)mCurrEvents.size(); i++) {
		if (mCurrEvents[i] != mCurrEventsEnd[i])
		{
			return;
		}
	}

	mRunning = false;
	this->reset();
	
	mListener->onStop(true);

}