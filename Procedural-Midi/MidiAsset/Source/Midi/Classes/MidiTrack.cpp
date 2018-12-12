// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#include "MidiTrack.h"

#include "Util/MidiUtil.h"
#include "Event/NoteOn.h"
#include "Event/Meta/TimeSignature.h"
#include "Event/Meta/Tempo.h"
#include "Event/Meta/EndOfTrack.h"

const char MidiTrack::IDENTIFIER[] = { 'M', 'T', 'r', 'k' };

MidiTrack* MidiTrack::createTempoTrack() {

	MidiTrack* T = new MidiTrack();

	T->insertEvent(new TimeSignature());
	T->insertEvent(new Tempo());

	return T;
}

MidiTrack::MidiTrack() {
	mSize = 0;
	mSizeNeedsRecalculating = false;
	mClosed = false;
}

MidiTrack::MidiTrack(istream & input)
{
	mSize = 0;
	mSizeNeedsRecalculating = false;
	mClosed = false;

	char buffer[4] = { 0 };
	input.read(buffer, 4);

	if (!MidiUtil::bytesEqual(buffer, IDENTIFIER, 0, 4)) {
		cerr << "Track identifier did not match MTrk!";
		return;
	}

	input.read(buffer, 4);
	mSize = MidiUtil::bytesToInt(buffer, 0, 4);

	this->readTrackData(input);
}
MidiTrack::~MidiTrack()
{
	// clear up track data
	std::vector<MidiEvent*>::iterator it;
	for (it = mEvents.begin();
		it != mEvents.end();
		it++) {
		delete *it;
	}
}

void MidiTrack::readTrackData(istream & input)
{
	long totalTicks = 0;

	while (!input.eof()) {

		VariableLengthInt delta(input);
		totalTicks += delta.getValue();

		MidiEvent * E = MidiEvent::parseEvent(totalTicks, delta.getValue(), input);
		if (E == NULL) {
			cout << "Event skipped!";
			continue;
		}

		if (VERBOSE) {
			cout << E->toString().c_str();
		}

		// Not adding the EndOfTrack event here allows the track to be edited
		// after being read in from file.
		if (E->getType() == MetaEvent::END_OF_TRACK) {
			break;
		}
		mEvents.push_back(E);
	}
}

std::vector<MidiEvent*>& MidiTrack::getEvents() {
	return mEvents;
}

int MidiTrack::getEventCount() {
	return (int)mEvents.size();
}

int MidiTrack::getSize() {
	if (mSizeNeedsRecalculating) {
		recalculateSize();
	}
	return mSize;
}

long MidiTrack::getLengthInTicks() {
	if (mEvents.size() == 0) {
		return 0;
	}

	// get the last note
	MidiEvent * E = *(--mEvents.end());
	return E->getTick();
}

void MidiTrack::insertNote(int channel, int pitch, int velocity, long tick, long duration) {

	insertEvent(new NoteOn(tick, channel, pitch, velocity));
	insertEvent(new NoteOn(tick + duration, channel, pitch, 0));
}

void MidiTrack::insertEvent(MidiEvent * newEvent) {

	if (newEvent == NULL) {
		return;
	}

	if (mClosed) {
		cerr << "Error: Cannot add an event to a closed track.";
		return;
	}

	MidiEvent * prev = NULL, *next = NULL;
	std::vector<MidiEvent*>::iterator it;
	for (it = mEvents.begin(); 
		it != mEvents.end();
		it++) {
		next = *it;

		if (next->getTick() > newEvent->getTick()) {
			break;
		}

		prev = next;
		next = NULL;
	}

	mEvents.insert(it, newEvent);
	mSizeNeedsRecalculating = true;

	// Set its delta time based on the previous event (or itself if no previous event exists)
	if (prev != NULL) {
		newEvent->setDelta(newEvent->getTick() - prev->getTick());
	}
	else {
		newEvent->setDelta(newEvent->getTick());
	}

	// Update the next event's delta time relative to the new event.
	if (next != NULL) {
		next->setDelta(next->getTick() - newEvent->getTick());
	}

	mSize += newEvent->getSize();
	if (newEvent->getType() == MetaEvent::END_OF_TRACK) {
		if (next != NULL) {
			cerr << "Attempting to insert EndOfTrack before an existing event.  Use closeTrack() when finished with MidiTrack.";
			return;
		}
		mClosed = true;
	}
}

bool MidiTrack::removeEvent(MidiEvent * E) {

	bool isRemoved = false;

	MidiEvent * prev = NULL, *curr = NULL, *next = NULL;
	std::vector<MidiEvent*>::iterator it;
	for (it = mEvents.begin();
		it != mEvents.end();
		it++) {
		next = *it;

		if (E == curr) {
			mEvents.erase(--it);
			isRemoved = true;
			delete curr;
			break;
		}

		prev = curr;
		curr = next;
		next = NULL;
	}

	if (next == NULL) {
		// Either the event was not found in the track,
		// or this is the last event in the track.
		// Either way, we won't need to update any delta times
		return isRemoved;
	}

	if (!isRemoved) {
		return false;
	}

	if (prev != NULL) {
		next->setDelta(next->getTick() - prev->getTick());
	}
	else {
		next->setDelta(next->getTick());
	}
	return true;
}

void MidiTrack::closeTrack() {
	long lastTick = 0;
	if (mEvents.size() > 0) {
		MidiEvent * last = *(--mEvents.end());
		lastTick = last->getTick() + 1;
	}

	insertEvent(new EndOfTrack(lastTick, 0));
}

void MidiTrack::dumpEvents() {
	std::vector<MidiEvent*>::iterator it;
	for (it = mEvents.begin();
		it != mEvents.end();
		it++) {
		cout << (*it)->toString().c_str();
	}
}

void MidiTrack::recalculateSize() {

	mSize = 0;

	MidiEvent * last = NULL;
	std::vector<MidiEvent*>::iterator it;
	for (it = mEvents.begin();
		it != mEvents.end();
		it++) {
		MidiEvent * E = *it;
		mSize += E->getSize();

		// If an event is of the same type as the previous event,
		// no status char is written.
		if (last != NULL && !E->requiresStatusByte(last)) {
			mSize--;
		}
		last = E;
	}

	mSizeNeedsRecalculating = false;
}

void MidiTrack::writeToFile(ostream & output) {

	if (!mClosed) {
		closeTrack();
	}

	if (mSizeNeedsRecalculating) {
		recalculateSize();
	}

	output.write((char*)IDENTIFIER, 4);
	output.write(MidiUtil::intToBytes(mSize, 4), 4);

	MidiEvent * lastEvent = NULL;

	std::vector<MidiEvent*>::iterator it;
	for (it = mEvents.begin();
		it != mEvents.end();
		it++) {
		MidiEvent * _event = *it;
		if (VERBOSE) {
			cout << "Writing: " << _event->toString().c_str();
		}

		_event->writeToFile(output, _event->requiresStatusByte(lastEvent));

		lastEvent = _event;
	}
}
