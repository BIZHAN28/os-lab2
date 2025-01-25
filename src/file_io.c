#include "file_io.h"
#include "cache.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

int lab2_open(const char *path) {
    return open(path, O_CREAT | O_RDWR, 0644);
}

int lab2_close(int fd) {
    return close(fd);
}

ssize_t lab2_read(int fd, void *buf, size_t count) {
    off_t offset = lseek(fd, 0, SEEK_CUR);  // Получаем текущую позицию в файле
    CacheBlock *cached_block = cache_find(offset);
    
    if (cached_block) {
        memcpy(buf, cached_block->data, count);  // Читаем данные из кэша
        cache_move_to_front(cached_block);  // Перемещаем блок в начало
        return count;
    } else {
        ssize_t bytes_read = read(fd, buf, count);
        if (bytes_read > 0) {
            cache_add(offset, buf);
        }
        return bytes_read;
    }
}

ssize_t lab2_write(int fd, const void *buf, size_t count) {
    off_t offset = lseek(fd, 0, SEEK_CUR);
    CacheBlock *cached_block = cache_find(offset);
    
    if (cached_block) {
        memcpy(cached_block->data, buf, count);  // Обновляем данные в кэше
    } else {
        cache_add(offset, buf);  // Добавляем в кэш
    }

    return write(fd, buf, count);
}

off_t lab2_lseek(int fd, off_t offset, int whence) {
    return lseek(fd, offset, whence);
}

int lab2_fsync(int fd) {
    return fsync(fd);
}
