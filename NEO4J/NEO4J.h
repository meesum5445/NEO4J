#pragma once
#include <cstring>
#include<array>
#include<algorithm>
#include<fstream>
#include <unordered_map>
#include<set>
#include"../ARRAY/array.h"
#include "../FUM/KVP.h"
#include "../FUM/FUM.h"
#include"../LIST/list.h"
#include"../NODE/node.h"
#include"../HASH/hash.h"




std::vector<unsigned int> intersection_of_vectors(const std::vector<std::vector<unsigned int>>& vectors) 
{
    if (vectors.empty()) {
        return std::vector<unsigned int>();
    }

    std::vector<std::vector<unsigned int>> sorted_vectors;
    for (const auto& vec : vectors) {
        std::vector<unsigned int> sorted_vec = vec;
        std::sort(sorted_vec.begin(), sorted_vec.end());
        sorted_vectors.push_back(sorted_vec);
    }

    std::vector<unsigned int> result_vector = sorted_vectors[0];
    for (size_t i = 1; i < sorted_vectors.size(); ++i) {
        std::vector<unsigned int> temp_result;
        std::set_intersection(
            result_vector.begin(), result_vector.end(),
            sorted_vectors[i].begin(), sorted_vectors[i].end(),
            std::back_inserter(temp_result)
        );
        result_vector = temp_result;
    }

    return result_vector;
}

class NEO4J
{
    private:    
        std::vector<unsigned int> get_nodes_with_attribute(std::string type,std::string value)
        {
            return attributesList[type][value].get_list();
        }
        bool compare_two_vectors(std::vector<std::pair<unsigned int,unsigned int>>a,std::vector<std::pair<unsigned int,unsigned int>>b)
        {
            for(auto v1:a)
            {
                bool check=false;
                for(auto v2:b)
                {
                    if(v1==v2)
                        check=true;
                }
                if(!check)
                    return false;
            }
            return true;
        }
        unsigned int count_of_database()
        {
            unsigned int t=0;
            std::ifstream rdr("NEO4J//neo4j_count.bin");
            if(!rdr)
            {
                std::ofstream wrt("NEO4J//neo4j_count.bin");
                wrt.write((char*)&(++t),sizeof(unsigned int));
                return t-1;
            }            
            rdr.read((char*)&t,sizeof(unsigned int));
            std::ofstream wrt("NEO4J//neo4j_count.bin");
            wrt.write((char*)&(++t),sizeof(unsigned int));
            return t-1;
        }

    public:
        unsigned int databaseId=-1;
        unsigned int countOfNodes=0;
        LIST<NODE>nodes;
        FUM<std::string,FUM<std::string,LIST<unsigned int>>>attributesList;

        LIST<std::string>relationshipTypeList;
        LIST<std::string>relationshipAttrTypeList;
        LIST<std::string>relationshipAttrValueList;

