//
//  editor.cpp
//  os_kp_Xcode
//
//  Created by Илья Ильин on 29.12.2020.
//

#include "editor.hpp"

Editor::Editor(const std::string file_path, const unsigned long long critical_size)
    : _critical_size(critical_size),
    _current_size(0)
{
    int fd = open_file(file_path, O_RDWR);
    off_t file_size = get_file_size(fd);
    close_file(fd);
    
    size_t size_to_load;
    if (file_size + _current_size > _critical_size) {
        size_to_load = _critical_size - _current_size;
    } else {
        size_to_load = file_size;
    }
    
    _doc = std::make_unique<Document>(file_path, size_to_load);
    _current_size += size_to_load;
}

void Editor::Print() const {
    _doc->Print(std::cout);
}
// line_idx = 1,2,... ; idx = 0,1,...
// idx = 0 to add at the beggining of the line
void Editor::Insert(const size_t line_idx, const size_t idx, const std::string& to_add) {
    if (to_add.size() + _current_size > _critical_size) {
        std::cout << "Can't fit this amount of text" << std::endl;
        return;
    }
    if (line_idx < 1 || idx < 0) {
        std::cout << "Invalid indices" << std::endl;
        return;
    }
    
    unsigned long long old_size = _doc->GetSize();
    try {
        _doc->InsertAfter(line_idx - 1, idx - 1, to_add);
        _current_size += (_doc->GetSize() - old_size);
    } catch (const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
    }
}

void Editor::Delete(const size_t line_idx, const size_t idx, const size_t length) {
    if (line_idx < 1 || idx < 1) {
        std::cout << "Invalid indices" << std::endl;
        return;
    }
    
    unsigned long long old_size = _doc->GetSize();
    try {
        _doc->Delete(line_idx - 1, idx - 1, length);
        _current_size -= (old_size - _doc->GetSize());
    } catch (const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
    }
}

void Editor::Edit(const size_t line_idx, const size_t idx, const std::string& change_to) {
    if (line_idx < 1 || idx < 1) {
        std::cout << "Invalid indices" << std::endl;
        return;
    }
    
    try {
        _doc->Edit(line_idx - 1, idx - 1, change_to);
    } catch (const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
    }
}

void Editor::InsertWord(const size_t line_idx, const size_t word_idx, const std::string& to_add) {
    if (to_add.size() + _current_size > _critical_size) {
        std::cout << "Can't fit this amount of text" << std::endl;
        return;
    }
    if (line_idx < 1 || word_idx < 0) {
        std::cout << "Invalid indices" << std::endl;
        return;
    }
    
    unsigned long long old_size = _doc->GetSize();
    try {
        _doc->InsertWordAfter(line_idx - 1, word_idx - 1, to_add);
        _current_size += (_doc->GetSize() - old_size);
    } catch (const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
    }
}

void Editor::DeleteWord(const size_t line_idx, const size_t word_idx) {
    if (line_idx < 1 || word_idx < 1) {
        std::cout << "Invalid indices" << std::endl;
        return;
    }
    
    unsigned long long old_size = _doc->GetSize();
    try {
        _doc->DeleteWord(line_idx - 1, word_idx - 1);
        _current_size -= (old_size - _doc->GetSize());
    } catch (const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
    }
}

void Editor::EditWord(const size_t line_idx, const size_t word_idx, const std::string& change_to) {
    if (line_idx < 1 || word_idx < 1) {
        std::cout << "Invalid indices" << std::endl;
        return;
    }
    long long old_size = _doc->GetWordLength(line_idx, word_idx);
    if (old_size - change_to.size() + _current_size > _critical_size) {
        std::cout << "Can't fit this amount of text" << std::endl;
        return;
    }
    try {
        _doc->ChangeWord(line_idx - 1, word_idx - 1, change_to);
        _current_size += old_size - change_to.size();
    } catch (const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
    }
    
}
