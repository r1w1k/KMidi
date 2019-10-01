COMP=g++ -std=c++14
MIDIFrame=-framework CoreMIDI -framework CoreAudio -framework CoreFoundation
APPFILES=main.cpp Phrase.o Note.o RtMidi.o KMidi.o ProjectSaver.o

kArp: main.cpp KMidi.o
	$(COMP) -Wall -o kArp $(APPFILES) $(MIDIFrame)

KMidi.o: KMidi.h KMidi.cpp RtMidi.o ProjectSaver.o
	$(COMP) -D__MACOSX_CORE__ -c KMidi.cpp

RtMidi.o: RtMidi.h RtMidi.cpp
	$(COMP) -D__MACOSX_CORE__ -c RtMidi.cpp

ProjectSaver.o: Phrase.o ProjectSaver.h ProjectSaver.cpp
	$(COMP) -c ProjectSaver.cpp

Phrase.o: Note.o Phrase.h Phrase.cpp
	$(COMP) -c Phrase.cpp

Note.o: Note.cpp Note.h
	$(COMP) -c Note.cpp