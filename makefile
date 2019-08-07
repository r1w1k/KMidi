COMP=g++ -std=c++14
MIDIFrame=-framework CoreMIDI -framework CoreAudio -framework CoreFoundation
APPFILES=main.cpp Phrase.o Note.o RtMidi.o KMidi.o

KMidi: main.cpp Note.o Phrase.o KMidi.o
	$(COMP) -Wall -o KMidi $(APPFILES) $(MIDIFrame)

KMidi.o: KMidi.h KMidi.cpp
	$(COMP) -D__MACOSX_CORE__ -c KMidi.cpp

RtMidi.o: RtMidi.h RtMidi.cpp
	$(COMP) -D__MACOSX_CORE__ -c RtMidi.cpp

Phrase.o: Note.o Phrase.h Phrase.cpp
	$(COMP) -c Phrase.cpp

Note.o: Note.cpp Note.h
	$(COMP) -c Note.cpp