/*
 * KeySet.hpp
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

#ifndef _MADA_KEY_SET_HPP_
#define _MADA_KEY_SET_HPP_

namespace mada
{

template <class T> class KeySet
{
private:
    size_t a_size;
    T max_key;
    T *array;

public:
    KeySet(T max_key);
    ~KeySet();

    void clear();
    void push_back(T c);
    void pop_back();
    size_t size();
    T &operator[](size_t i);
};

template <class T>
KeySet<T>::KeySet (T max_key)
{
    this->max_key = max_key;
    array = new T [max_key];

    this->a_size = 0;
}

template <class T>
KeySet<T>::~KeySet ()
{
    delete [] array;
}

template <class T>
void KeySet<T>::clear ()
{
    this->a_size = 0;
}

template <class T>
size_t KeySet<T>::size ()
{
    return this->a_size;
}


template <class T>
T &KeySet<T>::operator[] (size_t i)
{
    return array[i];
}

template <class T>
void KeySet<T>::push_back (T c)
{
    array[a_size] = c;
    a_size++;
}

template <class T>
void KeySet<T>::pop_back ()
{
    a_size--;
}

}

#endif // _MADA_KEY_SET_HPP_
