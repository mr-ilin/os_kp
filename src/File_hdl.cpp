//
//  File_hdl.cpp
//  os_kp_Xcode
//
//  Created by Илья Ильин on 28.12.2020.
//

#include "file_hdl.hpp"

#define ERR_ACCESS_NO_SUCH_FILE       124
#define ERR_ACCESS_NO_WR_PERMS        125
#define ERR_PERM_DENIED               126

DIR* open_directory(const std::string& dir_path) {
    DIR* to_return;
    if ((to_return = opendir(dir_path.data())) == NULL) {
        throw std::runtime_error("Error while opendir");
    }
    return to_return;
}

void close_directory(DIR* dir) {
    if (closedir(dir) == -1) {
        throw std::runtime_error("Error while closedir");
    }
}

int open_file(const std::string& path, int mode) {
    int fd = open(path.data(), mode);
    if (fd < 0) {
        throw std::runtime_error("Error while openning file");
    }
    return fd;
}

void close_file(const int fd) {
    if (close(fd) == -1) {
        throw std::runtime_error("Error while closing file");
    }
}

void get_file_statbuf(const std::string& path, struct stat * statbuf) {
    int err = stat(path.data(), statbuf);
    if (err < 0){
        throw std::runtime_error("Error while getting file stat");
    }
}

void get_file_statbuf(const int fd, struct stat * statbuf) {
    int err = fstat(fd, statbuf);
    if (err < 0){
        throw std::runtime_error("Error while getting file stat");
    }
}

off_t get_file_size(const int fd) {
    struct stat statbuf;
    get_file_statbuf(fd, &statbuf);
    return statbuf.st_size;
}

mode_t get_file_mode(const std::string& path) {
    struct stat statbuf;
    get_file_statbuf(path, &statbuf);
    return statbuf.st_mode;
}

// upon success return EXIT_SUCCESS (aka 0), either errno is returned
int access_file(const std::string& file_path, int mode) {
    if (access(file_path.data(), mode) == EXIT_SUCCESS) {
        return EXIT_SUCCESS;
    }
    return errno;
}

void* map_file_to_edit(int fd, size_t size, off_t offset) {
    void* mapped = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, offset);
    if (mapped == MAP_FAILED) {
        throw std::runtime_error("Error while mmap");
    }
    return mapped;
}

void unmap_file(void* address, size_t size) {
    if (munmap(address, size) == -1) {
        throw std::runtime_error("Error while unmapping file");
    }
}

void* remap_file(const char* path, void* old_address, size_t old_size, size_t new_size) {
    void* to_return;
#ifdef __APPLE__
    if (old_address != NULL) {
        unmap_file(old_address, old_size);
    }
    int fd = open_file(path, O_RDWR);
    ftruncate(fd, new_size);
    to_return = map_file_to_edit(fd, new_size, 0);
    close_file(fd);
    return to_return;
#else
    to_return = mremap(old_address, old_size, new_size, MREMAP_MAYMOVE);
    if (to_return == MAP_FAILED) {
        throw std::runtime_error("Error while mremap");
    }
#endif
    return to_return;
}
