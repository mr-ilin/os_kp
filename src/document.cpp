//
//  document.cpp
//  os_kp_Xcode
//
//  Created by Илья Ильин on 29.12.2020.
//

#include "document.hpp"

Document::Document(const std::string& file_path)
    : _path(file_path)
{
    int fd = open_file(file_path, O_RDWR);
    off_t file_size = get_file_size(fd);
    char* maped_file = (char*)map_file_to_edit(fd, file_size, 0);
    close_file(fd);
    
    _size = file_size;
    _map = maped_file;
}

Document::Document(const std::string& file_path, unsigned long long size_to_load)
    : _path(file_path),
    _size(size_to_load)
{
    char* maped_file = NULL;
    if (_size != 0) {
        int fd = open_file(file_path, O_RDWR);
        maped_file = (char*)map_file_to_edit(fd, _size, 0);
        close_file(fd);
    }
    
    _map = maped_file;
}

Document::Document(Document&& doc) noexcept
    : _path(std::move(doc._path)),
    _size(std::move(doc._size)),
    _map(std::move(doc._map))
{}

Document::~Document() {
    msync(_map, _size, MS_SYNC);
    munmap(_map, _size);
}

unsigned long long Document::GetSize() const {
    return _size;
}

std::string Document::GetFileName() const {
   char sep = '/';
   size_t i = _path.rfind(sep, _path.size());
   if (i != std::string::npos) {
      return(_path.substr(i + 1, _path.size() - i));
   }
   return("");
}

std::string Document::GetFullPath() const {
    return _path;
}

size_t getNumDigits(size_t num) {
    size_t digits = 0;
    while(num) {
        num /= 10;
        ++digits;
    }
    return digits;
}

std::string getLinePadding(const size_t line_count) {
    const size_t def_size = 3;
    size_t digits = getNumDigits(line_count);
    size_t sp_cnt;
    if (def_size - digits < 0) {
        sp_cnt = 0;
    } else {
        sp_cnt = def_size - digits;
    }
    
    std::string space_before;
    while (sp_cnt > 0) {
        space_before += " ";
        --sp_cnt;
    }
    
    return "[" + space_before + std::to_string(line_count) + "]";
}

void Document::Print(std::ostream& os) const {
    os << "==== " << GetFileName() << " ====\n";
    if (_size == 0) {
        os << "Loaded part of file is empty\n";
    } else {
        size_t line_count = 1;
        os << getLinePadding(line_count) << " ";
        for (size_t i = 0; i < _size; ++i) {
            if (_map[i] == '\n') {
               os << "\n";
                ++line_count;
                os << getLinePadding(line_count) << " ";
            } else {
                os << _map[i];
            }
        }
       os << "\n";
        if (_map[_size - 1] == '\n') {
            os << "==== end ====\n";
        } else {
            os << "=== no endl ===\n";
        }
    }
}

long long Document::GetAbsIdx(const size_t line_idx, const long long idx) const {
    // long long abs_idx = 0; // idx after '\n'
    long long abs_idx = 0; // idx after '\n'
    size_t line_count = 0;
    while (abs_idx < _size) {
        if (line_count == line_idx) { break; }
        if (_map[abs_idx] == '\n') {
            ++line_count;
        }
        ++abs_idx;
    }
    return abs_idx + idx;
}

// insert after abs_idx
void Document::AbsInsertAfter(const long long abs_idx, const std::string& to_add) {
    size_t old_size = _size;
    size_t new_size = _size + to_add.size();
    
    _map = (char*)remap_file(_path.data(), _map, old_size, new_size);
    
    // shift to end
    for (size_t i = new_size - 1; i > abs_idx + to_add.size(); --i) {
        _map[i] = _map[i - to_add.size()];
    }
    // add text
    for (size_t i = 0; i < to_add.size(); ++i) {
        _map[i + abs_idx + 1] = to_add[i];
    }
    
    _size = new_size;
}

void Document::InsertAfter(const size_t line_idx, const long long idx, const std::string& to_add) {
    long long abs_idx = GetAbsIdx(line_idx, idx);
    
    if (abs_idx != -1) {
        if ((abs_idx < -2) || abs_idx >= _size) {
            throw std::runtime_error("Index is out of text bounds");
        }
    }
    
    AbsInsertAfter(abs_idx, to_add);
}

