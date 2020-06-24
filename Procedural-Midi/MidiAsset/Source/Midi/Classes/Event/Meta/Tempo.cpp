// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#include "Tempo.h"

#include "../../Util/MidiUtil.h"
#include "GenericMetaEvent.h"

const float Tempo::DEFAULT_BPM = 120.0f;
const int Tempo::DEFAULT_MPQN = (int)(60000000 / DEFAULT_BPM);

Tempo::Tempo() 
	: MetaEvent(0, 0, MetaEvent::TEMPO, new VariableLengthInt(3))
{
	setMpqn(DEFAULT_MPQN);
}

Tempo::Tempo(long tick, long delta, int mpqn)
	: MetaEvent(tick, delta, MetaEvent::TEMPO, new VariableLengthInt(3))
{
	setMpqn(mpqn);
}

int Tempo::getMpqn() {
	return mMPQN;
}
float Tempo::getBpm() {
	return mBPM;
}

void Tempo::setMpqn(int m) {
	mMPQN = m;
	mBPM = 60000000.0f / mMPQN;
}
void Tempo::setBpm(float b) {
	mBPM = b;
	mMPQN = (int)(60000000 / mBPM);
}

int Tempo::getEventSize() {
	return 6;
}

void Tempo::writeToFile(ostream & output) {
	MetaEvent::writeToFile(output);

	output.put((char)3); // size
	output.write(MidiUtil::intToBytes(mMPQN, 3), 3);
}

MetaEvent * Tempo::parseTempo(long tick, long delta, MetaEventData& info) {
	// Check if valid Event
	if (info.length->getValue() != 3)
	{
		return new GenericMetaEvent(tick, delta, info);
	}

	int mpqn = MidiUtil::bytesToInt(info.data, 0, 3);

	return new Tempo(tick, delta, mpqn);
}

int Tempo::compareTo(MidiEvent *other) {
	// Compare time
	if (mTick != other->getTick()) {
		return mTick < other->getTick() ? -1 : 1;
	}
	if (mDelta->getValue() != other->getDelta()) {
		return mDelta->getValue() < other->getDelta() ? 1 : -1;
	}

	// Check if same event type
	if (!(other->getType() == MetaEvent::TEMPO)) {
		return 1;
	}

	Tempo * o = static_cast<Tempo*>(other);

	if (mMPQN != o->mMPQN) {
		return mMPQN < o->mMPQN ? -1 : 1;
	}
	return 0;
}