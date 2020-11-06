//
// Created by Zach Bortoff on 2019-11-03.
//

#include "string_manip.hpp"

/**
 * splits a string by some delimiting character
 * @param str   input string to be delimited
 * @param delim delimiting charater
 * @return      std::vector<std::string> of all substrings having been split
 */
std::vector<std::string> split(const std::string& str, char delim) {
    std::vector<std::string> ret;
    std::string::const_iterator first  = str.cbegin();
    std::string::const_iterator second = str.cbegin();

    for(; second <= str.end(); ++second) {
        if(*(second) == delim || second == str.end()) {
            if(second != first) {
                ret.emplace_back(first, second);
            }

            first = second + 1;
        }
    }

    return ret;
}

/**
 * reads the fens from a given file and line number, and returns it
 * @param file_name the path to the file
 * @param line_num  the linenumber
 * @return          the std::string representing the FEN
 */
std::string read_fen_from_file(const std::string& file_name, int line_num) {
    std::string fen;
    std::string line;

    try {
        std::fstream in(file_name);
        in.exceptions(std::ifstream::badbit);
        if (!in.is_open()) {
            std::string err("Failed to open file: ");
            err += file_name;
            throw std::runtime_error(err);
        }

        for (int i = 1; std::getline(in, line); i++) {
            if(i == line_num) {
                fen = line; // maybe I should just return here and hope it closes on during destruction
            }
        }

        if (in.bad()) {
            throw std::runtime_error("Runtime error in read_fen_from_file(const std::string&, const uint32): Badbit file.");
        }

        in.close();
    } catch(const std::exception& e) {
        throw;
    }

    return fen;
}

/**
 * reads all the fens from a given file
 * @param file_name the path to the file
 * @return          a std::vector<std::string> representing all the FENS
 */
std::vector<std::string> read_all_fen_from_file(const std::string& file_name) {
    std::vector<std::string> fen_vec;
    std::string line;

    try {
        std::fstream in(file_name);
        in.exceptions(std::ifstream::badbit);
        if (!in.is_open()) {
            std::string err("Failed to open file: ");
            err += file_name;
            throw std::runtime_error(err);
        }

        for (int i = 1; std::getline(in, line); i++) {
            fen_vec.push_back(line);
        }

        if (in.bad()) {
            throw std::runtime_error("Runtime error in read_all_fen_from_file(const std::string&, const uint32): Badbit file.");
        }

        in.close();
    } catch (const std::exception& e) {
        throw;
    }

    return fen_vec;
}

std::string swap_lower_upper_case(const std::string& str) {
    std::string ret(str);
    unsigned i = 0;
    while (i < ret.size()) {
        if (ret[i] <= 'z' && ret[i] >= 'a') {
            ret[i] += ('A' - 'a');
        } else if (ret[i] <= 'Z' && ret[i] >= 'A') {
            ret[i] += ('a' - 'A');
        }
        ++i;
    }
    return ret;
}