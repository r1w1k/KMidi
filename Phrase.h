#ifndef PHRASE_H
#define PHRASE_H
#include <vector>
#include <string>
#include "Note.h"

using namespace std;

class Phrase{
public:
	int length;
	int count = 4;
	int division = 4;
	
	int resolution;
	int bpm = 100;
	int repeat = 1;
	vector<vector<Note>> phrase;
	bool ringout = false;

	void transpose(int halfsteps=0);
	void oct(int octaves=0);
	virtual void print() const;


	Phrase(int l=16, int r=4);
	virtual ~Phrase() = default;

};

class Arp : public Phrase{
public:
	string name;
	vector<Note> voices;
	vector<int> pattern;

	bool wraparound{false};
	
	void extend(int oct=1);

	void sequence(vector<int> seq);
	virtual void print() const override;

	Arp(vector<int> notes_val={}, int length_val=16);
	//overloaded operators
	// Arp operator-() const;

	Arp operator*(int a) const;
	Arp operator+(int a) const;
	~Arp() = default;

};
#endif