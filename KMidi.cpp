#include <unistd.h>
#include "KMidi.h"
#include "RtMidi.h"
#include <iostream>
#include <chrono>

namespace ControllerInput {
	int channel { 0 };
	int tapped_tempo { 100 };
	std::vector<std::vector<Note>> phrase{};
	unsigned char last_message = NOTE_OFF | channel;
	bool recording { false };

	void phrase_callback(double deltatime, std::vector<unsigned char> *message, void *userData);
	void tap_tempo_callback(double deltatime, std::vector<unsigned char> *message, void *userData);
}


void MidiOut::sleep(const int &ms){
	usleep(ms*1000);
}

void MidiOut::play(Phrase *p){
	//TOD0: this needs to not only do intra-phrase latency, but latency between phrases (or between repeats of a single phrase)
	std::vector<unsigned char> message(3, '\0');
	auto now = chrono::steady_clock::now();
	auto then = now;
	for (int i = 0; i < p->repeat; i++){
		for (vector<Note> nVec : p->phrase){
			int length{0};
			for (Note n : nVec){
				if (n.velocity > 0){
					note_on(n);
				}
			}
			sleep(nVec.at(0).duration);
			now = chrono::steady_clock::now();
			then = now;

			for (Note n : nVec){
				if (n.velocity > 0){
					note_off(n);	
				}
			}
		}
	}
	// all_notes_off();
}

MidiOut::MidiOut(int c){
	channel = c;
	out = new RtMidiOut();

	unsigned int ports = out->getPortCount();

	int output_port{-99};

	std::cout << "MIDI OUT available ports:" << std::endl;
	for (int i = 0; i < ports; i++)
		std::cout << i + 1 << ": " << out->getPortName(i) << std::endl;
	std::cout << "Enter output port number: ";
	std::cin >> output_port;
	
	if (output_port >= 0 && output_port < ports){
		out->openPort(output_port);
		id = out->getPortName(output_port);
		std::cout << "Connected output to " << id << std::endl << std::endl;
		;
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
void MidiOut::sustain_on(){
	std::vector<unsigned char> message(3, '\0');
	message.at(0) = CONTROL | channel;
	message.at(1) = SUSTAIN;
	message.at(2) = 127;
	out->sendMessage(&message);
}
void MidiOut::sustain_off(){
	std::vector<unsigned char> message(3, '\0');
	message.at(0) = CONTROL | channel;
	message.at(1) = SUSTAIN;
	message.at(2) = 0;
	out->sendMessage(&message);
}
void MidiOut::all_notes_off(){
	std::vector<unsigned char> message(3, '\0');
	message.at(0) = CHANNEL_MODE | channel;
	message.at(1) = ALL_OFF;
	message.at(2) = 0;
	// out->sendMessage(&message);
}

MidiIn::MidiIn(){
	in = new RtMidiIn();
	unsigned int ports = in->getPortCount();
	std::cout << "MIDI IN available ports:" << std::endl;
	int keyboard_port{-99};
	for (int i = 0; i < ports; i++)
		std::cout << i+1 << ": " << in->getPortName(i) << std::endl;

	std::cout << std::endl;
	std::cout << "Enter port number: ";
	std::cin >> keyboard_port; keyboard_port--;
	if (keyboard_port >= 0 && keyboard_port < ports){
		in->openPort(keyboard_port);
		in->setCallback(&ControllerInput::phrase_callback);
		id = in->getPortName(keyboard_port);
		open = true;
		std::cout << "Connected input to " << id << std::endl << std::endl;
	}
	else
		std::cout << "Keyboard not found - MIDI In uninitialized" << std::endl;
}

MidiIn::~MidiIn(){
	delete in;
}

void obtain_input(){
	//this just holds up the UI thread while the input thread is live
	cin.ignore();
	std::cout << "Press enter when done: " << std::endl;
	std::string temp;
	std::getline(std::cin, temp);
}
std::vector<std::vector<Note>> MidiIn::get_voices(){
	ControllerInput::phrase.clear();
	ControllerInput::recording = true;
	if (open){
		obtain_input();
		ControllerInput::recording = false;
		return ControllerInput::phrase;
	}
	else {
		//just some Cmin7 placeholder when no keyboard is available
		return {{Note(48)}, {Note(58)}, {Note(63)}, {Note(67)}, {Note(68)}};
	}
}

int MidiIn::tap_tempo(){
	in->setCallback(&ControllerInput::tap_tempo_callback);
	if (open){
		obtain_input();
		return ControllerInput::tapped_tempo;
	}
	in->setCallback(&ControllerInput::phrase_callback);
	return ControllerInput::tapped_tempo;
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
std::string KMidi::note_name(int note_pitch, bool use_flats){
	if (note_pitch == 0) return " } ";

	std::string note;
	note = use_flats ? KMidi::note_names_flats.at(note_pitch%12) : KMidi::note_names.at(note_pitch%12);
	char octave =  '0' + (note_pitch/12 - 1);
	return note + octave;
}

void ControllerInput::phrase_callback(double deltatime, std::vector<unsigned char> *message, void *userData){
	if (recording){
		int msg = message->at(0);
		int pitch = message->at(1);
		int velocity = message->at(2);
		//for debugging what messages are allowed:
		std::cout << msg << " " << pitch << " " << velocity << std::endl;
		// std::vector<int> allowable { NOTE_ON, NOTE_OFF, MODULATE }

		if (phrase.size() > 1000) phrase.clear();

		if (msg == (NOTE_ON | channel)){
			if (last_message == (NOTE_ON | channel)){
				phrase.back().push_back(Note(pitch, 1, velocity));
			}
			else if (velocity > 0){
				//new entry
				Note new_note = Note(pitch, 1, velocity);
				phrase.push_back(std::vector<Note>{new_note});
			}
		}
		else if ((last_message == (NOTE_OFF | channel) ||
				last_message == (REST_MSG | channel)) &&
				msg == (REST_MSG | channel) &&
				pitch == 0 && velocity == 64){
			std::cout << "added rest" << std::endl;
			phrase.push_back(std::vector<Note>{Rest(1)});
		}

		if (msg == (NOTE_ON | channel) && velocity == 0){
			last_message = NOTE_OFF | channel;
		}
		else last_message = msg;
	}
}

void ControllerInput::tap_tempo_callback(double deltatime, std::vector<unsigned char> *message, void *userData){
	std::cout << "deltatime " << deltatime << std::endl;
}