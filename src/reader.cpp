//
//  reader.cpp
//  os_kp_Xcode
//
//  Created by Илья Ильин on 29.12.2020.
//

#include "reader.hpp"
#include <iostream>

Reader::Reader(const std::vector<std::string>& files_paths, const unsigned long long critical_size)
    : _critical_size(critical_size),
    _current_size(0)
{
    for (auto filepath : files_paths) {
        int fd = open_file(filepath, O_RDWR);
        off_t file_size = get_file_size(fd);
        close_file(fd);
        
        size_t size_to_load;
        if (file_size + _current_size > _critical_size) {
            size_to_load = _critical_size - _current_size;
        } else {
            size_to_load = file_size;
        }
        _docs.push_back(std::make_unique<Document>(filepath, size_to_load));
        _current_size += size_to_load;
    }
}

void Reader::PrintDoc(const std::string& file_name) const{
    for (size_t i = 0; i < _docs.size(); ++i) {
        if (_docs[i]->GetFileName() == file_name) {
            _docs[i]->Print(std::cout);
            break;
        }
        if (i == _docs.size() - 1) {
            std::cout << "No such file" << std::endl;
        }
    }
}

void Reader::PrintAll() const {
    for (size_t i = 0; i < _docs.size(); ++i) {
        _docs[i]->Print(std::cout);
    }
}

void Reader::PrintFilesList() const {
    for (size_t i = 0; i < _docs.size(); ++i) {
        std::cout << _docs[i]->GetFileName() << std::endl;
    }
}
    
