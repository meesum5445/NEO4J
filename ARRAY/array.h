#include<array>
#pragma once
template<class T>
class ARRAY
{
    public:
        std::array<T,20>arr;
        unsigned int size{};
        void insert(T val)
        {
            arr[size++]=val;
        }
};