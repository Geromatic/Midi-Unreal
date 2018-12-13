// Copyright -> Scott Bishel

#include "MidiComponent.h"
#include "MidiPrivatePCH.h"

// Unreal Engine Files
#include "Engine/Engine.h"
#include "Misc/FileHelper.h"

#include "MidiStruct.h"
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

#include "Event/Meta/TextualMetaEvent.h"
#include "Event/Meta/SequencerSpecificEvent.h"
#include "Event/Meta/TimeSignature.h"
#include "Event/Meta/EndOfTrack.h"
#include "Event/Meta/GenericMetaEvent.h"
#include "Event/Meta/KeySignature.h"
#include "Event/Meta/MidiChannelPrefix.h"
#include "Event/Meta/SequenceNumber.h"
#include "Event/Meta/SequencerSpecificEvent.h"
#include "Event/Meta/SmpteOffset.h"

#include "MidiAsset.h"

#include "Util/MetronomeTick.h"
#include "Util/MidiProcessor.h"

#include "MML/LabMidiSong.h"
#include "Thread/MidiThread.h"

#include <algorithm>    // std::sort

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

	if (mProcessor.PlayRate != PlaySpeed)
		mProcessor.PlayRate = PlaySpeed;

	if (!InBackground) {
		// update time
		if (isGameTime) {
			mProcessor.update(GetWorld()->TimeSeconds * 1000.0f);
		}
		else
			mProcessor.update(FPlatformTime::Cycles());
	}

	else
		while (!mQueue.IsEmpty()) {
			MidiCallbackMessage _midiEvent;
			mQueue.Dequeue(_midiEvent);
			handleCallback(_midiEvent.Event, _midiEvent.ms, _midiEvent.trackID);
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

struct membuf : std::streambuf
{
	membuf(char* begin, char* end) {
		this->setg(begin, begin, end);
	}
};

void UMidiComponent::LoadAsset(UMidiAsset* MidiAsset) {
	if (!canInit()) return;
	if (!MidiAsset) return;

	const TArray<uint8>& data = MidiAsset->Data;
	if (data.Num() == 0)
		return;

	char* ptr = (char*)data.GetData();

	membuf sbuf(ptr, ptr + data.Num());
	std::istream in(&sbuf);

	mMidiFile = new MidiFile(in);
	mProcessor.load(*mMidiFile);
}

void UMidiComponent::LoadFile(FString path) {
	if (!canInit()) return;

	TArray<uint8> data;
	bool result = FFileHelper::LoadFileToArray(data, path.GetCharArray().GetData());
	if (result == 0 || data.Num() == 0)
		return;

	char* ptr = (char*)data.GetData();

	membuf sbuf(ptr, ptr + data.Num());
	std::istream in(&sbuf);

	mMidiFile = new MidiFile(in);
	mProcessor.load(*mMidiFile);
}

void UMidiComponent::LoadMML(FString path) {
	if (!canInit()) return;
	std::string MyStdString(TCHAR_TO_UTF8(*path));
	
	mMidiFile = new MidiFile();
	// Load MML
	Lab::MidiSong song;
	song.channel = 0;
	song.LoadString(MyStdString);
	mMidiFile->addTrack(song.track);

	mProcessor.load(*mMidiFile);
}

void UMidiComponent::onEvent(MidiEvent* _event, long ms) {
	if (InBackground) {
		MidiCallbackMessage msg = { _event, ms, mProcessor._trackID };
		mQueue.Enqueue(msg);
	}
	else
		handleCallback(_event, ms, mProcessor._trackID);
}

void UMidiComponent::handleCallback(MidiEvent* _event, long ms, int trackID)
{
	int midi_type = _event->getType();

	// Channel Event
	if (midi_type >= ChannelEvent::NOTE_OFF &&
		midi_type <= ChannelEvent::PITCH_BEND) 
	{
		ChannelEvent* channelEvent = static_cast<ChannelEvent*>(_event);
		FMidiEvent _midiEvent;

		_midiEvent.Type = static_cast<EMidiTypeEnum>(midi_type);
		_midiEvent.Channel = channelEvent->getChannel();
		_midiEvent.Data1 = channelEvent->getValue1();
		_midiEvent.Data2 = channelEvent->getValue2();

		if (SimplifyNote) {
			// Running Status Event [Improved Midi Performance]
			if (midi_type == ChannelEvent::NOTE_OFF) {
				_midiEvent.Type = static_cast<EMidiTypeEnum>(ChannelEvent::NOTE_ON);
				_midiEvent.Data2 = 0;
			}
		}
		OnMidiEvent.Broadcast(_midiEvent, ms, trackID);
	}
	// System Exclusive
	else if (midi_type == 0xF0 || midi_type == 0xF7)
	{
		if (!OnSysExEvent.IsBound())
			return;

		SystemExclusiveEvent* sysExEvent = static_cast<SystemExclusiveEvent*>(_event);
		TArray<uint8> data;
		string * ptr = sysExEvent->getData();

		// multi-packet event
		bool isDivided = false;

		// Add 0xF0 SysEX Start
		if(midi_type == 0xF0)
			data.Add(midi_type);
		// multi-packet check
		else if (midi_type == 0xF7 && ptr->size() > 3) {
			data.Add((uint8)0xF0);
			isDivided = true;
		}

		// Add <sysex_dat>
		data.Append((uint8*)ptr->c_str(), ptr->size());

		// add 0xF7 SysEx End on Divided
		if (isDivided) {
			// close message
			if(data[data.Num() - 1] != 0xF7)
				data.Add((uint8)0xF7);
		}

		OnSysExEvent.Broadcast(data, ms, trackID);
	}
	// Textual Meta Event
	else if (midi_type >= MetaEvent::TEXT_EVENT && 
			 midi_type <= MetaEvent::CUE_POINT) 
	{
		if (!OnTextEvent.IsBound())
			return;
		TextualMetaEvent* textExEvent = static_cast<TextualMetaEvent*>(_event);
		FString text = UTF8_TO_TCHAR(textExEvent->getText().c_str());
		OnTextEvent.Broadcast(static_cast<EMidiTextTypeEnum>(midi_type), text, ms, trackID);
	}
	// Metronome
	else if (midi_type == MetronomeTick::TYPE) 
	{
		if (!OnMetronomeTick.IsBound())
			return;
		MetronomeTick* ev = static_cast<MetronomeTick*>(_event);
		OnMetronomeTick.Broadcast(ev->getBeatNumber(), ev->getMeasure(), ms);
	}
}

void UMidiComponent::onStart(bool fromBeginning) { 
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
		this->isGameTime = UseGameTime;


		if (UseGameTime) {
			mProcessor.milliFunction = NULL;
			if (!InBackground)
				mProcessor.start(GetWorld()->TimeSeconds * 1000.0f);
		}
		else {
			mProcessor.milliFunction = FPlatformTime::ToMilliseconds;
			if (!InBackground)
				mProcessor.start(FPlatformTime::Cycles());
		}

		// MultiThread
		if (InBackground) {
			mWorker = new FMidiProcessorWorker(&mProcessor, this->isGameTime);
		}
	}
}

