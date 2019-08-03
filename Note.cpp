#include "Note.h"
#include <iostream>

Note::Note(int p, int d, int v)
	: pitch{p}, duration{d}, velocity{v}{
	// std::cout << "Note ctor !!!" << std::endl;

}

Rest::Rest(int d) : Note(0, d, 0){
	// std::cout << "Rest ctor" << std::endl;
}

