#include <iostream>
#include <array>

#include "game_tree_reader.h"
#include "game.h"
#include "random_util.h"
#include "esmccfr.h"
#include "output_strategy.h"

void print_example_trajectories(const std::array<std::vector<Infoset>, 4> &infosets, const std::vector<Game> &games) {
	Game g = games[0];
	while(!g.terminal) {
		std::cout << g.name << std::endl;
		
		if(g.chance_actions.size() > 0) {
			for(int i = 0; i < g.chance_probs.size(); i++) {
				std::cout << "\t" << games[g.chance_actions[i]].name << " " << g.chance_probs[i] << std::endl;
			}
		
			int next_game = g.chance_actions[choose(g.chance_probs.size(), g.chance_probs.data())];
			g = games[next_game];
		} else {
			int actions[3];
			int cnt = 0;
			
			if(g.c_index != -1) {
				std::cout << "\tC" << std::endl;
				actions[cnt++] = g.c_index;
			}
			if(g.f_index != -1) {
				std::cout << "\tF" << std::endl;
				actions[cnt++] = g.f_index;
			}
			if(g.r_index != -1) {
				std::cout << "\tR" << std::endl;
				actions[cnt++] = g.r_index;
			}
			
			std::cout << "\tMy Infoset: " << infosets[g.player][g.infoset_index].name << std::endl;
			
			int next_game = actions[choose(cnt)];
			g = games[next_game];
		}
		
	}
	std::cout << g.name << std::endl;
	std::cout << "\n\n" << std::endl;
}

int main() {
	auto [infosets, games] = create_game_tree(2, 3);

	for(int i = 0; i < 10; i++) {
		print_example_trajectories(infosets, games);
	}
		
	/*
	std::cout << "saving npy's" << std::endl;
		
	for(int player = 0; player < 4; player++) {
		saveNPY(player, infosets[player]);
	}*/
	
	return 0;
}


