#include <iostream>
#include <sstream>

#include "KMidi.h"

MidiOut mOut(0);
MidiIn mIn;

void homescreen(Arp& arp);
void newArp();
void new_time_sig(Arp& arp, bool redirect=true);
void new_bpm(Arp& arp, bool redirect=true);

void change_subdivision(Arp& arp, bool redirect=true);

int main()
{
	newArp();
	return 0;
}

std::vector<Note> get_arp_voices(){
	if (mIn.open){
		mIn.clear_queue();
		std::cout << "Play voices (press enter when done):";
		std::string temp;
		std::getline(std::cin, temp);
		return mIn.get_voices();
	}
	else {
		std::cout << "No keyboard input - returning nothing" << std::endl;
		return {};
	}
}	

void change_voices(Arp& arp){
	arp.voices = get_arp_voices();
	homescreen(arp);

}

void newArp(){
	Arp a;
	a.voices = get_arp_voices();
	a.length = 20;
	a.sequence({1});
	new_time_sig(a, false);
	homescreen(a);
}

void playloop(Arp& arp){
	std::cout << "playing..." << std::endl;
	mOut.play(&arp, 1);
	homescreen(arp);
}

void togglewrap(Arp& arp){
	arp.wraparound = !arp.wraparound;
	arp.sequence(arp.pattern);
	homescreen(arp);
}

void togglering(Arp& arp){
	arp.ringout = !arp.ringout;
	homescreen(arp);
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
	homescreen(arp);
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
	if(redirect) homescreen(arp);
}
void new_bpm(Arp& arp, bool redirect){
	std::cout << "Enter bpm:" << std::endl;
	std::cin >> arp.bpm;
	arp.sequence(arp.pattern);
	if (redirect) homescreen(arp);

}

void change_subdivision(Arp& arp, bool redirect){
	std::cout << "Enter subdivision:" << std::endl;
	std::cin >> arp.resolution;
	arp.sequence(arp.pattern);
	if (redirect) homescreen(arp);
}
void homescreen(Arp& arp){
	mIn.clear_queue();
	if (!arp.voices.empty()){
		std::cout << std::endl;
		std::cout << "------------------------------------------------------------" << std::endl << std::endl;
		for (Note n : arp.voices){
			std::cout << KMidi::note_name(n.pitch) << " ";
		}
		std::cout << std::endl << arp.count << "/" << arp.division << std::endl;
		std::cout << arp.resolution << " notes per count" << std::endl << std::endl;
		std::cout << "p     play" << std::endl;
		std::cout << "n     change notes" << std::endl;
		std::cout << "t     time signature" << std::endl;
		std::cout << "b     change bpm" << std::endl;
		std::cout << "d     change subdivision" << std::endl;
		std::cout << "s     define sequence" << std::endl;
		std::cout << "w     toggle wraparound" << std::endl;
		std::cout << "r     toggle ringout" << std::endl;
		std::cout << "a     new arpeggio (start over)" << std::endl;
		std::cout << "c     add to composition" << std::endl << std::endl;
		std::cout << "------------------------------------------------------------" << std::endl;

		char c;
		std::cin >> c;
		switch(std::tolower(c)){
			case 'p':
				playloop(arp); break;
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
				arp = arp * 4;
				homescreen(arp);
				break;
			case 'q': break;
			default:
				homescreen(arp);
		}
	}
	else newArp();
}