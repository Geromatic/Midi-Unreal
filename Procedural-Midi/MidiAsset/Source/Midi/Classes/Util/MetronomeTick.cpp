// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#include "MetronomeTick.h"

#include <math.h>

MetronomeTick::MetronomeTick(TimeSignature* sig, int resolution) 
	: MidiEvent(0, 0), mMetronomeProgress(0.0)
{
	// custom type to represent class
	mType = MetronomeTick::TYPE;

	mResolution = resolution;

	setTimeSignature(sig);
	mCurrentMeasure = 1;
}

void MetronomeTick::setTimeSignature(TimeSignature* sig)
{
	mSignature = sig;
	mCurrentBeat = 0;

	setMetronomeFrequency(sig->getMeter());
}

bool MetronomeTick::update(double ticksElapsed)
{
	mMetronomeProgress += ticksElapsed;

	if (mMetronomeProgress >= mMetronomeFrequency)
	{
		mMetronomeProgress = fmod(mMetronomeProgress, mMetronomeFrequency);

		mCurrentBeat = (mCurrentBeat + 1) % mSignature->getNumerator();
		if (mCurrentBeat == 0)
		{
			mCurrentMeasure++;
		}

		return true;
	}
	return false;
}

void MetronomeTick::setMetronomeFrequency(int meter)
{
	switch (meter)
	{
	case TimeSignature::METER_EIGHTH:
		mMetronomeFrequency = mResolution / 2;
		break;
	case TimeSignature::METER_QUARTER:
		mMetronomeFrequency = mResolution;
		break;
	case TimeSignature::METER_HALF:
		mMetronomeFrequency = mResolution * 2;
		break;
	case TimeSignature::METER_WHOLE:
		mMetronomeFrequency = mResolution * 4;
		break;
	}
}

int MetronomeTick::getBeatNumber()
{
	return mCurrentBeat + 1;
}

int MetronomeTick::getMeasure()
{
	return mCurrentMeasure;
}

std::string MetronomeTick::toString()
{
	std::stringstream ss;
	ss << "Metronome: " << mCurrentMeasure << "\t" << getBeatNumber();
	return ss.str();
}

int MetronomeTick::compareTo(MidiEvent* o)
{
	return 0;
}

int MetronomeTick::getEventSize()
{
	return 0;
}

int MetronomeTick::getSize()
{
	return 0;
}