#pragma once

#include <string>
#include <vector>

struct Blackboard {
	const std::string CSV;
	std::vector<std::vector<std::string>> data = { {} };
	unsigned pos = 0;
	std::string currentWord = "";
};

class CsvParser {
public:
	static bool isEof(const Blackboard& bb) {
		return bb.CSV.size() <= bb.pos;
	}

	static bool isComma(const Blackboard& bb) {
		return bb.CSV[bb.pos] == ',';
	}

	static bool isNewline(const Blackboard& bb) {
		return bb.CSV[bb.pos] == '\n';
	}

	static void storeWord(Blackboard& bb) {
		bb.data.back().push_back(bb.currentWord);
		bb.currentWord = "";
		bb.pos++;
	}

	static void handleNewline(Blackboard& bb) {
		bb.data.push_back({});
	}

	static void advanceChar(Blackboard& bb) {
		bb.currentWord += bb.CSV[bb.pos];
		bb.pos++;
	}
};
