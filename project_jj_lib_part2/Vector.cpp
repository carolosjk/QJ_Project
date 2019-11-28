//
// Created by wmsksf on 28/11/19.
//

#include "Vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "MACROS.h"

Vector::Vector()
{
    vec = new uint64_t[1];
    ALLOC_CHECK(vec);

    capacity = 1;
    index = 0;
}

Vector::~Vector() { delete[] vec; }

uint64_t Vector::size() { return capacity; }

void Vector::push_back(uint64_t x)
{
    if (index == capacity)
    {
        uint64_t *tmp = new uint64_t[++capacity];
        ALLOC_CHECK(tmp);

        for (uint64_t i = 0; i < capacity; i++) tmp[i] = vec[i];
        delete[] vec;
        vec = tmp;
    }

    vec[index] = x;
    index++;
}

uint64_t& Vector::operator[](uint64_t indx)
{
    if (indx > capacity)
    {
        std::cerr << "Index out of boundaries." << std::endl;
        exit(EXIT_FAILURE);
    }

    return vec[indx];
}