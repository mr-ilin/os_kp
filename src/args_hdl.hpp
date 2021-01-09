//
//  args_hdl.hpp
//  os_kp_Xcode
//
//  Created by Илья Ильин on 29.12.2020.
//

#pragma once

#include <string>

#define DEFAULT_MEMSIZE     16777216
#define ERR_INV_ARGS        169

void print_usage();

unsigned long long parse_memsize_num(const std::string& str);
unsigned long long parse_memsize_option(const char* str);
void parse_argv(int argc, const char * argv[], unsigned long long& memsize, std::string& path);
