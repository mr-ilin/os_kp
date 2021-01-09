//
//  args_hdl.cpp
//  os_kp_Xcode
//
//  Created by Илья Ильин on 29.12.2020.
//

#include "args_hdl.hpp"

#include <iostream>

void print_usage() {
    std::cout << "USAGE: editor [options] [dir]" << std::endl;
    std::cout << "OPTIONS:" << std::endl;
    std::cout << "--help                 Prints out the usage information." << std::endl;
    std::cout << "-h                     Alias for --help." << std::endl;
    std::cout << "--memsize=<size>       Sets max size for text buffer." << std::endl;
    std::cout << "                       Supported units: b - byte; k - kilobyte; m - megabyte."<< std::endl;
    std::cout << "                       Default size is 16 megabytes."<< std::endl;
    std::cout << "-m=<size>              Alias for --memsize." << std::endl;
}

unsigned long long parse_memsize_num(const std::string& str) {
    std::string num_str = str.substr(0, str.size() - 1);
    unsigned long long result = stol(num_str);
    if (result < 0) {
        print_usage();
        exit(ERR_INV_ARGS);
    }
    
    switch (str[str.size() - 1]) {
        case 'b':
            return result;
        case 'k':
            return static_cast<unsigned long long>(result) * 1024;
        case 'm':
            return static_cast<unsigned long long>(result) * 1024 * 1024;
        default:
            print_usage();
            exit(ERR_INV_ARGS);
    }
}

// -m=9b len = 5
// --memsize=9b len = 12
unsigned long long parse_memsize_option(const char* str) {
    std::string option = str;
    size_t len = option.size();
    std::string m_pattern = "-m=";
    std::string memsize_pattern = "--memsize=";
    
    if (len < 4 || (option[len-1] != 'b' && option[len-1] != 'k' && option[len-1] != 'm')) {
        print_usage();
        exit(2);
    }
    
    if (option[1] == '-') {
        // --memsize=<size>
        if (len < 12) {
            print_usage();
            exit(ERR_INV_ARGS);
        }
        if (memsize_pattern != option.substr(0, 10)) {
            print_usage();
            exit(ERR_INV_ARGS);
        }
        return parse_memsize_num(option.substr(10));
    }
    
    // -m=<size>
    if (m_pattern != option.substr(0, 3)) {
        print_usage();
        exit(ERR_INV_ARGS);
    }
    return parse_memsize_num(option.substr(3));
}

// returns memsize in bytes and path from argv
void parse_argv(int argc, const char * argv[], unsigned long long& memsize, std::string& path) {
    if (argc == 1) {
        memsize = DEFAULT_MEMSIZE;
        path = "./";
    } else if (argc == 2) {
        if (argv[1][0] == '-') {
            memsize = parse_memsize_option(argv[1]);
            path = "./";
        } else {
            path = argv[1];
            memsize = DEFAULT_MEMSIZE;
        }
    } else if (argc == 3){
        if (argv[1][0] == '-') {
            memsize = parse_memsize_option(argv[1]);
            path = argv[2];
        } else if (argv[2][0] == '-') {
            memsize = parse_memsize_option(argv[2]);
            path = argv[1];
        } else {
            print_usage();
            exit(ERR_INV_ARGS);
        }
    } else {
        print_usage();
        exit(ERR_INV_ARGS);
    }
}
