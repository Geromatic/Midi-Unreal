// Copyright -> Scott Bishel

#include "MidiInterfaceLibrary.h"
#include "MidiInterfacePrivatePCH.h"

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
	if (nPorts < 1 || port >= nPorts) {
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

	return midiIn.isPortOpen();
}

bool UMidiInterface::OpenMidiOutput(uint8 port)
{
	// Check available ports.
	unsigned int nPorts = midiOut.getPortCount();
	if (nPorts < 1 || port >= nPorts) {
		UE_LOG(LogTemp, Display, TEXT("No ports available!"));
		return false;
	}

	if (midiOut.isPortOpen()) {
		UE_LOG(LogTemp, Warning, TEXT("Close Port Before opening another!"));
		return false;
	}

	midiOut.openPort(port);

	return midiOut.isPortOpen();
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
	uint8 status = ((uint8)Event.Type << 4) | Event.Channel;
	uint8 data[3] = { status, Event.Data1, Event.Data2 };

	// check for program change or CHANNEL_AFTERTOUCH
	if (Event.Type != EMidiTypeEnum::MTE_PROGRAM_CHANGE && Event.Type != EMidiTypeEnum::MTE_CHANNEL_AFTERTOUCH) {
		midiOut.sendMessage(&data[0], 3);
	}
	else
		midiOut.sendMessage(&data[0], 2);
}

void UMidiInterface::SendMidiRaw(const TArray<uint8>& Data)
{
	midiOut.sendMessage(Data.GetData(), Data.Num());
}

void UMidiInterface::GetMidiDevices(TArray<FMidiDevice>& Input, TArray<FMidiDevice>& Output) {

	int size = midiIn.getPortCount();
	for (int i = 0; i < size; i++) {
			FString name = FString(midiIn.getPortName(i).c_str());
			uint8 port = i;
			Input.Add(FMidiDevice(name, port));
	}

	size = midiOut.getPortCount();
	for (int i = 0; i < size; i++) {
		FString name = FString(midiOut.getPortName(i).c_str());
		uint8 port = i;
		Output.Add(FMidiDevice(name, port));
	}
}
