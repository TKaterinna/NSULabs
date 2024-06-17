#include <string>
#include "Converter.h"
const int wave_period = 10;

namespace converter {

    std::unique_ptr<Converter> create_converter(const std::string& line) {
        if (line == "mute") {
            return std::make_unique<MuteConverter>();
        }
        else if (line == "mix") {
            return std::make_unique<MixConverter>();
        }
        else if (line == "wave") {
            return std::make_unique<WaveConverter>();
        }
        return nullptr;
    }

    void MuteConverter::convert(std::vector <std::unique_ptr<areader::AReader>>& ifs, awriter::AWriter& ofs, const std::vector <uint32_t>& i_parameters) {
        if (ifs.size() < 1) {
            throw NotEnoughParameters("making mute convertion");
        }
        if (ifs.size() > 1) {
            throw TooMuchParameters("making mute convertion");
        }
        
        uint32_t cur_sec = 0;
        std::vector<int16_t> data_1sec(c_sample_rate);

        if (i_parameters.size() < 2) {
            throw NotEnoughParameters("making mute convertion");
        }
        uint32_t b_sec = i_parameters[0];
        uint32_t e_sec = i_parameters[1];

        do {
            ifs[0]->read_data(data_1sec, c_byte_rate);
            if (cur_sec >= b_sec && cur_sec < e_sec)
            {
                data_1sec.assign(c_byte_rate, static_cast<int16_t>(0));
            }
            ofs.write_data(data_1sec, ifs[0]->get_count());
            cur_sec++;
        } while (!ifs[0]->eof());
    }

    void MixConverter::convert(std::vector <std::unique_ptr<areader::AReader>>& ifs, awriter::AWriter& ofs, const std::vector <uint32_t>& i_parameters) {
        if (ifs.size() < 2) {
            throw NotEnoughParameters("making mix convertion");
        }
        if (ifs.size() > 2) {
            throw TooMuchParameters("making mix convertion");
        }

        uint32_t cur_sec = 0;
        std::vector<int16_t> data1_1sec(c_sample_rate);
        std::vector<int16_t> data2_1sec(c_sample_rate);

        if (i_parameters.size() < 1) {
            throw NotEnoughParameters("making mix convertion");
        }
        uint32_t b_sec = i_parameters[0];

        do {
            ifs[0]->read_data(data1_1sec, c_byte_rate);
            if (!ifs[1]->eof()) {
                ifs[1]->read_data(data2_1sec, c_byte_rate); //if we need start mixing second file from b_sec
                if (cur_sec >= b_sec) {
                    size_t f1_count = ifs[0]->get_count() / 2; //2 is count of samples in 1 sec
                    size_t f2_count = ifs[1]->get_count() / 2;
                    size_t i = 0;
                    for (i; (i < f1_count) && (i < f2_count); i++) {
                        data1_1sec[i] /= 2;
                        data2_1sec[i] /= 2;
                        data1_1sec[i] += data2_1sec[i];
                    }
                }
            }

            ofs.write_data(data1_1sec, ifs[0]->get_count());
            cur_sec++;
        } while (!ifs[0]->eof());
    }

    void WaveConverter::convert(std::vector <std::unique_ptr<areader::AReader>>& ifs, awriter::AWriter& ofs, const std::vector <uint32_t>& i_parameters) {
        if (ifs.size() < 1) {
            throw NotEnoughParameters("making wave convertion");
        }
        if (ifs.size() > 1) {
            throw TooMuchParameters("making wave convertion");
        }

        uint32_t cur_sec = 0;
        std::vector<int16_t> data_5sec0(c_sample_rate * wave_period);
        std::vector<int16_t> data_5sec1(c_sample_rate * wave_period);

        if (i_parameters.size() < 3) {
            throw NotEnoughParameters("making wave convertion");
        }
        uint32_t b_sec = i_parameters[0];
        uint32_t e_sec = i_parameters[1];
        uint32_t frequency = i_parameters[2];

        double time = 0;
        int i = 0;
        double delta_time = 0;
        do {
            ifs[0]->read_data(data_5sec0, c_byte_rate);
            if (cur_sec >= b_sec && cur_sec < e_sec)
            {
                if (i < wave_period / 2) {
                    delta_time = 1.0 / (c_sample_rate * wave_period);
                }
                else if (i < wave_period) {
                    delta_time = -1.0 / (c_sample_rate * wave_period);
                }
                for (size_t j = 0; j < (ifs[0]->get_count() / 2); j++) {
                    time += delta_time;
                    data_5sec0[j] = static_cast<int16_t> (data_5sec0[j] * std::sin(2 * 3.14 * frequency) * time);
                }
                i = (i + 1) % (wave_period);
            }

            ofs.write_data(data_5sec0, ifs[0]->get_count());
            cur_sec++;
        } while (!ifs[0]->eof());
    }
}