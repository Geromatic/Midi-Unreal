# Midi-Unreal
Midi for Unreal Engine

MarketPlace: https://www.unrealengine.com/marketplace/procedural-midi 
<br>Example Project: Updated 2/27/2017
</br>

This code provides an interface to read, manipulate, and write MIDI files. "Playback" is supported as a real-time event dispatch system.(Leffelman)

Original Credit goes to Alex Leffelman who created the Android Midi Lib - https://github.com/leffelmania/android-midi-lib
Modified for Unreal Engine: Scott Bishel

http://groundsthirteen.webs.com/midi.htm - libraries for c++/c#/obj-c

Features:

- Ability to import [drag & drop] Midi files in Unreal
- Load/Play Midi
- Built in Midi Device Interface 
- Choice between MIDI output device (ex. computer, synthesizer, etc) or generated audio playback [**]
- Ability to connect external MIDI input device (ex. Midi Keyboard, Midi Fighter 64, any MIDI-equipped hardware) [**]

[Sending MIDI to/from another program or VST] is possible with a virtual MIDI driver.
<a href="https://www.tobias-erichsen.de/software/loopmidi.html">LoopMidi</a>
</br>
--By sending midi data to the virtual midi device...you can have other apps be able to  retrieve that data
</br>
-You can interact with the midi device by using the Midi Interface Component
</br>

** <b>[Shown in Demo Project on marketplace]</b>

Plugin Supports:

- Windows(32/64): [Packaged+Tested+Works]
- HTML5: [Packaged+Tested+Works but no sound]
- Android(All): [Packaged+Untested]
- Linus: [Packaged+Tested+Works]
- Mac/IOS: [Packaged+Tested+Works] 

### Installation \[Github Version Only / C++ Project Required\] 

<b>[Caution: careful when you have marketplace  and c++ plugins on same UE version as they may conflict with eachother]</b>

Place MidiAsset folder in your Plugins folder in youe Unreal project

###Showcase: [Blueprint]

![ScreenShot](https://github.com/Geromatic/Midi-Unreal/blob/master/Feature/Midi_Screenshot_Overview.PNG)
![ScreenShot](https://github.com/Geromatic/Midi-Unreal/blob/master/Feature/Midi_Screenshot_Preview.png)
![ScreenShot](https://github.com/Geromatic/Midi-Unreal/blob/master/Feature/Midi_Screenshot_Detail.PNG)
![ScreenShot](https://github.com/Geromatic/Midi-Unreal/blob/master/Feature/Midi_Screenshot_Info.PNG)
![ScreenShot](https://github.com/Geromatic/Midi-Unreal/blob/master/Feature/Midi_Screenshot_Library.PNG)

###Example Usage: [Beta-Untested/C++]
----
#### Reading and Writing a MIDI file:
```java
FString path;
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
MidiTrack& track = *midi.getTracks()[1];

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
	MidiTrack& track = *midi.getTracks()[0];
	TArray<MidiEvent*>::TIterator it = track.getEvents().CreateIterator();

	while (it)
	{
		MidiEvent* _event = *it;

		if (_event->getType() == MetaEvent::TEMPO)
		{
			Tempo* tempoEvent = (Tempo*)_event;
			tempoEvent->setBpm(tempoEvent->getBpm() / 2);
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

tempoTrack.insertEvent(&ts);
tempoTrack.insertEvent(&tempo);

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
TArray<MidiTrack*>& tracks = *new TArray<MidiTrack*>();
tracks.Add(&tempoTrack);
tracks.Add(&noteTrack);

MidiFile& midi = *new MidiFile(MidiFile::DEFAULT_RESOLUTION, tracks);

// 4. Write the MIDI data to a file
TArray<uint8> data;
FMemoryWriter writer(data);
midi.writeToFile(writer);
```

#### Listening for and processing MIDI events
```java
// Create a new MidiProcessor:
MidiProcessor processor();
processor.load(midi);

// Register for the events you're interested in:
EventPrinter ep;
processor.setListener(&ep);

// Start the processor:
processor.start();
```
```java
// This class will print any event it receives to the console
class EventPrinter: public MidiEventListener
{

public:
    EventPrinter()
    {
    }

    void onStart(bool fromBeginning)
    {
        if(fromBeginning)
        {
	        UE_LOG(LogTemp, Display, TEXT("Started!"));
        }
        else
        {
	        UE_LOG(LogTemp, Display, TEXT("Resumed!"));
        }
    }

    void onEvent(MidiEvent* _event, int track)
    {
        //System.out.println(mLabel + " received event: " + _event);
    }

    void onStop(bool finished)
    {
        if(finished)
        {
	        UE_LOG(LogTemp, Display, TEXT("Finished!"));
        }
        else
        {
	        UE_LOG(LogTemp, Display, TEXT("Paused!"));
        }
    }
};
```
