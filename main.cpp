#include <iostream>
#include <sstream>

#include "KMidi.h"

MidiOut mOut(0);
MidiIn mIn;

void homescreen();
void editor(Arp& arp);
void edit_phrase();
void add_repeats();
void remove_phrase();
void newArp();
void new_time_sig(Arp& arp, bool redirect=true);
void new_bpm(Arp& arp, bool redirect=true);

void change_subdivision(Arp& arp, bool redirect=true);
void multiply(Arp& arp, bool redirect=true);

char get_option(std::string message=""){
	if (!message.empty()){
		std::cout << message << std::endl;
	}
	std::string answer;
	getline(std::cin, answer);
	return std::tolower(answer[0]);
}


std::vector<Arp> playlist{};
int repeats{1};

int main()
{
	newArp();
	return 0;
}


std::vector<std::vector<Note>> get_arp_voices(){
	return mIn.get_voices();
}	

void change_voices(Arp& arp){
	arp.voices = get_arp_voices();
	editor(arp);
}

void newArp(){
	Arp a;
	a.voices = get_arp_voices();
	a.length = 20;
	a.sequence({1});
	new_time_sig(a, false);
	editor(a);
}

void play_current(Arp& arp){
	std::cout << "playing..." << std::endl;
	mOut.play(&arp);
	editor(arp);
}
void play_all(){
	for (int i=0; i < repeats; i++){
		for (int j=0; j < playlist.size(); j++){
			std::cout << "Phrase " << j + 1 << std::endl;
			mOut.play(&playlist.at(j));
		}
	}
	homescreen();
}

void togglewrap(Arp& arp){
	arp.wraparound = !arp.wraparound;
	arp.sequence(arp.pattern);
	editor(arp);
}

void togglering(Arp& arp){
	if (arp.ringout)
		for (std::vector<Note> notes : arp.phrase)
			for (Note n : notes)
				mOut.note_off(n);
			
	arp.ringout = !arp.ringout;	
	editor(arp);
}
void new_sequence(Arp& arp){
	std::cout << "Enter steps separated by commas: " << std::endl;
	vector<int> pattern{};
	std::string step{};
	std::cin >> step;

	std::istringstream ss(step);
	std::string token{};
	while(getline(ss, token, ',')){
		pattern.push_back(std::stoi(token));
	}
	arp.sequence(pattern);
	editor(arp);
}

void new_time_sig(Arp& arp, bool redirect){
	double old_subdivision = arp.division;

	std::cout << "Enter time signature:" << std::endl;
	std::string input;
	std::cin >> input;

	std::istringstream ss(input);
	std::string token{};
	std::vector<int> time_sig{};

	while(getline(ss, token, '/')){
		time_sig.push_back(std::stoi(token));
	}
	arp.count = time_sig[0];
	arp.division = time_sig[1];
	arp.resolution *= arp.division/old_subdivision;

	arp.sequence(arp.pattern);
	if(redirect) editor(arp);
}
void new_bpm(Arp& arp, bool redirect){
	std::cout << "Enter bpm:" << std::endl;
	std::cin >> arp.bpm;
	arp.sequence(arp.pattern);
	if (redirect) editor(arp);
}

void change_subdivision(Arp& arp, bool redirect){
	std::cout << "Enter subdivision:" << std::endl;
	std::cin >> arp.resolution;
	arp.sequence(arp.pattern);
	if (redirect) editor(arp);
}
void multiply(Arp& arp, bool redirect){
	std::cout << "How many repeats?" << std::endl;
	std::cin >> arp.repeat;
	if (redirect) editor(arp);
}
void homescreen(){
	std::cout << std::endl << std::endl << std::endl << "Homescreen" << std::endl;
	std::cout << "------------------------------------------------------------" << std::endl << std::endl;
	std::cout << "p     play" << std::endl;
	std::cout << "n     new phrase" << std::endl;
	std::cout << "e     edit phrase" << std::endl;
	std::cout << "r     remove phrase" << std::endl;
	std::cout << "q     quit" << std::endl;
	std::cout << "------------------------------------------------------------" << std::endl << std::endl;

	switch(get_option("Enter option: ")){
		case 'p':
			play_all();
			break;
		case 'n':
			newArp();
			break;
		case 'e':
			edit_phrase();
			break;
		case 'm':
			add_repeats();
			break;
		case 'r':
			remove_phrase();
			break;
		case 'q':
			break;
		default:
			homescreen();
	}
}

void edit_phrase(){
	int p;
	std::cout << "Enter phrase number:" << std::endl;
	std::cin >> p;
	if (p > 0 && p <= playlist.size()){
		editor(playlist[p-1]);
	}

}

void add_repeats(){
	std::cout << "Enter repeats:" << std::endl;
	std::cin >> repeats;
	homescreen();
}
void remove_phrase(){
	int p;
	std::cout << "Enter phrase number:" << std::endl;
	std::cin >> p;
	if (p > 0 && p <= playlist.size()){
		playlist.erase(playlist.begin() + p-1);
	}
}

void editor(Arp& arp){
	if (!arp.voices.empty()){
		std::cout << std::endl;
		std::cout << std::endl << std::endl << std::endl << "Phrase Editor" << std::endl;
		std::cout << "------------------------------------------------------------" << std::endl << std::endl;
		for (std::vector<Note> n : arp.voices){
			for (Note n2 : n){
				std::cout << KMidi::note_name(n2.pitch) << (n.size() > 1 ? "-" : "");
			}
			std::cout << " ";
		}
		std::cout << std::endl << arp.count << "/" << arp.division << std::endl;
		std::cout << arp.resolution << " notes per count" << std::endl << std::endl;
		std::cout << "p     play" << std::endl;
		std::cout << "n     change notes" << std::endl;
		std::cout << "t     time signature" << std::endl;
		std::cout << "s     define sequence" << std::endl;
		std::cout << "m     multiply (add repeats)" << std::endl;
		std::cout << "b     change bpm" << std::endl;
		std::cout << "d     change subdivision" << std::endl;
		std::cout << "w     toggle wraparound" << std::endl;
		std::cout << "r     toggle ringout" << std::endl;
		std::cout << "a     new arpeggio (start over)" << std::endl;
		std::cout << "c     add to playlist" << std::endl;
		std::cout << "e     save edits to playlist" << std::endl;
		std::cout << "------------------------------------------------------------" << std::endl;

		switch(get_option("Enter option: ")){
			case 'p':
				play_current(arp); break;
			case 'n':
				change_voices(arp); break;
			case 't':
				new_time_sig(arp); break;
			case 'b':
				new_bpm(arp); break;
			case 'd':
				change_subdivision(arp); break;
			case 's':
				new_sequence(arp); break;
			case 'w':
				togglewrap(arp); break;
			case 'r':
				togglering(arp); break;
			case 'a':
				newArp(); break;
			case 'm':
				multiply(arp); break;
			case 'c':
				playlist.push_back(arp);
				homescreen();
				break;
			case 'e':
				homescreen();
				break;
			case 'q': break;
			default:
				editor(arp);
		}
	}
	else newArp();
}