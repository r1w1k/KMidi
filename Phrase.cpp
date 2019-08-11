#include "Phrase.h"
#include "Note.h"
#include <iostream>

using namespace std;

Phrase::Phrase(int l, int r)
	: length{l}, resolution{r}{
}

void Phrase::print() const{
	// cout << "Generic print" << endl;
}

void Arp::print() const {

}

void Arp::sequence(vector<int> seq){
	double duration_ms = 60.0/bpm/resolution * 1000;
	pattern = seq;
	if (!phrase.empty()) phrase = {};
	int  step{0};
	
	int step_count = count*resolution;
	for (size_t i{0}; i < step_count; i++){
		std::vector<Note> next_note{};
		if (step >= 0 && step < voices.size()){
			next_note = voices.at(step);
			for (Note &n : next_note) 
				n.duration = duration_ms;
			phrase.push_back(next_note);
		}
		else if (wraparound){
			next_note = voices.at(step % voices.size());
			for (Note &n : next_note) 
				n.duration = duration_ms;
			phrase.push_back(next_note);
		}
		else
			phrase.push_back(vector<Note>{Rest(duration_ms)});
		//loop over the pattern vector to see what tone to jump to next
		step += pattern.at(i % pattern.size());
	}
}

void Arp::extend(int oct){
	// int shift{oct*12};
	// vector<int> additions{};
	// for (Note voice : voices){
	// 	additions.push_back(pitch + shift);
	// }
	// voices.insert(voices.end(), additions.begin(), additions.end());
}

void Phrase::transpose(int halfsteps){
	// for (auto &note : notes) note.pitch += halfsteps;
}
void Phrase::oct(int octaves){
	// this->transpose(octaves*12);
}

Arp::Arp(vector<int> v, int length_val)
//TODO: check syntax of initializer list - does it have to be curlies?
	: Phrase(length_val){         
}

//TODO: make a move constructor that actually has these being efficient.
//right now, they're using the default copy constructor, and that's kind of costly.
// Arp Arp::operator-() const{
// 	Arp temp(this->name, this->notes, this->length);
// 	reverse(temp.notes.begin(), temp.notes.end());
// 	return temp;
// }
Arp Arp::operator+(int a) const{
	Arp ret = *this;
	for (vector<Note> p : phrase){
		for (Note note : p) note.pitch += a;
	}
	return ret;
}

Arp Arp::operator*(int a) const{
	Arp ret = *this;
	std::vector<std::vector<Note>> multiplied{};
	for (int i=0; i < a; i++){
		for (std::vector<Note> notes : ret.phrase){
			multiplied.push_back(notes);
		}
	}
	ret.phrase = multiplied;
	return ret;
}
