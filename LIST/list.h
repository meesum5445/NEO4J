#pragma once
#include<vector>
template <class K,class V>
class FUM;

template <class T>
class LIST
{
    public:
    FUM<T,T>list;
    LIST()
    {
        
    }
    LIST(int listNum)
    {
        FUM<T,T>listtemp(listNum);
        list=listtemp;
    }
    bool exist(T element)
    {
        return list.exist_in_alive(element);
    }
    unsigned int insert(T element)
    {
        return list.insert({element,element});
    }
    void remove(T element)
    {
        list.remove(element);
    }
    std::vector<T>get_list()
    {
        std::vector<T>li;
        auto KV =list.get_all_key_value();
        for(auto v:KV)
        {
            li.push_back(v.first);
        }
        return li;
    }
    unsigned int get_id_number(T element)
    {
        return list.get_id_number(element);
    }
    T get_by_id(long long id)
    {
        return list.get_key_by_id(id);
    }
};