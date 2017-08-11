// Copyright -> Scott Bishel

#include "MidiPrivatePCH.h"
#include "MidiComponent.h"

#include "MidiUtils.h"
#include "MidiFile.h"
#include "Util/MidiUtil.h"

#include "Event/ChannelAftertouch.h"
#include "Event/ChannelEvent.h"
#include "Event/Controller.h"
#include "Event/NoteAfterTouch.h"
#include "Event/NoteOff.h"
#include "Event/NoteOn.h"
#include "Event/PitchBend.h"
#include "Event/ProgramChange.h"
#include "Event/SystemExclusiveEvent.h"
#include "Event/Meta/Tempo.h"

#include "MidiAsset.h"

#include "Util/MidiProcessor.h"

#include "MML/LabMidiSong.h"
#include "Thread/MidiThread.h"

// Sets default values for this component's properties
UMidiComponent::UMidiComponent() : PlaySpeed(1.0), mWorker(NULL)
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
	mProcessor.stop();

	// make sure thread is deleted
	if (mWorker)
		delete mWorker;
	mWorker = NULL;

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

	if (mProcessor.PlaySpeed != PlaySpeed)
		mProcessor.PlaySpeed = PlaySpeed;

	if (!InBackground) {
		// update time
		if (mProcessor.isGameTime) {
			mProcessor.update(GetWorld()->TimeSeconds * 1000.0f);
		}
		else
			mProcessor.update();
	}

	else
		while (!mQueue.IsEmpty()) {
			FMidiEvent _midiEvent;
			mQueue.Dequeue(_midiEvent);
			OnMidiEvent.Broadcast(_midiEvent);
		}
}

bool UMidiComponent::canInit() {
	if (isRunning()) {
		UE_LOG(LogTemp, Warning, TEXT("Unable to load MIDI while another is playing"));
		return false;
	}

	if (mMidiFile)
		delete mMidiFile;
	mMidiFile = NULL;

	return true;
}

void UMidiComponent::LoadAsset(UMidiAsset* MidiAsset) {
	if (!canInit()) return;
	if (!MidiAsset) return;

	const TArray<uint8>& data = MidiAsset->Data;
	if (data.Num() == 0)
		return;

	FBufferReader reader((uint8*)data.GetData(), data.Num(), false);
	mMidiFile = new MidiFile(reader);
	mProcessor.load(*mMidiFile);
}

void UMidiComponent::LoadFile(FString path) {
	if (!canInit()) return;

	TArray<uint8> data;
	bool result = FFileHelper::LoadFileToArray(data, path.GetCharArray().GetData());
	if (result == 0 || data.Num() == 0)
		return;

	FBufferReader reader((uint8*)data.GetData(), data.Num(), false);
	mMidiFile = new MidiFile(reader);
	mProcessor.load(*mMidiFile);
}

void UMidiComponent::LoadMML(FString path) {
	if (!canInit()) return;

	char* a = new char[path.Len()];
	for (int i = 0; i < path.Len(); i++)
		a[i] = path[i];

	// TODO figure out better method of parsing MML
	//mMidiFile = Lab::MidiSong::parseMML(a, (int)path.Len(), false);
	//delete[] a;
	//mProcessor.load(*mMidiFile);

	Lab::MidiSong song;
	song.trackNumber = 0;
	song.LoadString(path);
	mMidiFile = new MidiFile();
	mMidiFile->addTrack(song.track);
	mProcessor.load(*mMidiFile);
}

void UMidiComponent::onEvent(MidiEvent* _event) {
	// Channel Event
	if (_event->getType() >= ChannelEvent::NOTE_OFF && _event->getType() <= ChannelEvent::PITCH_BEND) {
		ChannelEvent* channelEvent = static_cast<ChannelEvent*>(_event);
		FMidiEvent _midiEvent;

		_midiEvent.Type = static_cast<EMidiTypeEnum>(_event->getType() & 0X0F);
		_midiEvent.Channel = channelEvent->getChannel() & 0x0F;
		_midiEvent.Data1 = channelEvent->getValue1() & 0xFF;
		_midiEvent.Data2 = channelEvent->getValue2() & 0xFF;
		
		if (SimplifyNote) {
			// Running Status Event [Improved Midi Performance]
			if (_event->getType() == ChannelEvent::NOTE_OFF) {
				_midiEvent.Type = static_cast<EMidiTypeEnum>(ChannelEvent::NOTE_ON & 0X0F);
				_midiEvent.Data2 = 0 & 0XFF;
			}
		}
		if(InBackground)
			mQueue.Enqueue(_midiEvent);
		else
			OnMidiEvent.Broadcast(_midiEvent);
	}
}

void UMidiComponent::onStart(bool fromBeginning) { 
	// MultiThread
	if (InBackground) {
		mWorker = new FMidiProcessorWorker(&mProcessor);
	}
	else
		if (mProcessor.isGameTime)
			mProcessor.setBeginTime(GetWorld()->TimeSeconds * 1000.0f);

	OnStart.Broadcast(fromBeginning); 
}
void UMidiComponent::onStop(bool finish) { 
	// MultiThread
	if (mWorker) {
		mWorker->Stop();
		delete mWorker;
	}
	mWorker = NULL;
	mQueue.Empty();
	
	OnStop.Broadcast(finish); 
}

//-----------------------------------

void UMidiComponent::start(bool background, bool UseGameTime) {
	if (!isRunning()) {
		InBackground = background;
		mProcessor.isGameTime = UseGameTime;
	}


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

float UMidiComponent::GetDuration()
{
	// TODO find a better solution
	if (mMidiFile)
	{

		TArray<TArray<MidiEvent*>::TIterator> mCurrEvents;
		TArray<MidiTrack*>& tracks = mMidiFile->getTracks();
		for (int i = 0; i < tracks.Num(); i++) {
			mCurrEvents.Add(tracks[i]->getEvents().CreateIterator());
		}

		double mMsElapsed = 0;
		const int& mPPQ = mMidiFile->getResolution();
		int mMPQN = Tempo::DEFAULT_MPQN;
		double mTicksElapsed = 0;

		while (true) {

			const double msElapsed = 1.0;
			double ticksElapsed = (((msElapsed * 1000.0) * mPPQ) / mMPQN) * mProcessor.PlaySpeed;

			mMsElapsed += msElapsed;
			mTicksElapsed += ticksElapsed;

			for (int i = 0; i < mCurrEvents.Num(); i++) {
				while (mCurrEvents[i]) {
					MidiEvent * _event = *mCurrEvents[i];
					if (_event->getTick() <= mTicksElapsed) {
						// Tempo and Time Signature events are always needed by the processor
						if (_event->getType() == MetaEvent::TEMPO) {
							mMPQN = (static_cast<Tempo*>(_event))->getMpqn();
						}
						++mCurrEvents[i];
					}
					else
						break;
				}
			}
			
			bool more = false;
			for (int i = 0; i < mCurrEvents.Num(); i++) {
				if (mCurrEvents[i])
				{
					more = true; 
					break;
				}
			}

			if (more == false)
				break;
		}

		return  mMsElapsed / 1000.0f;
	}
	else
	{
		return 0;
	}
}
