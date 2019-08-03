#include <unistd.h>
#include "KMidi.h"
#include "RtMidi.h"
#include <iostream>

void MidiOut::sleep(const int &ms){
	usleep(ms*1000);
}

void MidiOut::play(Phrase *p, int repeat){
	std::cout << "Playing from " << id << std::endl;
	std::vector<unsigned char> message(3, '\0');
	for (int i = 0; i<repeat; i++){
		for (vector<Note> nVec : p->phrase){
			int length{0};
			for (Note n : nVec){
				if (n.velocity > 0){
					std::cout << "Note on!" << std::endl;
					note_on(n);
				}
				sleep(n.duration);		
			}
			if (!p->ringout){
				for (Note n : nVec){
					if (n.velocity > 0){
						note_off(n);	
					}
				}
			}
		}
	}
	all_notes_off();
}

MidiOut::MidiOut(int c){
	channel = c;
	out = new RtMidiOut();

	unsigned int ports = out->getPortCount();

	int output_port{-99};
	for (int i = 0; i < ports; i++){
		std::cout << out->getPortName(i) << std::endl;
		if (out->getPortName(i) == "IAC Driver Input port")
			output_port = i;
	}
	
	if (output_port >= 0){
		out->openPort(output_port);
		id = out->getPortName(output_port);
	}
	else
		std::cout << "IAC Driver not found - MIDI Out uninitialized" << std::endl;
}

MidiOut::~MidiOut(){
	delete out;
}

void MidiOut::note_on(Note n){
	std::vector<unsigned char> message(3, '\0');
	message.at(0) = NOTE_ON | channel;
	message.at(1) = n.pitch;
	message.at(2) = n.velocity;
	out->sendMessage(&message);
}
void MidiOut::note_off(Note n){
	std::vector<unsigned char> message(3, '\0');
	message.at(0) = NOTE_OFF | channel;
	message.at(1) = n.pitch;
	message.at(2) = n.velocity;
	out->sendMessage(&message);
}
void MidiOut::all_notes_off(){
	std::vector<unsigned char> message(3, '\0');
	message.at(0) = CHANNEL_MODE | channel;
	message.at(1) = ALL_OFF;
	message.at(2) = 0;
	out->sendMessage(&message);
}

MidiIn::MidiIn(){
	in = new RtMidiIn();
	unsigned int ports = in->getPortCount();

	int keyboard_port{-99};
	for (int i = 0; i < ports; i++){
		std::cout << "Port: " << in->getPortName(i) << std::endl;
		if (in->getPortName(i) == KEYBOARD)
			keyboard_port = i;
	}
	
	if (keyboard_port >= 0){
		in->openPort(keyboard_port);
		id = in->getPortName(keyboard_port);
		open = true;
	}
	else
		std::cout << "Keyboard not found - MIDI In uninitialized" << std::endl;
}

MidiIn::~MidiIn(){
	delete in;
}

std::vector<Note> MidiIn::get_voices(){
	std::vector<Note> ret{};
	if (open){
		std::vector<unsigned char> message(3,'\0');
		do{
			in->getMessage(&message);
			if (message.size() == 3 && message[0] == NOTE_ON && message[2] > 0){
				ret.push_back(Note(message[1], 1, message[2]));
			}
		} while(!message.empty());
	}
	return ret;
}

void MidiIn::clear_queue(){
	std::vector<unsigned char> message(3,'\0');
	do{
		in->getMessage(&message);
	} while(!message.empty());
}


std::map<int, std::string> KMidi::note_names = {
	{0, "C"},
	{1, "C#"},
	{2, "D"},
	{3, "D#"},
	{4, "E"},
	{5, "F"},
	{6, "F#"},
	{7, "G"},
	{8, "G#"},
	{9, "A"},
	{10, "A#"},
	{11, "B"}
};
std::map<int, std::string> KMidi::note_names_flats = {
	{0, "C"},
	{1, "Db"},
	{2, "D"},
	{3, "Eb"},
	{4, "E"},
	{5, "F"},
	{6, "Gb"},
	{7, "G"},
	{8, "Ab"},
	{9, "A"},
	{10, "Bb"},
	{11, "B"}
};

std::string KMidi::note_name(int decimal){
	std::string note;
	if (true) note = KMidi::note_names_flats.at(decimal%12);
	else note = KMidi::note_names.at(decimal%12);

	char octave =  '0' + (decimal/12 - 1);
	return note + octave;
}


