#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

#include "Header.h"
#include "ReaderWriter.h"
#include "ReaderWriterData.h"

namespace converter {
    class Converter {
    public:
        Converter() {};

        virtual ~Converter() {};

        virtual void convert(std::vector <std::unique_ptr<areader::AReader>>& ifs, awriter::AWriter& ofs, const std::vector <uint32_t>& i_parameters) = 0;
    };

    class MuteConverter : public Converter {
        virtual void convert(std::vector <std::unique_ptr<areader::AReader>>& ifs, awriter::AWriter& ofs, const std::vector <uint32_t>& i_parameters) override;
    };

    class MixConverter : public Converter {
        virtual void convert(std::vector <std::unique_ptr<areader::AReader>>& ifs, awriter::AWriter& ofs, const std::vector <uint32_t>& i_parameters) override;
    };

    class WaveConverter : public Converter {
        virtual void convert(std::vector <std::unique_ptr<areader::AReader>>& ifs, awriter::AWriter& ofs, const std::vector <uint32_t>& i_parameters) override;
    };

    std::unique_ptr<Converter> create_converter(const std::string& line);
}