#pragma once

#include <vector>
#include <array>

#include "game.h"

double walkESMCCFR(std::array<std::vector<Infoset>, 4> &infosets, const std::vector<Game> &games, Game g, int trainedPlayer) {	
	if(g.terminal) {
		return g.payoffs[trainedPlayer];
	}

	if(g.chance_actions.size() > 0) {
		int next_game = g.chance_actions[choose(g.chance_probs.size(), g.chance_probs.data())];		
		return walkESMCCFR(infosets, games, games[next_game], trainedPlayer);
	}
	
	int actions[3];
	int cnt = 0;

	if(g.c_index != -1) {
		actions[cnt++] = g.c_index;
	}
	if(g.f_index != -1) {
		actions[cnt++] = g.f_index;
	}
	if(g.r_index != -1) {
		actions[cnt++] = g.r_index;
	}
	assert(cnt > 1);

	Infoset &infoset = infosets[g.player][g.infoset_index];

	double curStrategyBuffer[3];
	for(int i = 0; i < cnt; i++) {
		curStrategyBuffer[i] = infoset.total_regrets[i];
	}
	normalizeReLU(curStrategyBuffer, cnt);

	if(g.player != trainedPlayer) {
		for(int i = 0; i < cnt; i++) {
			infoset.total_strategies[i] += curStrategyBuffer[i];
		}

		return walkESMCCFR(infosets, games, games[actions[choose(cnt, curStrategyBuffer)]], trainedPlayer);
	}
	
	double cfsBuffer[3];
	double average_cf = 0;

	for(int i = 0; i < cnt; i++) {
		cfsBuffer[i] = walkESMCCFR(infosets, games, games[actions[i]], trainedPlayer);
		average_cf += cfsBuffer[i]*curStrategyBuffer[i];
	}

	for(int i = 0; i < cnt; i++) {
		infoset.total_regrets[i] += cfsBuffer[i] - average_cf;
	}
	
	return average_cf;
}
