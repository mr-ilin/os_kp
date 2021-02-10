//
//  File_hdl.hpp
//  os_kp_Xcode
//
//  Created by Илья Ильин on 28.12.2020.
//


#pragma once
#include <string>
#include <stdexcept>
#include <errno.h>
#include <sys/mman.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>

DIR* open_directory(const std::string& dir_path);
void close_directory(DIR* dir);
int access_file(const std::string& file_path, int mode);

int   open_file(const std::string& path, int mode);
void  close_file(const int fd);
off_t get_file_size(const int fd);
mode_t get_file_mode(const std::string& path);

void* map_file_to_edit(int fd, size_t size, off_t offset);
void  unmap_file(void* address, size_t size);
void* remap_file(const char* path, void *old_address, size_t old_size, size_t new_size);
