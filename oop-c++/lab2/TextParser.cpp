#include "TextParser.h"

namespace text_parser {
	void TextParser::parse_line(std::string& command, std::vector <std::string>& parameters) {
		std::string line;
		std::getline(ifs, line);
		if (ifs.bad()) {
			throw ReadingError();
		}
		std::istringstream str_line(line);
		std::string word;
		str_line >> word;
		command = word;
		uint32_t i = 0;
		while (str_line >> word) {
			parameters.insert(parameters.begin() + i++, word);
		}
	}
}