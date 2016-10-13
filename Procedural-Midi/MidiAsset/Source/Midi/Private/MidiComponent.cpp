// Copyright -> Scott Bishel

#include "MidiPrivatePCH.h"
#include "MidiComponent.h"

#include "MidiUtils.h"
#include "MidiFile.h"
#include "Util/MidiUtil.h"

#include "Event/ChannelAftertouch.h"
#include "Event/ChannelEvent.h"
#include "Event/Controller.h"
#include "Event/NoteAftertouch.h"
#include "Event/NoteOff.h"
#include "Event/NoteOn.h"
#include "Event/PitchBend.h"
#include "Event/ProgramChange.h"
#include "Event/SystemExclusiveEvent.h"

#include "Interface/ShortMessege.h"


#include "MidiAsset.h"

#include "Util/MidiProcessor.h"

// Sets default values for this component's properties
UMidiComponent::UMidiComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.01f;
	// ...
	mProcessor.setListener(this);
	mMidiFile = NULL;
}

UMidiComponent::~UMidiComponent() {
	if (mMidiFile)
		delete mMidiFile;
	mMidiFile = NULL;
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
		
	mProcessor.process();
}

void UMidiComponent::LoadAsset(UMidiAsset* MidiAsset) {
	if (mProcessor.isRunning()) return;

	if (mMidiFile)
		delete mMidiFile;
	mMidiFile = NULL;

	if (!MidiAsset) return;
	const TArray<uint8>& data = MidiAsset->Data;
	if (data.Num() == 0)
		return;

	FBufferReader reader((uint8*)data.GetData(), data.Num(), false);
	mMidiFile = new MidiFile(reader);
	mProcessor.load(*mMidiFile);
}

void UMidiComponent::LoadFile(FString path) {
	if (mProcessor.isRunning()) return;

	if (mMidiFile)
		delete mMidiFile;
	mMidiFile = NULL;
	
	TArray<uint8> data;
	bool result = FFileHelper::LoadFileToArray(data, path.GetCharArray().GetData());
	if (result == 0 || data.Num() == 0)
		return;

	FBufferReader reader((uint8*)data.GetData(), data.Num(), false);
	mMidiFile = new MidiFile(reader);
	mProcessor.load(*mMidiFile);
}

void UMidiComponent::onEvent(MidiEvent* _event) {
	if (_event->getType() == (ChannelEvent::NOTE_ON & 0x0F)) {
		NoteOn* note = static_cast<NoteOn*>(_event);
		OnEvent.Broadcast(note->getChannel(), note->getNoteValue(), note->getVelocity(), _event->getTick());
	}

	else if (_event->getType() == (ChannelEvent::NOTE_OFF & 0x0F)) {
		NoteOff* note = static_cast<NoteOff*>(_event);
		OnEvent.Broadcast(note->getChannel(), note->getNoteValue(), 0, _event->getTick());
	}

	// MIDI Interface data
	if (_event->getType() >= ChannelEvent::NOTE_OFF && _event->getType() <= ChannelEvent::PITCH_BEND) {
		TArray<uint8> data;
		ShortMessage* shortMsg = static_cast<ShortMessage*>(_event);
		data.Add(shortMsg->getStatus());
		data.Add(shortMsg->getData1());
		data.Add(shortMsg->getData2());
		OnSend.Broadcast(data, _event->getTick());
	}
	else if (_event->getType() == 0xF0 || _event->getType() == 0xF7) {
	}
}

void UMidiComponent::onStart(bool fromBeginning) { OnStart.Broadcast(fromBeginning); }
void UMidiComponent::onStop(bool finish) { OnStop.Broadcast(finish); }

//-----------------------------------

void UMidiComponent::start() {
	mProcessor.start();
}

void UMidiComponent::stop() {
	mProcessor.stop();
}

void UMidiComponent::reset() {
	mProcessor.reset();
}

bool UMidiComponent::isStarted() {
	return mProcessor.isStarted();
}
bool UMidiComponent::isRunning() {
	return mProcessor.isRunning();
}