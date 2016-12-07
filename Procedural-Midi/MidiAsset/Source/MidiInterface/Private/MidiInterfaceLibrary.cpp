// Copyright -> Scott Bishel

#include "MidiInterfacePrivatePCH.h"
#include "MidiInterfaceLibrary.h"

void midiMessegeCallback(double deltatime, std::vector< unsigned char > *message, void *userData)
{
	if (message == NULL)
		return;

	RtMidiOut * midiOut = (RtMidiOut*)userData;
	midiOut->sendMessage(message);
}

RtMidiIn UMidiInterface::midiIn;
RtMidiOut UMidiInterface::midiOut;

bool UMidiInterface::OpenMidiInput(uint8 port)
{
	// Check available ports.
	unsigned int nPorts = midiIn.getPortCount();
	if (nPorts == 0 || port >= nPorts) {
		UE_LOG(LogTemp, Display, TEXT("No ports available!"));
		return false;
	}

	if (midiIn.isPortOpen()) {
		UE_LOG(LogTemp, Warning, TEXT("Close Port Before opening another!"));
		return false;
	}

	midiIn.openPort(port);

	// Set our callback function.  This should be done immediately after
	// opening the port to avoid having incoming messages written to the
	// queue.
	midiIn.setCallback(&midiMessegeCallback, &midiOut);

	// Don't ignore sysex, timing, or active sensing messages.
//	midiIn.ignoreTypes(false, false, false);

	return true;
}

bool UMidiInterface::OpenMidiOutput(uint8 port)
{
	// Check available ports.
	unsigned int nPorts = midiOut.getPortCount();
	if (nPorts == 0 || port >= nPorts) {
		UE_LOG(LogTemp, Display, TEXT("No ports available!"));
		return false;
	}

	if (midiOut.isPortOpen()) {
		UE_LOG(LogTemp, Warning, TEXT("Close Port Before opening another!"));
		return false;
	}

	midiOut.openPort(port);

	return true;
}


void UMidiInterface::CloseMidiInput()
{
	midiIn.closePort();
}

void UMidiInterface::CloseMidiOutput()
{
	midiOut.closePort();
}

bool UMidiInterface::isOutputOpen()
{
	return midiOut.isPortOpen();
}

void UMidiInterface::SendMidiEvent(const FMidiEvent& Event)
{
	std::vector<uint8> msg;
	uint8 status = ((uint8)Event.Type << 4) | Event.Channel;
	msg.push_back(status);
	msg.push_back(Event.Data1);
	msg.push_back(Event.Data2);
	midiOut.sendMessage(&msg);
}