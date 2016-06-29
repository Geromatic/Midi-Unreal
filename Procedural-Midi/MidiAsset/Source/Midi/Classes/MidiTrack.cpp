// Copyright -> Scott Bishel

#include "MidiPrivatePCH.h"
#include "MidiTrack.h"

#include "Util/MidiUtil.h"
#include "Event/NoteOn.h"
#include "Event/Meta/TimeSignature.h"
#include "Event/Meta/Tempo.h"
#include "Event/Meta/EndOfTrack.h"

const char MidiTrack::IDENTIFIER[] = { 'M', 'T', 'r', 'k' };

MidiTrack MidiTrack::createTempoTrack() {

	MidiTrack T;

	T.insertEvent(new TimeSignature());
	T.insertEvent(new Tempo());

	return T;
}

MidiTrack::MidiTrack() {
	mSize = 0;
	mSizeNeedsRecalculating = false;
	mClosed = false;
}

// TODO Sort
inline static bool ConstPredicate(MidiEvent& ip1, MidiEvent& ip2)
 {
	 return ip1.CompareTo(&ip2) < 0;
 }


MidiTrack::MidiTrack(FBufferReader & input)
{
	mSize = 0;
	mSizeNeedsRecalculating = false;
	mClosed = false;

	char buffer[4] = { 0 };
	input.Serialize(buffer, 4);

	if (!MidiUtil::bytesEqual(buffer, (char*)IDENTIFIER, 0, 4)) {
//		OutputDebugStringA("Track identifier did not match MTrk!");
		UE_LOG(LogTemp, Warning, TEXT("Track identifier did not match MTrk!"));
		return;
	}

	input.Serialize(buffer, 4);
	mSize = MidiUtil::bytesToInt(buffer, 0, 4);

	long totalTicks = 0;

	while (true) {

		VariableLengthInt delta(input);
		totalTicks += delta.getValue();

		MidiEvent * E = MidiEvent::parseEvent(totalTicks, delta.getValue(), input);
		if (E == NULL) {
//			OutputDebugStringA("Event skipped!");
			UE_LOG(LogTemp, Warning, TEXT("Event skipped!"));
			continue;
		}

		if (VERBOSE) {
			stringstream ss;
			ss << (*E).ToString() << endl;
			OutputDebugStringA(ss.str().c_str());
		}

		// Not adding the EndOfTrack event here allows the track to be edited
		// after being read in from file.
		if (E->getType() == (MetaEvent::END_OF_TRACK & 0XFF) ) {
			break;
		}
		mEvents.Add(E);
	}

//	mEvents.Sort(ConstPredicate);
}
MidiTrack::~MidiTrack()
{
	for (int i = 0; i < mEvents.Num(); i++)
	{
		delete mEvents[i];
		mEvents[i] = NULL;
	}
}

TArray<MidiEvent*>& MidiTrack::getEvents() {
	return mEvents;
}

int MidiTrack::getEventCount() {
	return (int)mEvents.Num();
}

int MidiTrack::getSize() {
	if (mSizeNeedsRecalculating) {
		recalculateSize();
	}
	return mSize;
}

long MidiTrack::getLengthInTicks() {
	if (mEvents.Num() == 0) {
		return 0;
	}

	MidiEvent * E = mEvents.Last();
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
		OutputDebugStringA("Error: Cannot add an event to a closed track.");
		return;
	}

	MidiEvent * prev = NULL, *next = NULL;

	const bool isAllowed = false;

	if (isAllowed) {
		//			    prev = mEvents.First(_event => _event.CompareTo(newEvent) == -1);
		//                next = mEvents.Last(_event => _event.CompareTo(newEvent) == 1);
	}
	else {
		for (int it = 0; it < mEvents.Num(); it++) {
			next = mEvents[it];

			if (next->getTick() > newEvent->getTick()) {
				break;
			}

			prev = next;
			next = NULL;
		}
	}

	mEvents.Add(newEvent);
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
	if (newEvent->getType() == (MetaEvent::END_OF_TRACK & 0XFF) ) {
		if (next != NULL) {
			throw ("Attempting to insert EndOfTrack before an existing event. Use closeTrack() when finished with MidiTrack.");
		}
		mClosed = true;
	}
}

bool MidiTrack::removeEvent(MidiEvent * E) {
;
	MidiEvent * prev = NULL, *curr = NULL, *next = NULL;

	for (int it = 0; it < mEvents.Num(); it++) {
		next = mEvents[it];

		if (E == curr) {
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
		return mEvents.Remove(curr) > 0;
	}

	if (!mEvents.Remove(curr)) {
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
	if (mEvents.Num() > 0) {
		MidiEvent * last = mEvents.Last();
		lastTick = last->getTick() + 1;
	}

	insertEvent(new EndOfTrack(lastTick, 0));
}

void MidiTrack::dumpEvents() {
	for (int it = 0; it < mEvents.Num(); it++) {
		//        OutputDebugStringA(*it);
	}
}

void MidiTrack::recalculateSize() {

	mSize = 0;

	MidiEvent * last = NULL;
	for (int it = 0; it < mEvents.Num(); it++) {
		MidiEvent * E = mEvents[it];
		mSize += E->getSize();

		// If an event is of the same type as the previous event,
		// no status char is written.
		if (last != NULL && !(*E).requiresStatusByte(last)) {
			mSize--;
		}
		last = E;
	}

	mSizeNeedsRecalculating = false;
}

void MidiTrack::writeToFile(FMemoryWriter & output) {

	if (!mClosed) {
		closeTrack();
	}

	if (mSizeNeedsRecalculating) {
		recalculateSize();
	}

	output.Serialize((char*)IDENTIFIER, 4);
	output.Serialize(MidiUtil::intToBytes(mSize, 4), 4);

	MidiEvent * lastEvent = NULL;

	for (int it = 0; it < mEvents.Num(); it++) {
		MidiEvent * _event = mEvents[it];
		if (VERBOSE) {
			//			OutputDebugStringA("Writing: " + _event->ToString());
		}

		_event->writeToFile(output, _event->requiresStatusByte(lastEvent));

		lastEvent = _event;
	}
}
