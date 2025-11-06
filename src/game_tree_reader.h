#pragma once

#include <fstream>
#include <sstream>
#include <unordered_map>
#include <cassert>

#include "game.h"

std::vector<std::string> tokenize_line(const std::string &line) {
	std::istringstream iss(line);
	std::string token;
	
	std::vector<std::string> tokens;
	
	while(iss >> token) {
		tokens.push_back(token);
	}
	
	return tokens;
}

std::pair<std::string, std::string> separate_string_pair(const std::string &token) {
	std::istringstream iss(token);
    std::string key, value_str;

    if (std::getline(iss, key, '=') && std::getline(iss, value_str)) {
        return {key, value_str};
    } else {
        std::cerr << "Invalid format, expecting {}={}!" << std::endl;
		exit(0);
	}
}

std::pair<std::unordered_map<std::string, int>, std::vector<Infoset>> read_infosets(int player) {
	std::ifstream infile("leduc-infosets/player_" + std::to_string(player+1) + "_infosets.txt");
	if(!infile) {
		std::cerr << "Error: could not open infosets for player " << player << std::endl;
		exit(0);
	}
	
	std::unordered_map<std::string, int> name_to_index;
	std::vector<Infoset> infosets;
	
	std::string line;
	while(std::getline(infile, line)) {
		std::vector<std::string> tokens = tokenize_line(line);
		
		//token[0] is index, token[1] is name, token[2] is available actions.
		std::string name = tokens[1];
		bool can_c = false, can_f = false, can_r = false;
		
		for(char c : tokens[2]) {
			if(c == 'C') can_c = true;
			if(c == 'F') can_f = true;
			if(c == 'R') can_r = true;
		}

		name_to_index[name] = infosets.size();
		infosets.emplace_back(player, can_c, can_f, can_r, name);
	}
	
	return {name_to_index, infosets};
}

std::pair<std::array<std::vector<Infoset>, 4>, std::vector<Game>> create_game_tree() {
	std::array<std::unordered_map<std::string, int>, 4> name_to_index;
	std::array<std::vector<Infoset>, 4> infosets;

	for(int i = 0; i < 4; i++) {
		auto p = read_infosets(i);
		name_to_index[i] = std::move(p.first);
		infosets[i] = std::move(p.second);
	}
	
	std::cout << "Finished reading infosets" << std::endl;
	
    std::ifstream infile("leduc_tree.txt");
    if (!infile) {
        std::cerr << "Error: could not open leduc_tree.txt\n";
        exit(0);
    }
	
	std::unordered_map<std::string, int> name_to_index_game;
	std::vector<Game> games;
	
    auto get_game_index = [&](const std::string& name) {
		if(name_to_index_game.find(name) == name_to_index_game.end()) {
			name_to_index_game[name] = games.size();
			games.emplace_back(name);
		}
		return name_to_index_game[name];
    };

    std::string line;
    while (std::getline(infile, line)) {
		std::vector<std::string> tokens = tokenize_line(line);
				
        if(tokens[0] == "node") {			
			std::string name = tokens[1];
			if(name == "/") name = "";
						
			int game_index = get_game_index(name);
			
			std::string player_type = tokens[2];
			
			if(player_type == "chance") {
				assert(tokens[3] == "actions");
				
				double total_prob = 0;
				
				for(int i = 4; i < tokens.size(); i++) {
					auto [first, second] = separate_string_pair(tokens[i]);
					double prob = std::stod(second);
										
					int next_game = get_game_index(name + "/C:" + first);
									
					Game &game = games[game_index];
					game.chance_actions.push_back(next_game);
					game.chance_probs.push_back(prob);
					total_prob += prob;
				}
				assert(abs(total_prob - 1) <= 1e-3);
			} else if(player_type == "player") {
				const std::string &player = tokens[3];
				assert(tokens[4] == "actions");
				
				games[game_index].player = std::stoi(player)-1;
				
				for(int i = 5; i < tokens.size(); i++) {
					if(tokens[i] == "C") {
						games[game_index].c_index = get_game_index(name + "/P" + player + ":C");
					} else if(tokens[i] == "F") {
						games[game_index].f_index = get_game_index(name + "/P" + player + ":F");
					} else if(tokens[i] == "R") {
						games[game_index].r_index = get_game_index(name + "/P" + player + ":R");
					} else {
						std::cerr << "Unknown action!" << std::endl;
						exit(0);
					}
				}
			} else if(player_type == "leaf") {
				assert(tokens[3] == "payoffs");

				Game &game = games[game_index];
				game.terminal = true;
				
				for(int i = 4; i < tokens.size(); i++) {
					auto [first, second] = separate_string_pair(tokens[i]);
					int player = std::stoi(first) - 1;
					int payoff = std::stoi(second);
					
					game.payoffs[player] = payoff;
				}
			} else {
				std::cerr << "Unknown player type " << player_type << std::endl;
				exit(0);
			}
		} else if(tokens[0] == "infoset") {
			std::string infoset_name = tokens[1];
			int player = -1;
			int infoset_index = -1;
			for(int i = 0; i < 4; i++) {
				if(name_to_index[i].find(infoset_name) != name_to_index[i].end()) {
					player = i;
					infoset_index = name_to_index[i][infoset_name];
				}
			}
			assert(player != -1);
			assert(infoset_index != -1);
			
			assert(tokens[2] == "nodes");
			for(int i = 3; i < tokens.size(); i++) {
				std::string game_name = tokens[i];
				
				games[get_game_index(game_name)].infoset_index = infoset_index;
			}
		} else {
			std::cerr << "Unknown first token type" << std::endl;
			exit(0);
		}
    }
	std::cout << "Finished reading game tree" << std::endl;
	
	return {infosets, games};
}