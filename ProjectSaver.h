#ifndef PROJECT_SAVER_H
#define PROJECT_SAVER_H

#include <fstream>
#include <streambuf>
#include "Phrase.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;


class Project{
public:
	json j;
	std::string filename;
	std::vector<Arp> playlist;

	void print() const;
	void save();
	std::vector<Arp> load();
};

#endif