#ifndef NOTE_H
#define NOTE_H

class Note {
public:
	int pitch;
	int duration; //in steps, whatever the resolution is
	int velocity;
	Note(int p, int d=1, int v=90);
	Note(const Note &source);
};

class Rest : public Note{
public:
	Rest(int d=1);
};
#endif