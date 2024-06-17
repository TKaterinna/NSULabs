#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

#include "Exceptions.h"

namespace text_parser {
    class TextParser {
    private:
        std::ifstream ifs;
    public:
        TextParser(const std::string& ifs_filename) {
            ifs.open(ifs_filename, std::ios::in);
            if (!ifs.is_open()) {
                throw CannotOpenFile(ifs_filename);
            }
        };

        void parse_line(std::string& command, std::vector <std::string>& parameters);

        bool not_eof() noexcept {
            return !ifs.eof();
        }
    };
}