void UMidiComponent::stop() {
	mProcessor.stop();
}

void UMidiComponent::reset() {
	mProcessor.stop();
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

// sort MIDI predicate
inline static bool _ConstPredicate(const MidiEvent* ip1, const MidiEvent* ip2)
{
	int value = ((MidiEvent*)ip1)->compareTo((MidiEvent*)ip2);

	// somehow value should be less then else it flips the MIDI file
	return value < 0;
}

float UMidiComponent::GetDuration()
{

	if (mMidiFile)
	{
		vector<std::vector<MidiEvent*>::iterator > mCurrEvents;
		vector<std::vector<MidiEvent*>::iterator > mCurrEventsEnd;

		vector<MidiTrack*>& tracks = mMidiFile->getTracks();
		for (int i = 0; i < (int)tracks.size(); i++) {
			mCurrEvents.push_back(tracks[i]->getEvents().begin());
			mCurrEventsEnd.push_back(tracks[i]->getEvents().end());
		}

		double msElapsed = 0;
		const int& mPPQ = mMidiFile->getResolution();
		int mMPQN = Tempo::DEFAULT_MPQN;

		vector<Tempo*> tempoTicks;
		for (int i = 0; i < (int)mCurrEvents.size(); i++) {
			while (mCurrEvents[i] != mCurrEventsEnd[i]) {
				MidiEvent * _event = *mCurrEvents[i];
				// Tempo and Time Signature events are always needed by the processor
				if (_event->getType() == MetaEvent::TEMPO) {
					tempoTicks.push_back(static_cast<Tempo*>(_event));
				}
				++mCurrEvents[i];
			}
		}
		std::sort(tempoTicks.begin(), tempoTicks.end(), _ConstPredicate);

		double ticksElapsed = 0;
		for (int i = 0; i < (int)tempoTicks.size(); i++) {
			Tempo * tempo = tempoTicks[i];

			if (i > 0)
				ticksElapsed = tempoTicks[i - 1]->getTick();

			msElapsed += MidiUtil::ticksToMs(tempo->getTick() - ticksElapsed, mMPQN, mPPQ);
			mMPQN = tempo->getMpqn();
		}

		if (tempoTicks.size() > 0)
			ticksElapsed = tempoTicks[(int)tempoTicks.size() - 1]->getTick();

		msElapsed += MidiUtil::ticksToMs(mMidiFile->getLengthInTicks() - ticksElapsed, mMPQN, mPPQ);

		return  msElapsed / 1000.0;
	}
	else
	{
		return 0;
	}
}
