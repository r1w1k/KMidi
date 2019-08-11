#ifndef _KMIDI_H_
#define _KMIDI_H_

#define NOTE_ON 0b10010000
#define NOTE_OFF 0b10000000
#define CHANNEL_MODE 0b10110000
#define ALL_OFF 0b01111011
#define KEYBOARD "nanoKEY2 KEYBOARD"
#define KMIDI_OUTPUT "IAC Driver KMidi"

#include <map>
#include "RtMidi.h"
#include "Phrase.h"

namespace KMidi{
	extern std::map<int, std::string> note_names;
	extern std::map<int, std::string> note_names_flats;
	std::string note_name(int decimal, bool use_flats = true);
}

class MidiIn{
public:
	RtMidiIn *in;
	bool open = false;
	std::string id;

	std::vector<std::vector<Note>> get_voices();
	void clear_queue();

	MidiIn();
	~MidiIn();
};

class MidiOut{
public:
	RtMidiOut *out;
	int channel;
	std::string id;

	void sleep(const int &ms);
	void play(Phrase *p);
	void note_on(Note n);
	void note_off(Note n);

	void all_notes_off();

	MidiOut(int c=0);
	~MidiOut();
};

#endif