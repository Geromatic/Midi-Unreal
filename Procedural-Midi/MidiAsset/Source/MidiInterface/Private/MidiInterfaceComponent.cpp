// RtMidi : Copyright (c) 2003-2016 Gary P. Scavone

#include "MidiInterfacePrivatePCH.h"
#include "MidiInterfaceComponent.h"

#include <vector>


void mycallback(double deltatime, std::vector< unsigned char > *message, void *userData)
{
	UMidiInterfaceComponent* component = (UMidiInterfaceComponent*)userData;
	if (message == NULL)
		return;

	unsigned int nBytes = message->size();
	for (unsigned int i = 0; i < nBytes; ) {
		int id = message->at(i++);
		int type = id >> 4;
		int channel = id & 0x0F;

		// check if it is a channel message
		if (type >= 0x8 && type <= 0xE) {
			FMidiEvent Event;
			Event.Type = (EMidiTypeEnum)type;
			Event.Channel = channel;
			Event.Data1 = message->at(i++);
			// Running Status Byte
			if (type == 0x8) {
				Event.Type = (EMidiTypeEnum)0x9;
				Event.Data2 = 0;
			}
			// check for program change or CHANNEL_AFTERTOUCH
			else if (!(type == 0xC || type == 0xD)) {
				Event.Data2 = message->at(i++);
			}
			component->OnReceiveEvent.Broadcast(Event, deltatime);
		}
	}
}

// Sets default values
UMidiInterfaceComponent::UMidiInterfaceComponent()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryComponentTick.bCanEverTick = false;
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
//	midiIn.ignoreTypes(false, false, false);

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
	msg.push_back(Event.Data2);
	midiOut.sendMessage(&msg);
}
