#include "lru_evictor.hh"

//evict blocks in order they were added, without regard to how often they were accessed before.

LRU_Evictor::LRU_Evictor(){

}

LRU_Evictor::~LRU_Evictor() = default;


const key_type LRU_Evictor::evict(){ 
    if (values.empty() == false)
    {
        key_type lastkey = values.front();
        //check if memory address has been marked with while loop. if so, ignore it. return last thing that hasn't been marked. 
        values.pop_front();
        return lastkey;
    }
    return "";
}


void LRU_Evictor::touch_key(const key_type& key){  
    auto location  = std::find(values.begin(), values.end(), key);
    if (location != values.end())
    {
        values.erase(location);
    }
    
    values.push_back(key);
}
