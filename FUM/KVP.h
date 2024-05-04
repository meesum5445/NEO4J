#pragma once
#include <iostream>
#include <cstring>
#include<array>
#include<string>


template<class T>
class BYTEARRAY
{
    public:
    std::array<char,100>key{};
    int keySize{};
    
    void assignIN(T obj)
    {
        std::memcpy(key.data(),(char*)&obj, sizeof(obj));
        keySize=sizeof(obj);
    }
    void assignOUT(T &obj)
    {
        std::memcpy((char*)&obj,key.data(), this->keySize);
    }
};
template<>
void BYTEARRAY<std::string>::assignIN(std::string obj)
{
    std::memcpy(key.data(),obj.data(), obj.size());
    keySize=obj.size();
}
template<>
void BYTEARRAY<std::string>::assignOUT(std::string &obj)
{
    obj.resize(this->keySize);
    std::memcpy(obj.data(),key.data(), this->keySize);
}
//....................................

template<class K,class V>
class KVP
{
    public:
        bool alive{};
        BYTEARRAY<K>key;
        BYTEARRAY<V>val;
};