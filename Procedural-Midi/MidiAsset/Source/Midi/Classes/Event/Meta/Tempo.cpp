// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#include "Tempo.h"

#include "../../Util/MidiUtil.h"

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

	int size = getEventSize() - 3;
	output.put((char)size);
	output.write(MidiUtil::intToBytes(mMPQN, 3), 3);
}

Tempo * Tempo::parseTempo(long tick, long delta, istream & input) {

	input.ignore();		// Size = 3

	char buffer[3] = { 0 };
	input.read(buffer, 3);
	int mpqn = MidiUtil::bytesToInt(buffer, 0, 3);

	return new Tempo(tick, delta, mpqn);
}

int Tempo::CompareTo(MidiEvent *other) {
	// Compare time
	int value = MidiEvent::CompareTo(other);
	if (value != 0)
		return value;

	// events are not the same
	if (!(other->getType() == this->getType())) {
		return 1;
	}

	Tempo * o = static_cast<Tempo*>(other);

	if (mMPQN != o->mMPQN) {
		return mMPQN < o->mMPQN ? -1 : 1;
	}
	return 0;
}