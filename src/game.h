#pragma once

#include <iostream>
#include <vector>
#include <utility>

void normalizeReLU(double* array, int N) {
	double sum = 0;
	for(int i = 0; i < N; i++) {
		sum += std::max(array[i],0.0);
	}

	if(sum <= 1e-9) {
		for(int i = 0; i < N; i++) {
			array[i] = 1.0/N;
		}
	} else {
		for(int i = 0; i < N; i++) {
			array[i] = std::max(array[i],0.0)/sum;
		}
	}
}

struct Infoset {
	Infoset(int player, bool can_c, bool can_f, bool can_r, const std::string &name) : 
		player(player), can_c(can_c), can_f(can_f), can_r(can_r), name(name)
	{
		for(int i = 0; i < 3; i++) {
			total_strategies[i] = 0;
			total_regrets[i] = 0;
		}
	}
	
	void makeOutputBuffer(double *d) const {
		double average_strategy[3];
		for(int i = 0; i < 3; i++) {
			average_strategy[i] = total_strategies[i];
		}
		normalizeReLU(average_strategy, can_c + can_f + can_r);
		
		int ptr = 0;
		if(can_c) {
			d[0] = average_strategy[ptr++];
		}
		if(can_f) {
			d[1] = average_strategy[ptr++];
		}
		if(can_r) {
			d[2] = average_strategy[ptr++];
		}
	}
	
	int player;
	bool can_c, can_f, can_r;

	double total_strategies[3];
	double total_regrets[3];
	
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