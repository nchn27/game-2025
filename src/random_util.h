#pragma once

#include <random>

std::mt19937_64 mt(std::random_device{}());

std::uniform_real_distribution<double> uniform(0.0,1.0);

int choose(int N, double* weights=nullptr) {
	double sum = 0;
	if(weights != nullptr) {
		for(int i = 0; i < N; i++) sum += weights[i];
	}
	if(sum <= 1e-9) {
		return (int)(uniform(mt)*N);
	}

	double selection = uniform(mt)*sum;
	for(int i = 0; i < N; i++) {
		if(weights[i] > selection) {
			return i;
		}
		selection -= weights[i];
	}

	return N-1;
}