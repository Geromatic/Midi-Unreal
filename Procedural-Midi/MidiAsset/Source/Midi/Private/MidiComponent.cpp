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

#include "MidiAsset.h"

#include "Util/MidiProcessor.h"

// Sets default values for this component's properties
UMidiComponent::UMidiComponent() : PlaySpeed(1.0f)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.000f;
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

	if(mProcessor.PlaySpeed != PlaySpeed)
		mProcessor.PlaySpeed = PlaySpeed;
	
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
	if (_event->getType() >= ChannelEvent::NOTE_OFF && _event->getType() <= ChannelEvent::PITCH_BEND) {
		ChannelEvent* channelEvent = static_cast<ChannelEvent*>(_event);
		FMidiEvent _midiEvent;
		_midiEvent.Channel = channelEvent->getChannel();
		_midiEvent.Data1 = channelEvent->getValue1();
		
		// Running Status Event [Improved Midi Performance]
		if (_event->getType() == ChannelEvent::NOTE_OFF) {
			_midiEvent.Type = static_cast<EMidiTypeEnum>(ChannelEvent::NOTE_ON);
			_midiEvent.Data2 = 0;
		}
		else {
			_midiEvent.Type = static_cast<EMidiTypeEnum>(_event->getType());
			_midiEvent.Data2 = channelEvent->getValue2();
		}
		OnMidiEvent.Broadcast(_midiEvent);
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

int UMidiComponent::GetResolution()
{
	if (mMidiFile)
	{
		return mMidiFile->getResolution();
	}
	else
	{
		return 0;
	}
}