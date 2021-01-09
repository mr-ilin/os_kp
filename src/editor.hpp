//
//  editor.hpp
//  os_kp_Xcode
//
//  Created by Илья Ильин on 29.12.2020.
//

#pragma once
#include <memory>
#include "document.hpp"

class Editor {
private:
    const unsigned long long    _critical_size; // in bytes
    unsigned long long          _current_size;
    std::unique_ptr<Document>   _doc;

public:
    Editor(const std::string file_path, const unsigned long long critical_size);
    
    void Print() const;
    void Insert(const size_t line_idx, const size_t idx, const std::string& to_add);
    void Delete(const size_t line_idx, const size_t idx, const size_t length);
    void Edit(const size_t line_idx, const size_t idx, const std::string& change_to);
    
    void InsertWord(const size_t line_idx, const size_t word_idx, const std::string& to_add);
    void DeleteWord(const size_t line_idx, const size_t word_idx);
    void EditWord(const size_t line_idx, const size_t word_idx, const std::string& change_to);
    
};
