//
//  main.cpp
//  os_kp_Xcode
//
//  Created by Илья Ильин on 27.12.2020.
//

#include <iostream>
#include <sstream>
#include <cstring>

#include "file_hdl.hpp"
#include "args_hdl.hpp"
#include "reader.hpp"
#include "editor.hpp"


enum workMode {
    readMode,
    editMode,
    unsupportedFileMode
};

void get_files_from_dir(const std::string& dir_path, std::vector<std::string>& file_paths) {
    DIR *dir;
    struct dirent *entry;
    dir = open_directory(dir_path);
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') { continue; }
        std::string full_path = dir_path + entry->d_name;
        if (access_file(full_path, R_OK|W_OK) == EXIT_SUCCESS) {
            file_paths.push_back(full_path);
        } else {
            std::cerr << "Error while checking file \"" << full_path << "\" " << strerror(errno) << std::endl;
        }
    }
    close_directory(dir);
}

// for regular file - editMode, for dirs - readMode, either - unsupportedFileMode
workMode pick_work_mode(const std::string& path) {
    if (access_file(path, F_OK) != EXIT_SUCCESS ) {
        throw std::runtime_error(strerror(errno));
    }
    
    mode_t file_type = get_file_mode(path);
    if (S_ISREG(file_type)) {
        return editMode;
    }
    if (S_ISDIR(file_type)) {
        return readMode;
    }
    return unsupportedFileMode;
}

void read_mode_loop(const std::string& path, const unsigned long long memsize) {
    std::cout << "+++ Reader mode +++" << std::endl;
    try {
        std::vector<std::string> file_paths;
        get_files_from_dir(path, file_paths);
        Reader reader(file_paths, memsize);
        
        std::string cmd;
        std::cout << "> ";
        while(std::cin >> cmd) {
            if (cmd == "print") {
                std::string file_name;
                std::cin >> file_name;
                if (file_name == "all") {
                    reader.PrintAll();
                } else {
                    reader.PrintDoc(file_name);
                }
            } else if (cmd == "list") {
                reader.PrintFilesList();
            } else if (cmd == "quit") {
                break;
            } else {
                std::cout << "Invalid command" << std::endl;
            }
            std::cout << "> ";
        }
        std::cout << std::endl;
    } catch (const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
    }
}

// a 4 10 add
// d 4 10
// e 4 10 change
// aw 4 5 add
// dw 4 5
// ew 4 5 change
void edit_mode_loop(const std::string& file_path, const unsigned long long memsize) {
    std::cout << "+++ Editor mode +++" << std::endl;
    try {
        Editor editor(file_path, memsize);
        
        std::string input_line;
        std::cout << "> ";
        while(getline(std::cin, input_line)) {
            std::istringstream iss(input_line);
//            std::vector<std::string> tokens{std::istream_iterator<std::string>{iss},
//                                  std::istream_iterator<std::string>{}};
//            if (tokens.size() != 3 || tokens.size() != 4) {
//                std::cout << "Invalid commad" << std::endl;
//            }
//
            std::string cmd;
            iss >> cmd;
            if (cmd == "print") {
                editor.Print();
            } else if (cmd == "a" || cmd == "aw") {
                size_t line;
                size_t idx;
                std::string to_insert;
                iss >> line >> idx >> to_insert;
                if (cmd == "a")
                    editor.Insert(line, idx, to_insert);
                else
                    editor.InsertWord(line, idx, to_insert);
            } else if (cmd == "d" || cmd == "dw") {
                size_t line;
                size_t idx;
                if (cmd == "d") {
                    size_t length;
                    iss >> line >> idx >> length;
                    editor.Delete(line, idx, length);
                } else {
                    iss >> line >> idx;
                    editor.DeleteWord(line, idx);
                }
            } else if (cmd == "e" || cmd == "ew") {
                size_t line;
                size_t idx;
                std::string change_to;
                iss >> line >> idx >> change_to;
                if (cmd == "e")
                    editor.Edit(line, idx, change_to);
                else
                    editor.EditWord(line, idx, change_to);
            } else if (cmd == "quit") {
                break;
            } else {
                std::cout << "Invalid command" << std::endl;
            }
            std::cout << "> ";
        }
        std::cout << "quit" << std::endl;
    } catch (const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
    }
}



int main(int argc, const char * argv[]) {
    
    unsigned long long memsize;
    std::string path;
    parse_argv(argc, argv, memsize, path);
    
    try {
        workMode mode = pick_work_mode(path);
        
        if (mode == readMode) {
            read_mode_loop(path, memsize);
        } else if (mode == editMode) {
            edit_mode_loop(path, memsize);
        } else {
            // unsupportedFileMode
            std::cout << "Error: unsupported file. -h for help." << std::endl;
        }
    } catch (const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
    }
    
    // изменение при открытии одного файла
    // если несколько - просмотр
    // в какую строчку с какой позиции
    
    return 0;
}