void Document::AbsDelete(const size_t abs_idx, const size_t length) {
    size_t old_size = _size;
    size_t new_size = _size - length;
    for (size_t i = abs_idx; i < new_size; ++i) {
        _map[i] = _map[i + length];
    }
    
    _map = (char*)remap_file(_path.data(), _map, old_size, new_size);
    _size -= length;
}

void Document::Delete(const size_t line_idx, const size_t idx, const size_t length) {
    long long abs_idx = GetAbsIdx(line_idx, idx);
    
    if (abs_idx < 0 || abs_idx + length >= _size) {
        throw std::runtime_error("Text range to delete is out of bounds");
    }
    
    AbsDelete(abs_idx, length);
}

void Document::Edit(const size_t line_idx, const size_t idx, const std::string& change_to) {
    long long abs_idx = GetAbsIdx(line_idx, idx);
    
    if (abs_idx < 0) {
        throw std::runtime_error("Start of text range to edit is out of bounds");
    }
    
    size_t last_idx = abs_idx + change_to.size();
    // extend if needed
    if (last_idx >= _size) {
        size_t change_start_idx = _size - abs_idx;
        AbsInsertAfter(_size - 1, change_to.substr(change_start_idx));
    }
    
    for (size_t i = abs_idx; i < last_idx; ++i) {
        _map[i] = change_to[i - abs_idx];
    }
}

bool isSep(char cc) {
    return (cc == ' ' || cc == '\t');
}

// -1 if no word at word_idx in line
long long Document::FindWordStartIdx(const size_t line_idx, const size_t word_idx) const {
    long long line_start = GetAbsIdx(line_idx, 0);
    size_t word_num = word_idx + 1;
    size_t word_count = 0;
    long long word_start = -1;
    bool in_word = isSep(_map[line_start]);
    for (size_t i = line_start; i < _size; ++i) {
        if (_map[i] == '\n') {
            break;
        }
        if (in_word) {
            if (isSep(_map[i])) {
                in_word = false;
            }
        } else { // !in_word
            if (!isSep(_map[i])) {
                in_word = true;
                ++word_count;
                if (word_count == word_num) {
                    word_start = i;
                    break;
                }
            }
        }
    }
    return word_start;
}

long long Document::FindWordEndIdx(const size_t line_idx, const size_t word_idx) const {
    long long word_end = FindWordStartIdx(line_idx, word_idx);
    if (word_end < 0) {
        return word_end;
    }
    while (word_end < _size && _map[word_end] != '\n' && !isSep(_map[word_end])) {
        ++word_end;
    }
    return word_end - 1;
}

long long Document::GetWordLength(const size_t line_idx, const size_t word_idx) const {
    long long word_start = FindWordStartIdx(line_idx, word_idx);
    if (word_start < 0) {
        return -1;
    }
    long long word_end = word_start;
    while (word_end < _size && _map[word_end] != '\n' && !isSep(_map[word_end])) {
        ++word_end;
    }
    --word_end;
    
    return word_end - word_start + 1;
}

void Document::InsertWordAfter(const size_t line_idx, const size_t word_idx, const std::string& word) {
    long long word_end = FindWordEndIdx(line_idx, word_idx);
    if (word_end < 0) {
        throw std::runtime_error("No word with such index");
    }
    AbsInsertAfter(word_end, " " + word);
}

size_t Document::CountSpaceAfter(const size_t word_start, const size_t word_len) const {
    size_t res = 0;
    size_t idx = word_start + word_len;
    while (idx < _size && isSep(_map[idx])) {
        ++res;
        ++idx;
    }
    return res;
}

void Document::DeleteWord(const size_t line_idx, const size_t word_idx) {
    long long start_idx = FindWordStartIdx(line_idx, word_idx);
    if (start_idx < 0) {
        throw std::runtime_error("No word with such index");
    }

    size_t length = GetWordLength(line_idx, word_idx);
    length += CountSpaceAfter(start_idx, length);
    
    AbsDelete(start_idx, length);
}

void Document::ChangeWord(const size_t line_idx, const size_t word_idx, const std::string& word) {
    long long start = FindWordStartIdx(line_idx, word_idx);
    DeleteWord(line_idx, word_idx);
    AbsInsertAfter(start - 1, word);
}
