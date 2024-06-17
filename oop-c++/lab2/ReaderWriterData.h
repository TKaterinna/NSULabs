#pragma once
#include <fstream>
#include <vector>

#include "Exceptions.h"

namespace areader {
    class AReader {
    private:
        std::ifstream ifs;
        std::string filename;
        size_t count_last_reading = 0;

    public:
        AReader(const std::string& ifs_filename) {
            ifs.open(ifs_filename, std::ios::binary);
            if (!ifs.is_open()) {
                throw CannotOpenFile(ifs_filename);
            }
            filename = ifs_filename;
        };

        void read_data(std::vector<int16_t>& buffer, size_t size) {
            if (buffer.size() * sizeof(int16_t) < size) {
                throw IncorrectParameters("incorrect size for read_data");
            }
            ifs.read(reinterpret_cast<char*> (buffer.data()), size);
            if (ifs.bad()) {
                throw ReadingError();
            }
            count_last_reading = ifs.gcount();
        }

        size_t get_count() { return count_last_reading; }

        bool eof() {
            return ifs.eof();
        }
    };
}

namespace awriter {
    class AWriter {
    private:
        std::fstream ofs;
        std::string filename;

    public:
        AWriter(const std::string& ofs_filename) {
            ofs.open(ofs_filename, std::ios::binary | std::ios::in | std::ios::out);
            if (!ofs.is_open()) {
                throw CannotOpenFile(ofs_filename);
            }
            filename = ofs_filename;
        };

        void write_data(std::vector<int16_t>& buffer, size_t size) {
            if (buffer.size() * sizeof(int16_t) < size) {
                throw IncorrectParameters("incorrect size for write_data");
            }
            ofs.write(reinterpret_cast<char*> (buffer.data()), size);
            if (ofs.bad()) {
                throw WritingError();
            }
        }
    };
}