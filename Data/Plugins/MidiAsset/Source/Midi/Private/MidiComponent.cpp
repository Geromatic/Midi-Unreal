// Copyright -> Scott Bishel

#include "MidiPrivatePCH.h"
#include "MidiComponent.h"

#include "MidiUtils.h"
#include "MidiFile.h"
#include "Event/Meta/Tempo.h"
#include "Event/Meta/TimeSignature.h"
#include "Event/MidiEvent.h"
#include "Event/ChannelEvent.h"
#include "Event/NoteOn.h"
#include "Event/NoteOff.h"
#include "Util/MidiUtil.h"

#include "MidiAsset.h"

// Sets default values for this component's properties
UMidiComponent::UMidiComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.01f;
	// ...
}

UMidiComponent::~UMidiComponent() {
	if (mMidiFile)
		delete mMidiFile;
	if (mMetronome)
		delete mMetronome;
	mMidiFile = NULL;
	mMetronome = NULL;

}


// Called when the game starts
void UMidiComponent::BeginPlay()
{
	Super::BeginPlay();
	// ...
}


// Called every frame
void UMidiComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	process();
}

TimeSignature UMidiComponent::sig;

void UMidiComponent::LoadAsset(UMidiAsset* MidiAsset) {
	if (mRunning) return;

	if (mMidiFile)
		delete mMidiFile;
	if (mMetronome)
		delete mMetronome;
	mMidiFile = NULL;
	mMetronome = NULL;

	if (!MidiAsset) return;
	const TArray<uint8>& data = MidiAsset->Data;
	if (data.Num() == 0)
		return;

	mMidiFile = new MidiFile((uint8*)data.GetData(), data.Num());

	mRunning = false;
	mTicksElapsed = 0;
	mMsElapsed = 0;

	mMPQN = Tempo::DEFAULT_MPQN();
	mPPQ = mMidiFile->getResolution();

	mMetronome = new MetronomeTick(&sig, mPPQ);

	mCurrEvents.Empty();
	TArray<MidiTrack*>& tracks = mMidiFile->getTracks();
	for (int i = 0; i < tracks.Num(); i++) {
		mCurrEvents.Add(tracks[i]->getEvents().CreateIterator());
	}
}

void UMidiComponent::LoadFile(FString path) {
	if (mRunning) return;

	if (mMidiFile)
		delete mMidiFile;
	if (mMetronome)
		delete mMetronome;
	mMidiFile = NULL;
	mMetronome = NULL;
	
	TArray<uint8> data;
	bool result = FFileHelper::LoadFileToArray(data, path.GetCharArray().GetData());
	if (result == 0 || data.Num() == 0)
		return;

	mMidiFile = new MidiFile((uint8*)data.GetData(), data.Num());

	mRunning = false;
	mTicksElapsed = 0;
	mMsElapsed = 0;

	mMPQN = Tempo::DEFAULT_MPQN();
	mPPQ = mMidiFile->getResolution();

	mMetronome = new MetronomeTick(&sig, mPPQ);

	mCurrEvents.Empty();
	TArray<MidiTrack*>& tracks = mMidiFile->getTracks();
	for (int i = 0; i < tracks.Num(); i++) {
		mCurrEvents.Add(tracks[i]->getEvents().CreateIterator());
	}
}

//-----------------------------------

void UMidiComponent::start() {
	if (mRunning) return;
	mLastMs = milliseconds_now();
	mRunning = true;

	onStart.Broadcast(mMsElapsed == 0);
}

void UMidiComponent::stop() {
	mRunning = false;

	onStop.Broadcast(false);
}

void UMidiComponent::reset() {
	mRunning = false;
	mTicksElapsed = 0;
	mMsElapsed = 0;

	if (mMetronome)
		mMetronome->setTimeSignature(&sig);

	for (int i = 0; i < mCurrEvents.Num(); i++) {
		mCurrEvents[i].Reset();
	}

	onStop.Broadcast(false);
}

bool UMidiComponent::isStarted() {
	return mTicksElapsed > 0;
}
bool UMidiComponent::isRunning() {
	return mRunning;
}

void UMidiComponent::dispatch(MidiEvent * _event) {

	// Tempo and Time Signature events are always needed by the processor
	if (_event->getType() == (MetaEvent::TEMPO & 0XFF)) {
		mMPQN = (static_cast<Tempo*>(_event))->getMpqn();
	}

	else if (_event->getType() == (MetaEvent::TIME_SIGNATURE & 0XFF)) {
		bool shouldDispatch = mMetronome->getBeatNumber() != 1;
		mMetronome->setTimeSignature(static_cast<TimeSignature*>(_event));

		if (shouldDispatch)
		{
			dispatch(mMetronome);
		}
	}
	//	onEvent(*_event, mMsElapsed);

	// TODO
	if (_event->getType() == (ChannelEvent::NOTE_ON & 0x0F)) {
		NoteOn* note = static_cast<NoteOn*>(_event);
		onEvent.Broadcast(CurrentTrack, note->getNoteValue(), note->getVelocity(), mMsElapsed);
	}

	if (_event->getType() == (ChannelEvent::NOTE_OFF & 0x0F)) {
		NoteOff* note = static_cast<NoteOff*>(_event);
		onEvent.Broadcast(CurrentTrack, note->getNoteValue(), 0,  mMsElapsed);
	}
}

long long UMidiComponent::milliseconds_now() {
	static LARGE_INTEGER s_frequency;
	static BOOL s_use_qpc = QueryPerformanceFrequency(&s_frequency);
	if (s_use_qpc) {
		LARGE_INTEGER now;
		QueryPerformanceCounter(&now);
		return (1000LL * now.QuadPart) / s_frequency.QuadPart;
	}
	else {
		return GetTickCount();
	}
}

void UMidiComponent::process() {

	if (!mRunning)
		return;

	long long now = milliseconds_now();
	long msElapsed = now - mLastMs;

	if (msElapsed < PROCESS_RATE_MS)
	{
		return;
	}

	double ticksElapsed = MidiUtil::msToTicks(msElapsed, mMPQN, mPPQ);

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

	for (int i = 0; i < mCurrEvents.Num(); i++) {
		while (mCurrEvents[i]) {
			MidiEvent * _event = *mCurrEvents[i];
			if (_event->getTick() <= mTicksElapsed) {

				CurrentTrack = i;

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
	onStop.Broadcast(true);

}