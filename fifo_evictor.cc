#include "fifo_evictor.hh"

//evict blocks in order they were added, without regard to how often they were accessed before.
Fifo_Evictor::Fifo_Evictor(){

}

Fifo_Evictor::~Fifo_Evictor() = default; //may be able to be lazy with this?

const key_type Fifo_Evictor::evict(){ //Have to find a way to deal with potnetially getting back keys from the evictor
    // This causes a bug where program behaves incorrectly, does not handle when thing is deleted? Not just fifo, but perhaps all evictors?
    // Since we CAN'T account for deleting without changing the header file... I think there's no way out of this. Not without editing the header file (or perhaps some painful memory manipulation). 
    if (values.empty() == false)
    {
        key_type lastkey = values.front();
        //check if memory address has been marked with while loop. if so, ignore it. return last thing that hasn't been marked. 
        values.pop();
        return lastkey;
    }
    return values.front();
}


void Fifo_Evictor::touch_key(const key_type& key){  //worry about same key/overwriting a key
    //touch key. if key is already in queue, remove it and then put it back via pushing it
    values.push(key);
}

