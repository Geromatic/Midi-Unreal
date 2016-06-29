# Midi-Unreal
Midi for Unreal Engine

Original Credit goes to Alex Leffelman who created the Android Midi Lib - https://github.com/leffelmania/android-midi-lib
Modified for Unreal Engine: Scott Bishel

http://groundsthirteen.webs.com/midi.htm - libraries for c++/c#/obj-c

Currently the project is compiled in Unreal 4.9.2. (Although a bit messy)

Plugin Available 

Donations appreciated @ www.paypal.me/Bishel

//-------------------------------------------

(reference taken from https://github.com/LeffelMania/android-midi-lib)

Example Usage:
----
#### Reading and Writing a MIDI file:
```java
TArray<uint8> data;
bool result = FFileHelper::LoadFileToArray(data, path.GetCharArray().GetData());
FBufferReader reader((uint8*)data.GetData(), data.Num(), false);
MidiFile midi(reader);

...

TArray<uint8> data;
FMemoryWriter writer(data);
midi.writeToFile(writer);
```

#### Manipulating a MIDI file's data:
Removing a track:
```java
midi.removeTrack(2);
```

Removing any event that is not a note from track 1:
```java
MidiTrack track = *getTracks()[1];

TArray<MidiEvent*>::TIterator it = track.getEvents().CreateIterator();
TArray<MidiEvent*> eventsToRemove;

while (it)
{
	MidiEvent* _event = *it;

	if (!(_event->getType() == ChannelEvent::NOTE_ON) && !(_event->getType() == ChannelEvent::NOTE_OFF))
	{
		eventsToRemove.Add(_event);
	}
	it++;
}
for (int i = 0; i < eventsToRemove.Num(); i++) {
	track.removeEvent(eventsToRemove[i]);
}
```

Reducing the tempo by half:
```java
	MidiTrack track = *getTracks()[0];
	TArray<MidiEvent*>::TIterator it = track.getEvents().CreateIterator();

	while (it)
	{
		MidiEvent* _event = *it;

		if (_event.getType() == MetaEvent::TEMPO)
		{
			Tempo& tempoEvent = (Tempo*)_event;
			tempoEvent.setBpm(tempo.getBpm() / 2);
		}
		it++;
	}
```

#### Composing a new MIDI file:
```java
// 1. Create some MidiTracks
MidiTrack& tempoTrack = *new MidiTrack();
MidiTrack& noteTrack = *new MidiTrack();

// 2. Add events to the tracks
// Track 0 is the tempo map
TimeSignature& ts = *new TimeSignature();
ts.setTimeSignature(4, 4, TimeSignature::DEFAULT_METER, TimeSignature::DEFAULT_DIVISION);

Tempo& tempo = *new Tempo();
tempo.setBpm(228);

tempoTrack.insertEvent(ts);
tempoTrack.insertEvent(tempo);

// Track 1 will have some notes in it
const int NOTE_COUNT = 80;

for(int i = 0; i < NOTE_COUNT; i++)
{
    int channel = 0;
    int pitch = 1 + i;
    int velocity = 100;
    long tick = i * 480;
    long duration = 120;
    
    noteTrack.insertNote(channel, pitch, velocity, tick, duration);
}

// 3. Create a MidiFile with the tracks we created
TArray<MidiTrack*> tracks = new TArray<MidiTrack*>();
tracks.add(tempoTrack);
tracks.add(noteTrack);

MidiFile midi = new MidiFile(MidiFile::DEFAULT_RESOLUTION, tracks);

// 4. Write the MIDI data to a file
TArray<uint8> data;
FMemoryWriter writer(data);
midi.writeToFile(writer);
```

#### Listening for and processing MIDI events
```java
// Create a new MidiProcessor:
MidiProcessor processor(midi);

// Register for the events you're interested in:
EventPrinter ep = new EventPrinter();
processor.setListener(&processor)

// Start the processor:
processor.start();
```
```java
// This class will print any event it receives to the console
public EventPrinter: public MidiEventListener
{

public:
    EventPrinter()
    {
    }

    void _onStart(bool fromBeginning)
    {
        if(fromBeginning)
        {
         //   System.out.println(mLabel + " Started!");
        }
        else
        {
         //   System.out.println(mLabel + " resumed");
        }
    }

    void _onEvent(MidiEvent* _event, int track)
    {
        //System.out.println(mLabel + " received event: " + _event);
    }

    void _onStop(bool finished)
    {
        if(finished)
        {
            //System.out.println(mLabel + " Finished!");
        }
        else
        {
            //System.out.println(mLabel + " paused");
        }
    }
}
```
