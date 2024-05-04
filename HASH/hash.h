#pragma once
#include<string>
class NODE;
template<class K>
class HASH
{
    public:
        unsigned int hash_function(K *key);
};
template<>
unsigned int HASH<unsigned int>::hash_function(unsigned int *key)
{
    return *key;
}
template<>
unsigned int HASH<std::string>::hash_function(std::string* key)
{
    std::hash<std::string> hasher;
    return hasher(*key);
}

template<>
unsigned int HASH<NODE>::hash_function(NODE * key)
{
    return key->nodeId;
}