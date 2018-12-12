// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#include "MidiThread.h"
#include "MidiPrivatePCH.h"

#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"

#include "Util/MidiProcessor.h"

bool FMidiProcessorWorker::IsFinished() const
{
	return !ThePC->isRunning();
}

//Constructor / Destructor
FMidiProcessorWorker::FMidiProcessorWorker(MidiProcessor* IN_PC, bool UseGameTime)
	: ThePC(IN_PC)
{
	this->isGameTime = UseGameTime;

	Thread = FRunnableThread::Create(this, TEXT("FMidiProcessorWorker"), 0, EThreadPriority::TPri_BelowNormal); //windows default = 8mb for thread, could specify more
}

FMidiProcessorWorker::~FMidiProcessorWorker() {
	if(Thread)
		delete Thread;
	Thread = NULL;
}

bool FMidiProcessorWorker::Init() {
	return true;
}
uint32 FMidiProcessorWorker::Run() {
	UWorld* world = GEngine->GameViewport->GetWorld();
	if (!world)
		return 0;

	if (this->isGameTime) {
		ThePC->start(world->TimeSeconds * 1000.0f);
	}
	else {
		ThePC->start(FPlatformTime::Cycles());
	}

	while (!IsFinished())
	{
		if (isGameTime)
			ThePC->update(world->TimeSeconds * 1000.0f);
		else
			ThePC->update(FPlatformTime::Cycles());
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