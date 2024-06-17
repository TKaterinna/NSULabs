#include <string>

#include "PrepearingConversion.h"
#include "ReaderWriterData.h"

struct st_wav_header
{
    uint8_t chunk_id[4];
    uint32_t chunk_size;   // Иначе говоря, это размер файла - 8, то есть, исключены поля chunkId и chunkSize.
    uint8_t format[4];     // Содержит символы "WAVE"

    uint8_t subchunk1_id[4];   // Содержит символы "fmt "
    uint32_t subchunk1_size;    //16 для формата PCM. Это оставшийся размер подцепочки, начиная с этой позиции.
    uint16_t audio_format;    // Для PCM = 1 (то есть, Линейное квантование).
    uint16_t num_channels;    // Количество каналов. Моно = 1, Стерео = 2 и т.д.
    uint32_t sample_rate;    // Частота дискретизации. 8000 Гц, 44100 Гц и т.д.
    uint32_t byte_rate;    // sampleRate * numChannels * bitsPerSample/8
    uint16_t block_align;    // numChannels * bitsPerSample/8. Количество байт для одного сэмпла, включая все каналы.
    uint16_t bits_per_sample; // Так называемая "глубиная" или точность звучания. 8 бит, 16 бит и т.д.

    uint8_t subchunk2_id[4];    // Содержит символы "data"
    uint32_t subchunk2_size;    // numSamples * numChannels * bitsPerSample/8. Количество байт в области данных.
};

namespace reader {
    void Reader::read_header(st_wav_header& buffer) {
        ifs.read(reinterpret_cast<char*> (&buffer), header_size);
        if (ifs.bad()) {
            throw ReadingError();
        }

        size_t cur_pos = header_size;
        while (buffer.subchunk2_id[0] != 'd' || buffer.subchunk2_id[1] != 'a' || buffer.subchunk2_id[2] != 't' || buffer.subchunk2_id[3] != 'a')
        {
            if (ifs.eof()) {
                throw IncorretWAVFile("no data in " + filename);
            }
            ifs.seekg(buffer.subchunk2_size + cur_pos);
            ifs.read(reinterpret_cast<char*> (&buffer.subchunk2_id), 4);
            if (ifs.bad()) {
                throw ReadingError();
            }
            ifs.read(reinterpret_cast<char*> (&buffer.subchunk2_size), 4);
            if (ifs.bad()) {
                throw ReadingError();
            }
            cur_pos += 8;
        }

        if (buffer.audio_format != 1 || buffer.num_channels != 1 || buffer.bits_per_sample != 16 || buffer.sample_rate != 44100) {
            throw IncorretWAVFile("unsupported file format in " + filename);
        }

        count_last_reading = ifs.gcount();
    }
}

namespace writer {
    void Writer::write_header(st_wav_header& header) {
        ofs.write(reinterpret_cast<char*> (&header), header_size);
        if (ofs.bad()) {
            throw WritingError();
        }
    }
}

namespace prepearing_conversion {
    void PrepearingConversion::start_conversion() {
        if (help == true) {
            get_help();
            exit(0);
        }

        std::string out_filename = ofs_filename;
        {
            std::fstream o;
            o.open(ofs_filename, std::ios::out);
            if (!o.is_open()) {
                throw CannotOpenFile(ofs_filename);
            }
            //o.close();
        }
        text_parser::TextParser in_text(text_filename);
        std::string command;
        std::vector <std::string> s_parameters;
        std::vector <uint32_t> i_parameters;
        std::string in_filename = ifs_filename[0];

        while (in_text.not_eof()) {
            in_text.parse_line(command, s_parameters);
            std::unique_ptr <converter::Converter> f1 = converter::create_converter(command);
            if (f1 == nullptr) {
                throw NoConvertion(command);
            }
            prepearing_parameters(s_parameters, i_parameters);
            s_parameters.insert(s_parameters.begin(), in_filename);
            s_parameters.insert(s_parameters.begin(), out_filename);

            std::vector <std::unique_ptr<areader::AReader>> ifs;

            writer::Writer ofs(s_parameters[0]);
            st_wav_header header;
            for (size_t i = 1; i < s_parameters.size(); i++) {
                reader::Reader* ifs1 = new reader::Reader(s_parameters[i]);
                ifs1->read_header(header);
                if (i == 1) {
                    ofs.write_header(header);
                }
                ifs.push_back((std::unique_ptr<areader::AReader>) ifs1);
            }
            f1->convert(ifs, (awriter::AWriter&) ofs, i_parameters);


            in_filename = out_filename;
            /*for (size_t i = 0; i < s_parameters.size() - 1; i++) {
                delete[] ifs[i];
            }*/
            s_parameters.resize(0);
            i_parameters.resize(0);

        }
    }

    void PrepearingConversion::prepearing_parameters(std::vector <std::string>& s_parameters, std::vector <uint32_t>& i_parameters) {
        size_t i = 0;
        for (; s_parameters[i].find('$') != std::string::npos; i++) {
            s_parameters[i].erase(0, 1);
            uint32_t num_file = std::stoi(s_parameters[i], nullptr, 10) - 1;
            s_parameters[i] = ifs_filename[num_file];
        }
        size_t new_size = i;
        size_t size = s_parameters.size();
        for (; i < size; i++) {
            int32_t p = std::stoi(s_parameters[i]);
            if (p == 0 && (s_parameters[i].find('0') == std::string::npos)) {
                throw IncorrectParameters(s_parameters[i] + " not a number");
            }
            else if (p < 0) {
                throw IncorrectParameters(s_parameters[i] + " not a positive number");
            }
            i_parameters.push_back((uint32_t)p);
        }
        s_parameters.resize(new_size);
    }

    void PrepearingConversion::get_help() {
        std::cout << "Options:" << std::endl;
        std::cout << "-h                   Display this information." << std::endl;
        std::cout << "Parameters:" << std::endl;
        std::cout << "1. Path to input wav files" << std::endl;
        std::cout << "2. Path to text file" << std::endl;
        std::cout << "3. Path to output wav file" << std::endl;
        std::cout << "Struct of text file:" << std::endl;
        std::cout << "Each line consists from type of conversion and parameters devided by spaces. ";
        std::cout << "Each conversion is performed with the file received on the previous conversion." << std::endl;
    }
}