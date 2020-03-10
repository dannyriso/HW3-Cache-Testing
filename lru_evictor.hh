#include "evictor.hh"
#include <vector>
#include <list>
#include <deque>
#include <queue>
#include <algorithm>

using namespace std;

class LRU_Evictor:public Evictor
{
private:
    //least recently used
   deque<key_type> values;
public:
    const key_type evict();
    void touch_key(const key_type&);
    LRU_Evictor();
    ~LRU_Evictor();
};
