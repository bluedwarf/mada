/*
 * MappedArray.hpp
 * Copyright (C) 2009 Takashi Nakamoto <bluedwarf@bpost.plala.or.jp>.
 *
 * This program is part of MaDa Double Array library.
 *
 * MaDa Double Array library is free software: you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation, either version 3
 * of the License, or (at your option) any later version.
 *
 * MaDa Double Array library is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with MaDa Double Array library. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef _MADA_MAPPED_ARRAY_HPP_
#define _MADA_MAPPED_ARRAY_HPP_

#define INITIAL_MAPPED_SIZE (4096)
//#define INITIAL_MAPPED_SIZE (4096*25)
#define RESIZE_SIZE (4096)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

namespace mada
{
template <class T> class MappedArray
{
private:
    T* array;
    int fd;
    size_t mapped_size;
//    size_t size;

    // Copy is forbidden.
    MappedArray(const MappedArray &a);
    MappedArray &operator=(const MappedArray &a);

    void resize(size_t new_size) throw (int);
public:
    MappedArray(const char *filename) throw (int);
    ~MappedArray() throw (int);

    void expand_to(size_t size) throw (int);
    void clear() throw (int);
    void truncate(size_t size) throw (int);
    T &operator[](size_t i) throw (int);
};

template <class T>
MappedArray<T>::MappedArray (const char *filename) throw (int)
{
    struct stat st;

    if (stat(filename, &st) != 0 || st.st_size == 0) {
        // The specified file doesn't exist. Create a new file.

        if ((fd = open(filename, O_RDWR | O_CREAT, 0666)) == -1)
            throw 1; // Failed to create the specified file.

        if (lseek(fd, INITIAL_MAPPED_SIZE * sizeof(T), SEEK_SET) < 0) {
            close(fd);
            throw 2; // Failed to expand the file size.
        }

        T c;
        if (read(fd, &c, sizeof(T)) == -1)
            c = 0;

        if (write(fd, &c, sizeof(T)) == -1) {
            close(fd);
            throw 3; // Failed to write a new value.
        }

        mapped_size = INITIAL_MAPPED_SIZE;
//	size = 0;
    } else {
        // Open the existing file as an array.

//	mapped_size = size = st.st_size / sizeof(T);
	mapped_size = st.st_size / sizeof(T);
        if ((fd = open(filename, O_RDWR)) == -1)
            throw 4; // Failed to open the specified file.
    }

    array = (T*) mmap(NULL, mapped_size * sizeof(T), PROT_READ | PROT_WRITE,
                      MAP_SHARED, fd, 0);
    if (array == MAP_FAILED) {
        close(fd);
        throw 5; // Failed to map the specified file to an array.
    }
}

template <class T>
MappedArray<T>::~MappedArray() throw (int)
{
    if (msync(array, mapped_size * sizeof(T), 0) == -1)
        throw 1; // Failed to write the content of array to file.

    if (munmap(array, mapped_size * sizeof(T)) == -1)
        throw 2; // Failed to release the allocated array.

    if (close(fd) == -1)
        throw 3; // Failed to close the specified file.
}

template <class T>
void MappedArray<T>::truncate(size_t size) throw (int)
{
    if (ftruncate(fd, size * sizeof(T)) == -1)
	throw 1; // Failed to truncate the file size.
}

template <class T>
void MappedArray<T>::clear() throw (int)
{
    if (munmap (array, mapped_size * sizeof(T)) == -1)
        throw 1; // Failed to release the allocated array.

    if (ftruncate (fd, 0) == -1)
        throw 2; // Failed to truncate the file size.

    if (lseek (fd, INITIAL_MAPPED_SIZE * sizeof(T), SEEK_SET) < 0)
        throw 3; // Failed to expand the file size.

    T c;
    if (read (fd, &c, sizeof(T)) == -1)
        c = 0;

    if (write (fd, &c, sizeof(T)) == -1)
        throw 4; // Failed to write a new value.

    mapped_size = INITIAL_MAPPED_SIZE;
//    size = 0;

    array = (T *) mmap(NULL, mapped_size * sizeof(T), PROT_READ | PROT_WRITE,
                       MAP_SHARED, fd, 0);
    if (array == MAP_FAILED)
        throw 5; // Failed to map the specified file to an array.
}

template <class T>
inline void MappedArray<T>::resize(size_t new_size) throw (int)
{
    if (mapped_size >= new_size)
        return;

    if (msync (array, mapped_size * sizeof(T), 0) == -1)
        throw 1; // Failed to write the content of array to file.

    if (munmap (array, mapped_size * sizeof(T)) == -1)
        throw 2; // Failed to release the allocated array.

    while (mapped_size < new_size)
        mapped_size += RESIZE_SIZE;

    if (lseek (fd, mapped_size * sizeof(T), SEEK_SET) < 0)
        throw 3; // Failed to expand the file size.
    // Note that: this makes sure that the new allocated memories
    //            are initialized by 0.

    T c;
    if (read(fd, &c, sizeof(T)) == -1)
        c = 0;

    if (write(fd, &c, sizeof(T)) == -1)
        throw 4; // Failed to write a new value.

    array = (T *) mmap(NULL, mapped_size * sizeof(T), PROT_READ | PROT_WRITE,
                       MAP_SHARED, fd, 0);

    if (array == MAP_FAILED)
        throw 5; // Failed to remap the specified file to an array.
}

template <class T>
void MappedArray<T>::expand_to (size_t size) throw (int)
{
    if (mapped_size <= size) {
	resize(size+1);
    }
}

template <class T>
T &MappedArray<T>::operator[] (size_t i) throw (int)
{
//    if (i >= size) {
//        if (mapped_size <= i) {
//            try {
//                resize(i+1);
//            } catch(int e) {
//                throw e;
//            }
//        }

//        for (int j=size; j<=i; j++)
//            array[j] = 0;

//        size = i + 1;
//    }

    return array[i];
}

}

#endif // _MADA_MAPPED_ARRAY_HPP_
