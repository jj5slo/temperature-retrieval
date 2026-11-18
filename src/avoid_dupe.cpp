#include<iostream>
#include<filesystem>
#include<string>

#include"avoid_dupe.h"

int avoid_dupe(std::string PATH){
	std::string updated_path = PATH;
	int addition = 0;
	while(std::filesystem::exists(updated_path)){
		updated_path = PATH + "_" + std::to_string(addition);
	}
	return addition;
}
