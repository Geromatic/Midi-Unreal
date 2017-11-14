// RtMidi : Copyright (c) 2003-2016 Gary P. Scavone

#include "MidiInterfaceComponent.h"
#include "MidiInterfacePrivatePCH.h"

#include <vector>


void mycallback(double deltatime, std::vector< unsigned char > *message, void *userData)
{
	UMidiInterfaceComponent* component = (UMidiInterfaceComponent*)userData;
	if (UMidiInterfaceComponent::queueCallbacks)
	{
		component->postCallback(deltatime, message);
	}
	else
	{
		component->handleCallback(deltatime, message);
	}
}

// Sets default values
UMidiInterfaceComponent::UMidiInterfaceComponent()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	
	PrimaryComponentTick.bCanEverTick = queueCallbacks;
	inSysEx = false;
	
}

// Called when the game starts or when spawned
void UMidiInterfaceComponent::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void UMidiInterfaceComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	while (!messageQueue.IsEmpty())
	{
		CallbackMessage message;
		messageQueue.Dequeue(message);
		handleCallback(message.deltaTime, &message.message);
	}

}
void UMidiInterfaceComponent::postCallback(double deltatime, std::vector< unsigned char > *message)
{
	CallbackMessage cbMessage{ static_cast<float>(deltatime), *message };

	messageQueue.Enqueue(cbMessage);

}
void UMidiInterfaceComponent::handleCallback(double deltatime, std::vector< unsigned char > *message)
{
	size_t nBytes = message->size();
	for (size_t i = 0; i < nBytes; )
	{
		uint8 id = message->at(i++);
		uint8 type = id >> 4;
		uint8 channelOrSubtype = id & 0x0F;

		// system message (top four bits 1111)
		if (type == 0xF)
		{
			// sysex start?
			if(channelOrSubtype == 0) {
				startSysEx();
				continue;
			}
			// sysex end?
			else if(channelOrSubtype == 7) {
				stopSysEx(deltatime);
				continue;
			}
			// MIDI Clock Events
			else {
				FMidiClockEvent Event;
				Event.Type = (EMidiClockTypeEnum)channelOrSubtype;

				// song position pointer
				if (channelOrSubtype == 2) {
					// LSB
					uint8 lsb = message->at(i++) & 0XFF;

					//MSB
					Event.Data = message->at(i++) & 0XFF;
					Event.Data = Event.Data << 7;
					Event.Data += lsb;
				}
				OnReceiveClockEvent.Broadcast(Event, deltatime);
			}
		}
		// if in the middle of sysex, pass it on to sysex buffer
		else if (getInSysEx())
		{
			appendSysEx(id);
		}
		// check if it is a channel message
		else if (type >= 0x8 && type <= 0xE)
		{
			FMidiEvent Event;

			Event.Type = (EMidiTypeEnum)(type & 0X0F);
			Event.Channel = channelOrSubtype & 0X0F;
			Event.Data1 = message->at(i++) & 0XFF;

			// check for program change or CHANNEL_AFTERTOUCH
			if (type != 0xC && type != 0xD) {
				Event.Data2 = message->at(i++) & 0XFF;
			}

			OnReceiveEvent.Broadcast(Event, deltatime);
		}
	}
}

bool UMidiInterfaceComponent::OpenInput(uint8 port)
{
	// Check available ports.
	unsigned int nPorts = midiIn.getPortCount();
	if (nPorts == 0 || port >= nPorts) {
		UE_LOG(LogTemp, Display, TEXT("No ports available!"));
		return false;
	}

	if (midiIn.isPortOpen()) {
		UE_LOG(LogTemp, Warning, TEXT("Port already opened!"));
		return false;
	}

	midiIn.openPort(port);

	// Set our callback function.  This should be done immediately after
	// opening the port to avoid having incoming messages written to the
	// queue.
	midiIn.setCallback(&mycallback, this);

	// Don't ignore sysex, timing, or active sensing messages.
	midiIn.ignoreTypes(false, false, true);

	return true;
}

bool UMidiInterfaceComponent::OpenOutput(uint8 port)
{
	// Check available ports.
	unsigned int nPorts = midiOut.getPortCount();
	if (nPorts == 0 || port >= nPorts) {
		UE_LOG(LogTemp, Display, TEXT("No ports available!"));
		return false;
	}

	if (midiOut.isPortOpen()) {
		UE_LOG(LogTemp, Warning, TEXT("Port already opened!"));
		return false;
	}

	midiOut.openPort(port);

	return true;
}

void UMidiInterfaceComponent::CloseInput()
{
	midiIn.closePort();
}

void UMidiInterfaceComponent::CloseOutput()
{
	midiOut.closePort();
}

void UMidiInterfaceComponent::Send(const FMidiEvent& Event)
{
	std::vector<uint8> msg;
	uint8 status = ((uint8)Event.Type << 4) | Event.Channel;
	msg.push_back(status);
	msg.push_back(Event.Data1);
	// check for program change or CHANNEL_AFTERTOUCH
	if (Event.Type != EMidiTypeEnum::MTE_PROGRAM_CHANGE && Event.Type != EMidiTypeEnum::MTE_CHANNEL_AFTERTOUCH) {
		msg.push_back(Event.Data2);
	}
	midiOut.sendMessage(&msg);
}
void UMidiInterfaceComponent::SendRaw(const TArray<uint8>& Data)
{
	std::vector<uint8> msg;

	for (int i = 0; i < Data.Num(); i++)
	{
		msg.push_back(Data[i]);
	}
	
	midiOut.sendMessage(&msg);
}
void UMidiInterfaceComponent::startSysEx()
{
	if (getInSysEx())
	{
		UE_LOG(LogTemp, Display, TEXT("Already in sysex and a new sysex start received - throwing away the last one!"));
	}
	sysExArray.Empty();
	setInSysEx(true);
}

void UMidiInterfaceComponent::stopSysEx(float deltaTime)
{
	if (!getInSysEx())
	{
		UE_LOG(LogTemp, Display, TEXT("Not in sysex and a sysex end received - ignoring!"));
	}
	else
	{
		setInSysEx(false);
		// send sysex to BP here
		OnReceiveSysExEvent.Broadcast(sysExArray, deltaTime);
	}
}
void UMidiInterfaceComponent::appendSysEx(int data)
{
	sysExArray.Add(data);
}
