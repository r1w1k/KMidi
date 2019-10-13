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
		json j_structure = json::array();
		for (const Arp& arp : playlist){
			json j_phrase;
			j_phrase["length"] = arp.length;
			j_phrase["count"] = arp.count;
			j_phrase["division"] = arp.division;
			j_phrase["resolution"] = arp.resolution;
			j_phrase["bpm"] = arp.bpm;
			j_phrase["repeat"] = arp.repeat;
			j_phrase["voices"] = json::array();
			j_phrase["pattern"] = json::array();

			for (const std::vector<Note>& notes : arp.voices){
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
			for (int i : arp.pattern)
				j_phrase["pattern"].push_back(i);
			
			for (const std::vector<Note>& notes : arp.phrase){
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

		for (const char& s : structure){
			j_structure.push_back(s);
		}
		j["structure"] = j_structure;

	}
	ofstream json_file;
	json_file.open("./compositions/" + filename + ".json");
	json_file << j.dump(2);
	json_file.close();
 	std::cout << "Succesfully saved as " << filename + ".json!" << std::endl;
}


std::vector<Arp> Project::load(){
	std::string filename{"untitled"};
	std::cout << "Enter filename: ";
	std::cin >> filename;

	std::ifstream saved_file("./compositions/" + filename + ".json");
	json json_file;
	saved_file >> json_file;

	playlist = {};
	structure = {};
	for (int s : json_file["structure"]){
		structure.push_back(s);
	}
	for (const auto& phrase : json_file["phrases"]){
		Arp a;
		a.length = phrase["length"];
		a.count = phrase["count"];
		a.division = phrase["division"];
		a.resolution = phrase["resolution"];
		a.bpm = phrase["bpm"];
		a.repeat = phrase["repeat"];
		a.count = phrase["count"];

		a.phrase = {};
		for (const auto& step : phrase["phrase"]){
			std::vector<Note> step_notes{};
			for (auto note : step){
				Note n(note["pitch"], note["duration"], note["velocity"]);
				step_notes.push_back(n);
			}
			a.phrase.push_back(step_notes);
		}

		a.voices = {};
		for (const auto& step : phrase["voices"]){
			std::vector<Note> step_voices{};
			for (auto note : step){
				Note n(note["pitch"], note["duration"], note["velocity"]);
				step_voices.push_back(n);
			}
			a.voices.push_back(step_voices);
		}
		a.pattern = {};
		for (const auto& p : phrase["pattern"]){
			a.pattern.push_back(p);
		}
		playlist.push_back(a);
	}
	return playlist;
}