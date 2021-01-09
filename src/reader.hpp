//
//  reader.hpp
//  os_kp_Xcode
//
//  Created by Илья Ильин on 29.12.2020.
//

#pragma once

#include <vector>
#include <string>
#include <memory>
#include "document.hpp"

class Reader {
private:
    const unsigned long long                _critical_size; // in bytes
    unsigned long long                      _current_size;
    std::vector< std::unique_ptr<Document> >  _docs;
public:
    Reader(const std::vector<std::string>& files_paths, const unsigned long long critical_size);
//    ~Reader();
    void PrintDoc(const std::string& file_name) const;
    void PrintAll() const;
    void PrintFilesList() const;
};
