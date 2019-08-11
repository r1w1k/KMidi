#include <unistd.h>
#include "KMidi.h"
#include "RtMidi.h"
#include <iostream>
#include <chrono>

void MidiOut::sleep(const int &ms){
	usleep(ms*1000);
}

void MidiOut::play(Phrase *p){
	//TOD0: this needs to not only do intra-phrase latency, but latency between phrases (or between repeats of a single phrase)
	std::vector<unsigned char> message(3, '\0');
	auto now = chrono::steady_clock::now();
	auto then = now;
	double latency = 0;
	for (int i = 0; i < p->repeat; i++){
		for (vector<Note> nVec : p->phrase){
			int length{0};
			for (Note n : nVec){
				if (n.velocity > 0){
					note_on(n);
				}
			}
			sleep(nVec.at(0).duration - latency);
			now = chrono::steady_clock::now();
			latency = chrono::duration_cast<chrono::milliseconds>(now - then).count() - nVec.at(0).duration;
			then = now;

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
		if (out->getPortName(i) == KMIDI_OUTPUT)
			output_port = i;
	}
	if (output_port >= 0){
		out->openPort(output_port);
		id = out->getPortName(output_port);
	}
	else
		std::cout << "MIDI Out uninitialized" << std::endl;
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

namespace Controller_Input {
	int channel{0};
	std::vector<std::vector<Note>> phrase{};
	unsigned char last_message = NOTE_OFF;

	void callback( double deltatime, std::vector<unsigned char> *message, void *userData){
		if (message->at(0) == (NOTE_ON | channel)){
			if (last_message == (NOTE_ON | channel)){
				phrase.back().push_back(Note(message->at(1), 1, message->at(2)));
			}
			else{
				//new entry
				Note new_note = Note(message->at(1), 1, message->at(2));
				phrase.push_back(std::vector<Note>{new_note});
			}
		}
		last_message = message->at(0);
	}
}

MidiIn::MidiIn(){
	in = new RtMidiIn();
	unsigned int ports = in->getPortCount();

	int keyboard_port{-99};
	for (int i = 0; i < ports; i++){
		if (in->getPortName(i) == KEYBOARD)
			keyboard_port = i;
	}
	
	if (keyboard_port >= 0){
		in->openPort(keyboard_port);
		in->setCallback(&Controller_Input::callback);
		id = in->getPortName(keyboard_port);
		open = true;
	}
	else
		std::cout << "Keyboard not found - MIDI In uninitialized" << std::endl;
}

MidiIn::~MidiIn(){
	delete in;
}

std::vector<std::vector<Note>> MidiIn::get_voices(){
	Controller_Input::phrase.clear();
	if (open){
		std::cout << "Play voices (press enter when done):";
		std::string temp;
		std::getline(std::cin, temp);
		return Controller_Input::phrase;
	}
	else {
		//just some Cmin7 placeholder when no keyboard is available
		return {{Note(48)}, {Note(58)}, {Note(63)}, {Note(67)}, {Note(68)}};
	}

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
std::string KMidi::note_name(int decimal, bool use_flats){
	std::string note;
	note = use_flats ? KMidi::note_names_flats.at(decimal%12) : KMidi::note_names.at(decimal%12);
	char octave =  '0' + (decimal/12 - 1);
	return note + octave;
}