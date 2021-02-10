//
//  document.hpp
//  os_kp_Xcode
//
//  Created by Илья Ильин on 29.12.2020.
//

#pragma once

#include <string>
#include <iostream>
#include "file_hdl.hpp"

class Document {
private:
    std::string          _path;
    unsigned long long   _size;
    char*                _map;
    
    long long GetAbsIdx(const size_t line_idx,  const long long idx) const;
    long long FindWordStartIdx(const size_t line_idx, const size_t word_idx) const;
    long long FindWordEndIdx(const size_t line_idx, const size_t word_idx) const;
    size_t    CountSpaceAfter(const size_t word_start, const size_t word_len) const;
    
    void AbsInsertAfter(const long long abs_idx, const std::string& to_add);
    void AbsDelete(const size_t abs_idx, const size_t length);
    
public:
    Document(const std::string& file_path);
    Document(const int fd, const std::string& file_path, unsigned long long size_to_load);
    Document(const std::string& file_path, unsigned long long size_to_load);
    Document(Document&& doc) noexcept;
    ~Document();
    
    unsigned long long GetSize() const;
    std::string GetFileName() const;
    std::string GetFullPath() const;
    
    void Print(std::ostream& os) const;
    void InsertAfter(const size_t line_idx, const long long idx, const std::string& to_add);
    void Delete(const size_t line_idx, const size_t idx, const size_t length);
    void Edit(const size_t line_idx, const size_t idx, const std::string& change_to);
    
    long long GetWordLength(const size_t line_idx, const size_t word_idx) const;
    void InsertWordAfter(const size_t line_idx, const size_t word_idx, const std::string& word);
    void DeleteWord(const size_t line_idx, const size_t word_idx);
    void ChangeWord(const size_t line_idx, const size_t word_idx, const std::string& word);
};
