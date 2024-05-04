#pragma once
#include<iostream>
#include<array>
#include<vector>
#include<string>
#include <fstream>
#include<filesystem>
namespace fs =std::filesystem;
template<class K,class V>
class KVP;
template<class T>
class HASH;

template<class K,class V>
class FUM
{
    private:
        int FUMNumber{};
    public:
        FUM()
        {
            FUMNumber=-1;
        }
        FUM(int FUMNum)
        {
            if(FUMNum==-1)
            {
                unsigned int countOfFUM{};
                std::ifstream rdr("FUM//countOfFUM.txt");      
                rdr>>countOfFUM;      
               // rdr.read((char*)&countOfFUM,sizeof(countOfFUM));

                    
                
                fs::create_directory("FUM//UMP_"+std::to_string(++countOfFUM));
                this->FUMNumber=countOfFUM;
        
                std::ofstream wrt("FUM//countOfFUM.txt");
                wrt<<countOfFUM;
                //wrt.write((char*)&countOfFUM,sizeof(countOfFUM));
            }
            else
                this->FUMNumber=FUMNum;
        }
        unsigned int hashFunction(K key)
        {
            HASH<K> hasher;
            return hasher.hash_function(&key);
        }
        unsigned int insert(std::pair<K,V>KV)
        {
            KVP<K,V> KVPobj;
            KVPobj.alive=true;;
            KVPobj.key.assignIN(KV.first);
            KVPobj.val.assignIN(KV.second);

            
            unsigned int hashIndex=hashFunction(KV.first);
            KVP<K,V> KVPtemp;
            do
            {               
                if(std::filesystem::exists("FUM//UMP_"+std::to_string(this->FUMNumber)+"//"+std::to_string(hashIndex)+".bin"))
                {
                    std::ifstream rdr("FUM//UMP_"+std::to_string(this->FUMNumber)+"//"+std::to_string(hashIndex)+".bin"); 
                    rdr.read((char*)&KVPtemp,sizeof(KVP<K,V>));
                    K keytemp;
                    KVPobj.key.assignOUT(keytemp);
                    if(keytemp==KV.first)
                        break;
                    hashIndex*=2;
                } 
                else
                    break;           
            } while (true);
            std::ofstream wrt("FUM//UMP_"+std::to_string(this->FUMNumber)+"//"+std::to_string(hashIndex)+".bin"); 
            wrt.write((char*)&KVPobj,sizeof(KVP<K,V>));   
            return hashIndex;       
        }
        V operator[](K key)
        {
            unsigned int hashIndex=hashFunction(key);
            KVP<K,V> KVPtemp;
            do
            {               
                if(std::filesystem::exists("FUM//UMP_"+std::to_string(this->FUMNumber)+"//"+std::to_string(hashIndex)+".bin"))
                {
                    std::ifstream rdr("FUM//UMP_"+std::to_string(this->FUMNumber)+"//"+std::to_string(hashIndex)+".bin"); 
                    rdr.read((char*)&KVPtemp,sizeof(KVP<K,V>));
                    K keytemp;
                    KVPtemp.key.assignOUT(keytemp);
                    if(KVPtemp.alive && keytemp==key)
                    {
                        break;
                    }
                    hashIndex*=2;
                } 
                else
                {
                    V emptyobj;
                    return emptyobj;
                }         
            } while (true);
            V valtemp;
            KVPtemp.val.assignOUT(valtemp);
            return valtemp;
        }
        void remove(K key)
        {
            unsigned int hashIndex=hashFunction(key);
            KVP<K,V> KVPtemp;
            do
            {               
                if(std::filesystem::exists("FUM//UMP_"+std::to_string(this->FUMNumber)+"//"+std::to_string(hashIndex)+".bin"))
                {
                    //if file found
                    std::ifstream rdr("FUM//UMP_"+std::to_string(this->FUMNumber)+"//"+std::to_string(hashIndex)+".bin"); 
                    rdr.read((char*)&KVPtemp,sizeof(KVP<K,V>));

                    K keytemp;
                    KVPtemp.key.assignOUT(keytemp);
                    if(keytemp==key)
                    {
                        if(!KVPtemp.alive)
                            return;
                        else
                            break;
                    }
                    hashIndex*=2;
                } 
                else
                {
                    //if file not found
                    return;
                }         
            } while (true);
            KVPtemp.alive=false;
            std::ofstream wrt("FUM//UMP_"+std::to_string(this->FUMNumber)+"//"+std::to_string(hashIndex)+".bin"); 
            wrt.write((char*)&KVPtemp,sizeof(KVP<K,V>));         
        }
        long long get_id_number(K key)
        {
            unsigned int hashIndex=hashFunction(key);
            KVP<K,V> KVPtemp;
            do
            {               
                if(std::filesystem::exists("FUM//UMP_"+std::to_string(this->FUMNumber)+"//"+std::to_string(hashIndex)+".bin"))
                {
                    //if file found
                    std::ifstream rdr("FUM//UMP_"+std::to_string(this->FUMNumber)+"//"+std::to_string(hashIndex)+".bin"); 
                    rdr.read((char*)&KVPtemp,sizeof(KVP<K,V>));

                    K keytemp;
                    KVPtemp.key.assignOUT(keytemp);
                    if(keytemp==key)
                    {
                        if(!KVPtemp.alive)
                            return -1;
                        else
                            break;
                    }
                    hashIndex*=2;
                } 
                else
                {
                    //if file not found
                    return -1;
                }         
            } while (true);
            return hashIndex;
        }
        K get_key_by_id(long long id)
        {
            K key;
            KVP<K,V> KVPtemp;
            if(std::filesystem::exists("FUM//UMP_"+std::to_string(this->FUMNumber)+"//"+std::to_string(id)+".bin"))
            {
                std::ifstream rdr("FUM//UMP_"+std::to_string(this->FUMNumber)+"//"+std::to_string(id)+".bin"); 
                rdr.read((char*)&KVPtemp,sizeof(KVP<K,V>));
                KVPtemp.key.assignOUT(key);
            }          
            return key;
        }
        V get_value_by_id(long long id)
        {
            unsigned int hashIndex=id;
            KVP<K,V> KVPtemp;
            do
            {               
                if(std::filesystem::exists("FUM//UMP_"+std::to_string(this->FUMNumber)+"//"+std::to_string(hashIndex)+".bin"))
                {
                    std::ifstream rdr("FUM//UMP_"+std::to_string(this->FUMNumber)+"//"+std::to_string(hashIndex)+".bin"); 
                    rdr.read((char*)&KVPtemp,sizeof(KVP<K,V>));
                    if(KVPtemp.alive)
                    {
                        break;
                    }
                    hashIndex*=2;
                } 
                else
                {
                    V emptyobj;
                    return emptyobj;
                }         
            } while (true);
            V valtemp;
            KVPtemp.val.assignOUT(valtemp);
            return valtemp;
        }
        bool exist_in_alive(K key)
        {
             unsigned int hashIndex=hashFunction(key);
            KVP<K,V> KVPtemp;
            do
            {               
                if(std::filesystem::exists("FUM//UMP_"+std::to_string(this->FUMNumber)+"//"+std::to_string(hashIndex)+".bin"))
                {
                    //if file found
                    std::ifstream rdr("FUM//UMP_"+std::to_string(this->FUMNumber)+"//"+std::to_string(hashIndex)+".bin"); 
                    rdr.read((char*)&KVPtemp,sizeof(KVP<K,V>));

                    K keytemp;
                    KVPtemp.key.assignOUT(keytemp);
                    if(keytemp==key)
                    {
                        if(!KVPtemp.alive)
                            return false;
                        else
                            break;
                    }
                    hashIndex*=2;
                } 
                else
                {
                    //if file not found
                    return false;
                }         
            } while (true);
            return true;
        }
        std::vector<std::pair<K,V>>get_all_key_value()
        {
            if(this->FUMNumber==-1)
                return {};
            std::vector<std::pair<K,V>>KandV;
            for (const auto& entry : fs::directory_iterator("FUM//UMP_"+std::to_string(this->FUMNumber))) 
            {
                if (entry.is_regular_file() && entry.path().extension() == ".bin") 
                {
                    std::ifstream rdr(entry.path().string());
                    KVP<K,V> KVPtemp;
                    rdr.read((char*)&KVPtemp,sizeof(KVP<K,V>));
                    if(KVPtemp.alive){
                        K keytemp;
                        V valtemp;
                        KVPtemp.key.assignOUT(keytemp);
                        KVPtemp.val.assignOUT(valtemp);
                        KandV.push_back({keytemp,valtemp});
                    }
                    
                }
                
                
            }
            return KandV;
        }
        unsigned int get_fum_num()
        {
            return this->FUMNumber;
        }
        void set_fum_num(unsigned int n)
        {
            this->FUMNumber=n;
        }
        
};
template<class K,class V>
std::ofstream& operator<<(std::ofstream& out,FUM<K,V> writer) {
    out<<writer.get_fum_num();
    return out;
}

template<class K,class V>
std::ifstream& operator>>(std::ifstream& in, FUM<K,V> &reader) {
    unsigned int t;
    in>>t;
    reader.set_fum_num(t);

    return in;
}