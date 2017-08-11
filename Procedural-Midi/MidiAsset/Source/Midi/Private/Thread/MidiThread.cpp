// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#include "MidiPrivatePCH.h"
#include "MidiThread.h"

#include "Util/MidiProcessor.h"

bool FMidiProcessorWorker::IsFinished() const
{
	return !ThePC->isRunning();
}

//Constructor / Destructor
FMidiProcessorWorker::FMidiProcessorWorker(MidiProcessor* IN_PC) 
	: ThePC(IN_PC)
{
	Thread = FRunnableThread::Create(this, TEXT("FMidiProcessorWorker"), 0, TPri_BelowNormal); //windows default = 8mb for thread, could specify more
}

FMidiProcessorWorker::~FMidiProcessorWorker() {
	delete Thread;
	Thread = NULL;
}

bool FMidiProcessorWorker::Init() {
	return true;
}
uint32 FMidiProcessorWorker::Run() {
	while (!IsFinished())
	{
		ThePC->process();
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		//prevent thread from using too many resources
		FPlatformProcess::Sleep(0.008f);
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	}
	return 0;
}
void FMidiProcessorWorker::Stop() {
	Thread->WaitForCompletion();
}