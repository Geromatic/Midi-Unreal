/// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#pragma once

class FRunnable;
class FRunnableThread;
class MidiProcessor;

//~~~~~ Multi Threading ~~~
class MIDI_API FMidiProcessorWorker : public FRunnable
{
	/** Thread to run the worker FRunnable on */
	FRunnableThread* Thread;

	/** The PC */
	MidiProcessor* ThePC;

	bool isGameTime;

public:

	//Done?
	bool IsFinished() const;

	//~~~ Thread Core Functions ~~~

	//Constructor / Destructor
	FMidiProcessorWorker(MidiProcessor* IN_PC, bool UseGameTime);
	virtual ~FMidiProcessorWorker();

	// Begin FRunnable interface.
	virtual bool Init();
	virtual uint32 Run();
	virtual void Stop();

	// End FRunnable interface
};