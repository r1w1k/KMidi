#ifndef NOTE_H
#define NOTE_H

class Note {
public:
	int pitch;
	double duration; //in steps, whatever the resolution is
	int velocity;
	bool disabled = false;
	Note(int p, double d=125, int v=90);
	Note(const Note &source);
};

class Rest : public Note{
public:
	Rest(double d=125);
};
#endif