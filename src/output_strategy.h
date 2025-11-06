#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdint>

#include "game.h"

// Credit: ChatGPT
// Helper function to generate the NPY header
std::string createNPYHeader(int rows, int cols) {
    std::string header = "\x93NUMPY";  // Magic string
    header += '\x01';                   // Version major
    header += '\x00';                   // Version minor

    std::string dict = "{'descr': '<f8', 'fortran_order': False, 'shape': (";
    dict += std::to_string(rows);
    if (cols > 1) dict += ", " + std::to_string(cols);
    dict += "), }";

    // Calculate total length: magic(6) + version(2) + header_len(2) + dict
    // We pad dict so that total length mod 16 == 0, then add a newline.
    while ((10 + dict.size()) % 16 != 15) {
        dict += ' ';
    }
    dict += '\n';

    // Add header length (only for dict)
    uint16_t header_len = static_cast<uint16_t>(dict.size());
    header += std::string(reinterpret_cast<const char*>(&header_len), 2);

    header += dict;
    return header;
}


void saveNPY(int player, const std::vector<Infoset> &infosets) {
	std::string folder = (player % 2 == 0) ? "team13" : "team24";

	std::ofstream out(
		folder + "/strategy0-player" + std::to_string(player + 1) + ".npy",
		std::ios::binary
	);

	int numRows = infosets.size();
	std::string header = createNPYHeader(numRows, 3);
	out.write(header.c_str(), header.size());

	double strategyBuffer[3];  // now doubles
	double realBuffer[3];      // now doubles

	for (int i = 0; i < numRows; i++) {
		for (int j = 0; j < 3; j++) {
			strategyBuffer[j] = 0.0;
		}
		infosets[i].makeOutputBuffer(strategyBuffer);

		for (int j = 0; j < 3; j++) {
			realBuffer[j] = strategyBuffer[j];
		}
		out.write(reinterpret_cast<char*>(realBuffer), sizeof(realBuffer));
	}

	out.close();
}
