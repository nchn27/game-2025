#pragma once

#include <iostream>

#include "game.h"

// Credit: ChatGPT
// Helper function to generate the NPY header
std::string createNPYHeader(int rows, int cols) {
	std::string header = "\x93NUMPY";  // Magic string
	header += '\x01';                   // Version major
	header += '\x00';                   // Version minor

	std::string dict = "{'descr': '<f4', 'fortran_order': False, 'shape': (";
	dict += std::to_string(rows) + ", " + std::to_string(cols) + "), }";

	// Pad the dictionary to a multiple of 16 bytes (for alignment)
	while ((10 + dict.size()) % 16 != 15) {
		dict += ' ';
	}
	dict += '\n';

	// Add length of header as 2 bytes (little-endian)
	uint16_t header_len = static_cast<uint16_t>(dict.size());
	header += std::string(reinterpret_cast<char*>(&header_len), 2);

	return header + dict;
}

void saveNPY(int player, const std::vector<Infoset> &infosets) {
	std::string folder = (player%2 == 0) ? "team13" : "team24";

	std::ofstream out(folder + "/strategy0-player" + std::to_string(player+1) + ".npy",
			std::ios::binary);
	int numRows = infosets.size();
	std::string header = createNPYHeader(numRows, 3);
	out.write(header.c_str(), header.size());

	float realBuffer[3];
	double strategyBuffer[3];

	for(int i = 0; i < numRows; i++) {
		for(int j = 0; j < 3; j++) {
			realBuffer[j] = 0;
		}
		infosets[i].makeOutputBuffer(strategyBuffer);

		for(int j = 0; j < 3; j++) {
			realBuffer[j] = (float) strategyBuffer[j];
		}
		out.write(reinterpret_cast<char*>(realBuffer), sizeof(realBuffer));
	}

	out.close();
}