    NEO4J()
    {

    }  
    NEO4J(unsigned int dbId)
    {
        std::ifstream rdr("NEO4J//neo4j_"+std::to_string(dbId)+".bin");
        if(rdr)
        {
            rdr.read((char*)this,sizeof(*this));
        }
        else
        {
            nodes=LIST<NODE>(-1);
            attributesList= FUM<std::string,FUM<std::string,LIST<unsigned int>>>(-1);
            relationshipTypeList=LIST<std::string>(-1);

            relationshipAttrTypeList=LIST<std::string>(-1);

            
            relationshipAttrValueList=LIST<std::string>(-1);
            this->databaseId=count_of_database();
            std::ofstream wrt("NEO4J//neo4j_"+std::to_string(this->databaseId)+".bin");
            wrt.write((char*)this,sizeof(*this)); 
        }
    }
    void readCSVNodes(std::string filePath)
    {
        std::vector<std::pair<std::string,std::string>> attributes;
        std::ifstream rdr(filePath);

        std::string csvLine;
        std::getline(rdr, csvLine);
        std::istringstream stream(csvLine);


        std::string word;
        bool first=true;
        while (std::getline(stream, word, ',')) {
            if(first)
            {
                word=word.substr(3);
                first=false;
            }
            attributes.push_back({word,""});
        }
        
        while(std::getline(rdr, csvLine))
        {
            int i=0;
            std::istringstream stream(csvLine);
            std::string word;
            while (std::getline(stream, word, ',')) {
                attributes[i++].second=word;
            }
            
            this->CREATENODE("","",attributes);
        }

    }
    void readCSVrelations(std::string filePath)
    {
        std::vector<std::pair<std::string,std::string>> attributes;

        std::ifstream rdr(filePath);

        std::string csvLine;
        std::getline(rdr, csvLine);
        std::istringstream stream(csvLine);
        stream.ignore(3);
        std::string word;
        int i=0;
        while (std::getline(stream, word, ',')) {
            if(i++>2)
                attributes.push_back({word,""});
        }

        while(std::getline(rdr, csvLine))
        {
            
            std::istringstream stream(csvLine);
            std::string word;
            unsigned int n1,n2;
            std::string relation;
            int i=0;
            while (std::getline(stream, word, ',')) {
                if(i==0)
                    n1=std::stoi(word);
                else if(i==1)
                    n2=std::stoi(word);
                else if(i==2)
                    relation=word;
                if(i>2)
                {
                    attributes[(i-3)].second=word;
                }
                i++; 
            }
            this->CREATERELATION(n1,n2,relation,attributes);
        }
    }
    int CREATENODE(std::string variableName,std::string familyType,std::vector<std::pair<std::string,std::string>>attr)
    {
        if(familyType.size()!=0)
        {
            attr.push_back({"family",familyType});
        }
        
        NODE temp(-1,this->nodes,this->countOfNodes);
        //.................................................
        for(auto ATT:attr)
        {
            if(this->attributesList.exist_in_alive(ATT.first))
            {
                if(attributesList[ATT.first].exist_in_alive(ATT.second))
                {
                    attributesList[ATT.first][ATT.second].insert(temp.nodeId);
                }
                else
                {
                    LIST<unsigned int>l1(-1);
                    l1.insert(temp.nodeId);
                    attributesList[ATT.first].insert({ATT.second,l1});
                }

                
            }
            else
            {
                LIST<unsigned int>l1(-1);
                l1.insert(temp.nodeId);
                FUM<std::string,LIST<unsigned int>>f1(-1);
                f1.insert({ATT.second,l1});
                attributesList.insert({ATT.first,f1});
            }
            temp.attributes.insert({attributesList.get_id_number(ATT.first),attributesList[ATT.first].get_id_number(ATT.second)});
        }
        return temp.nodeId;
    }
    void CREATERELATION_BETWEEN_LIST(std::vector<unsigned int>l1,std::vector<unsigned int>l2,std::string relation,std::vector<std::pair<std::string,std::string>>attr)
    {
        for(auto a:l1)
        {
            for(auto b:l2)
            {
                CREATERELATION(a,b,relation,attr);
            }
        }
    }
    void CREATERELATION(unsigned int a,unsigned int b,std::string relation,std::vector<std::pair<std::string,std::string>>attr)
    {
        NODE n1(a,this->nodes,this->countOfNodes),n2(b,this->nodes,this->countOfNodes);
        FUM<unsigned int,unsigned int> relTypeTemp(-1);
        for(auto v:attr)
        {
            relTypeTemp.insert({this->relationshipAttrTypeList.insert(v.first),this->relationshipAttrValueList.insert(v.second)});
        }
        n1.relatedTo.insert({n2.nodeId,{relationshipTypeList.insert(relation),relTypeTemp}});
        //..............................
    }   
    std::vector<unsigned int> search(std::string variableName,std::string familyType,std::vector<std::pair<std::string,std::string>>attr)
    {
        if(familyType.size()!=0)
        {
            attr.push_back({"family",familyType});
        }
        std::vector<std::vector<unsigned int>> all_lists;
        if(attr.size()==0)
        {
            return get_all_nodes_ids();
        }
        for(auto v:attr)
        {
            all_lists.push_back(this->get_nodes_with_attribute(v.first,v.second));
        }              
        return intersection_of_vectors(all_lists);
    }
    std::vector<unsigned int> As_related_to_Bs(std::vector<unsigned int> a,std::vector<unsigned int> b,std::string relation,std::vector<std::pair<std::string,std::string>>attr)
    {
        std::vector<std::pair<unsigned int,unsigned int>>attrInNums;
        for(auto v:attr)
        {
            attrInNums.push_back({this->relationshipAttrTypeList.get_id_number(v.first),this->relationshipAttrValueList.get_id_number(v.second)});
        }
        std::set<unsigned int>S;
        for(auto aPicked:a)
        {
                
                NODE pickedNodeFrom_a(aPicked,this->nodes,this->countOfNodes);
                bool check=false;
                for(auto bPicked:b)
                {
                    auto relatedToTemp=pickedNodeFrom_a.relatedTo.get_all_key_value();

                    for(int i=0;i<relatedToTemp.size();i++)
                    {
                        if(relatedToTemp[i].first==bPicked && (relation.size()>0?relatedToTemp[i].second.first==this->relationshipTypeList.get_id_number(relation):true))
                        {
                            if(compare_two_vectors(attrInNums,relatedToTemp[i].second.second.get_all_key_value()))
                            {
                                std::cout<<aPicked;
                                S.insert(aPicked);
                                check=true;
                                break;
                            }
                            
                        }
                    }
                    if(check)
                    break;
                }
            
        }
        auto temp=std::vector<unsigned int>(S.begin(),S.end());
        return temp;
    }
    std::vector<unsigned int> Bs_related_by_As(std::vector<unsigned int> a,std::vector<unsigned int> b,std::string relation,std::vector<std::pair<std::string,std::string>>attr)
    {
        std::vector<std::pair<unsigned int,unsigned int>>attrInNums;
        for(auto v:attr)
        {
            attrInNums.push_back({this->relationshipAttrTypeList.get_id_number(v.first),this->relationshipAttrValueList.get_id_number(v.second)});
        }
        std::set<unsigned int>S;
        for(auto aPicked:a)
        {
                
                NODE pickedNodeFrom_a(aPicked,this->nodes,this->countOfNodes);
                for(auto bPicked:b)
                {
                    auto relatedToTemp=pickedNodeFrom_a.relatedTo.get_all_key_value();
                    for(int i=0;i<relatedToTemp.size();i++)
                    {
                        if(relatedToTemp[i].first==bPicked && (relation.size()>0?relatedToTemp[i].second.first==this->relationshipTypeList.get_id_number(relation):true))
                        {
                            if(compare_two_vectors(attrInNums,relatedToTemp[i].second.second.get_all_key_value()))
                            {
                                std::cout<<bPicked;
                                S.insert(bPicked);
                                break;
                            }
                            
                        }
                    }
                }
            
        }
        auto temp=std::vector<unsigned int>(S.begin(),S.end());
        return temp;
    }
    std::vector<char> get_attributes_of(unsigned int id)
    {
        std::vector<char>Rtemp;
        NODE temp(id,this->nodes,this->countOfNodes);
        auto attributes=temp.attributes.get_all_key_value();
        for(auto attr:attributes)
        {
            auto attType=this->attributesList.get_key_by_id(attr.first);
            for(int i=0;i<attType.size();i++)
            {
                std::cout<<attType[i]<<",";
                Rtemp.push_back(attType[i]);
            }
            Rtemp.push_back(':');
            auto attVal=this->attributesList[this->attributesList.get_key_by_id(attr.first)].get_key_by_id(attr.second);
            for(int i=0;i<attVal.size();i++)
            {
                std::cout<<attVal[i]<<",";
                Rtemp.push_back(attVal[i]);
            }
            Rtemp.push_back(',');
        }
        return Rtemp;
    }
    std::vector<unsigned int>get_all_nodes_ids()
    {
         std::vector<unsigned int>nodesId;
            auto nodes= this->nodes.get_list();
            for(auto n:nodes)
                nodesId.push_back(n.nodeId);
            return nodesId;
    }
    void delete_nodes(std::vector<unsigned int>list)
    {
        for(auto l:list)
        {
            NODE tempnode(l,this->nodes,this->countOfNodes);
            this->nodes.remove(tempnode);
            auto tempattr=tempnode.attributes.get_all_key_value();
            for(auto t2:tempattr)
            {
                this->attributesList.get_value_by_id(t2.first).get_value_by_id(t2.second).remove(l);
            }
        }
        
    }
    void remove_edges(std::vector<unsigned int>list1,std::vector<unsigned int>list2,std::string relation,std::vector<std::pair<std::string,std::string>>attr)
    {
        for(auto l1:list1)
        {
            for(auto l2:list2)
            {
                std::cout<<"CHECKING CONDITION "<<l1<<" to "<<l2<<"\n";
                if(As_related_to_Bs({l1},{l2},relation,attr).size()==1)
                {
                    
                    NODE tempNode(l1,this->nodes,this->countOfNodes);
                    std::cout<<"\nGONNA DELETE RELATION "<<l1<<" to "<<l2<<"\n";
                    tempNode.relatedTo.remove(l2);
                    std::cout<<"RELATION "<<l1<<" to "<<l2<<" deleted\n";
                }
            }
        }
    }
    ~NEO4J()
    {
        std::ofstream wrt("NEO4J//neo4j_"+std::to_string(this->databaseId)+".bin");
        wrt.write((char*)this,sizeof(*this)); 
    }
};
