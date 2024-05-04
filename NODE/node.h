#pragma once
#include <cstring>
#include<array>
#include<fstream>

template <class K>
class ARRAY;

template <class K,class V>
class FUM;

template <class K>
class LIST;

class NODE
{
    public:
        LIST<NODE>storageLocation;
        unsigned int nodeId=-1;
        FUM<unsigned int,unsigned int>attributes;            
        //FUM of  (attribute type ID,attribute value ID)             

        FUM<unsigned int,std::pair<unsigned int,FUM<unsigned int,unsigned int>>>relatedTo;
        //FUM of (node ID || (relation type ID || FUM of (attribute type ID|| attribute value ID)))



    NODE()
    {
    }
    NODE(long long nodeNum,LIST<NODE> sL,unsigned int &countOfNodes)
    {
        storageLocation=sL;
        if(nodeNum==-1)
        {
            this->nodeId=countOfNodes++;
            this->attributes=FUM<unsigned int,unsigned int>(-1);
            this->relatedTo=FUM<unsigned int,std::pair<unsigned int,FUM<unsigned int,unsigned int>>>(-1);
            storageLocation.insert(*this);
        }
        else
        {
            *this=storageLocation.get_by_id(nodeNum);
        }
        
    }
    bool operator ==(NODE k)
    {
        return this->nodeId==k.nodeId;
    }
};