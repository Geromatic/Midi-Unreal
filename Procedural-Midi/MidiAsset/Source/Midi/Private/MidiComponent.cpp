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

void UMidiComponent::onEvent(MidiEvent* _event, int track) {
	if (_event->getType() == (ChannelEvent::NOTE_ON & 0x0F)) {
		NoteOn* note = static_cast<NoteOn*>(_event);
		OnEvent.Broadcast(track, note->getNoteValue(), note->getVelocity(), _event->getTick());
	}

	else if (_event->getType() == (ChannelEvent::NOTE_OFF & 0x0F)) {
		NoteOff* note = static_cast<NoteOff*>(_event);
		OnEvent.Broadcast(track, note->getNoteValue(), 0, _event->getTick());
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