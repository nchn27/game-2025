#pragma once

#include <iostream>
#include <vector>
#include <utility>

struct Infoset {
	Infoset(int player, bool can_c, bool can_f, bool can_r, const std::string &name) : 
		player(player), can_c(can_c), can_f(can_f), can_r(can_r), name(name)
	{}
	
	int player;
	//regrets, strategies, etc.
	bool can_c, can_f, can_r;
	
	std::string name;
};

struct Game {
	Game(const std::string &name) : 
		terminal(false), payoffs({0,0,0,0}), player(-1), infoset_index(-1), 
		chance_actions({}), chance_probs({}), c_index(-1), f_index(-1), r_index(-1), name(name)
	{}
	
	bool terminal;
	std::array<int,4> payoffs;

	int player;
	int infoset_index;
	
	//if this vector is empty, then this node is not a chance node. Otherwise it is a chance node. 
	std::vector<int> chance_actions; 
	std::vector<double> chance_probs;
	//next states if an action is taken
	int c_index, f_index, r_index;
	
	std::string name;
};