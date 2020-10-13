//
// Created by Zach Bortoff on 2019-11-03.
//

#ifndef PROMETHEUS_STRING_MANIP_HPP
#define PROMETHEUS_STRING_MANIP_HPP

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

std::vector<std::string> split(const std::string& str, char delim = ' ');
std::string read_fen_from_file(const std::string& file_name, int line_num);
std::vector<std::string> read_all_fen_from_file(const std::string& file_name);
std::string swap_lower_upper_case(const std::string& str);

#endif //PROMETHEUS_STRING_MANIP_HPP
