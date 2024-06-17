#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>

#include "TextParser.h"
#include "Converter.h"
#include "Exceptions.h"
#include "ReaderWriter.h"

namespace prepearing_conversion {
    class PrepearingConversion {
    private:
        std::vector <std::string> ifs_filename;
        std::string ofs_filename;
        std::string text_filename;
        bool help = false;

    public:
        PrepearingConversion(int count_of_files, char** files) {
            if (count_of_files < 3) {
                throw NotEnoughParameters("making convertion");
            }
            if (strcmp(files[1], "-h") == 0) {
                help = true;
            }
            else {
                count_of_files -= 2;
                for (int i = 1; i < count_of_files; i++) {
                    ifs_filename.push_back(files[i]);
                }
                text_filename = files[count_of_files];
                ofs_filename = files[count_of_files + 1];
            }
        };

        ~PrepearingConversion() {};

        void start_conversion();

        void prepearing_parameters(std::vector <std::string>& s_parameters, std::vector <uint32_t>& i_parameters);

        void get_help();
    };
}