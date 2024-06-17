#pragma once
#include <iostream>
#include <fstream>
#include <vector>

#include "Header.h"
#include "Exceptions.h"
#include "ReaderWriterData.h"

struct st_wav_header;

namespace reader {
    class ZeroRider : public areader::AReader {
    private:
        std::ifstream ifs;
        size_t count_last_reading = 0;
    };

    class Reader : public areader::AReader {
    private:
        std::ifstream ifs;
        std::string filename;
        size_t count_last_reading = 0;

    public:
        Reader(const std::string& ifs_filename) : AReader(ifs_filename) {
            ifs.open(ifs_filename, std::ios::binary);
            if (!ifs.is_open()) {
                throw CannotOpenFile(ifs_filename);
            }
            filename = ifs_filename;
        };

        void read_header(st_wav_header& buffer);
    };
}

namespace writer {
    class Writer : public awriter::AWriter {
    private:
        std::fstream ofs;
        std::string filename;

    public:
        Writer(const std::string& ofs_filename) : AWriter(ofs_filename) {
            ofs.open(ofs_filename, std::ios::binary | std::ios::in | std::ios::out);
            if (!ofs.is_open()) {
                throw CannotOpenFile(ofs_filename);
            }
            filename = ofs_filename;
        };
        void write_header(st_wav_header& header);
    };
}