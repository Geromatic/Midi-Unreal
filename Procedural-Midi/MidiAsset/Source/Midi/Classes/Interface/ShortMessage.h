#include "../Event/ChannelEvent.h"

/**
* 
*/
class MIDI_API ShortMessage : public ChannelEvent {

public:
	ShortMessage(): ChannelEvent(0,0,0,0,0) {

	}
	
	char getChannel() {
		return mChannel;
	}
	unsigned char getStatus() {
		return (mType << 4) | mChannel;
	}
	char getData1() {
		return mValue1;
	}
	char getData2() {
		return mValue2;
	}
};