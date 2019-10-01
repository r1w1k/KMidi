#include "ProjectSaver.h"
#include "Phrase.h"

void Project::print() const {
	std::cout << "in project print" << std::endl;
	std::cout << this->j.dump(4) << std::endl;
}

void Project::save() {
	std::string filename{"untitled"};
	std::cout << "Enter filename: ";
	std::cin >> filename;

	j.clear();
	if (true){ //if playlist not empty
		json phrases = json::array();
		for (Arp& arp : playlist){
			json j_phrase;
			j_phrase["length"] = arp.length;
			j_phrase["count"] = arp.count;
			j_phrase["division"] = arp.division;
			j_phrase["resolution"] = arp.resolution;
			j_phrase["bpm"] = arp.bpm;
			j_phrase["repeat"] = arp.repeat;
			j_phrase["voices"] = json::array();

			for (std::vector<Note> notes : arp.voices){
				json j_notes = json::array();
				for (Note n : notes){
					json j_note;
					j_note["pitch"] = n.pitch;
					j_note["velocity"] = n.velocity;
					j_note["duration"] = n.duration;
					j_notes.push_back(j_note);
				}
				j_phrase["voices"].push_back(j_notes);
			}

			for (std::vector<Note> notes : arp.phrase){
				json j_notes = json::array();
				for (Note n : notes){
					json j_note;
					j_note["pitch"] = n.pitch;
					j_note["velocity"] = n.velocity;
					j_note["duration"] = n.duration;
					j_notes.push_back(j_note);
				}
				j_phrase["phrase"].push_back(j_notes);
			}
			phrases.push_back(j_phrase);
		}
		j["phrases"] = phrases;
	}
	std::cout << j.dump(3) << std::endl;
}

// std::vector<Arp> Project::load{
// 	std::string file="untitled";
// 	std::string filename{"untitled"};
// 	std::cout << "Enter filename: ";
// 	std::cin >> filename;


// }