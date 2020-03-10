// cache_lib.cc
// Written by Hannah Hellerstein & Danny Riso

#include "cache.hh"
#include <unordered_map>
#include <iostream>
#include <vector>
#include <tuple>
#include <cassert>

using namespace std;
using storage_unit = pair<Cache::val_type, Cache::size_type>;

/*
 * Notes from Eitan:
 *
 * You will implement your cache as a RAM-based storage of values, indexed by keys.
 * All keys and values will reside in main memory.
 * The cache needs to conform to the interface (API, or contract) that's provided in the attached cache.hh header file.
 * This API abstracts away the implementation of the cache while retaining a fixed interface.
 * You'll write a cache_lib.cc file that implements the interface, any way you see fit.
 * I recommend you start with the most trivial implementation you can think of---the quickest code you can write
 * that will compile and work all the functions---even if not terribly efficiently.
 */

//Need a hash table sooner or later
//Read through all the text of the assignment first

// Type definitions (if need defining beyond .hh's declaration):

// The link in cache.hh, https://www.fluentcpp.com/2017/09/22/make-pimpl-using-unique_ptr/,
// is super important to know what Impl is and how to implement it here
class Cache::Impl
{
	// Changed explicit types in unordered_map below to types defined in cache.hh
	
	//vector<key_type> holdkeys;
	// There is almost definitely a purpose for this vector, and I'm sorry for commenting it out,
	// but I can't find its purpose and it's giving me a compile error for trying to make a vector 
	// of consts. For now, I'm commenting it out.
	
	public:
	size_type maximummemory;
	size_type usedmemory;
	Evictor* evictor_;
	hash_func used_hasher; 
	unordered_map<key_type, storage_unit, hash_func> map;
	
	bool del(key_type key) {
		// Somehow, should be able to get the size of key's value, so we can subtract it
		// from usedmemory, since we're handling that manually
        if (map.find(key) != map.end()) //if Overwriting?
        {
			usedmemory -= map.at(key).second;
			delete[] map.at(key).first;   
			map.erase(key);

			//write something to key IF THE KEY IS NOT COPYING ITSELF TO NEW LOCATION. Something for the evictor to recognize. 
			// MAKE SURE THAT WRITING TO THE KEY (with a ptr) DOESN'T COPY. DEFREFRENCING IT MAY TRIGGER COPY. 
			return true;
        }
        return false;
	}
	
	void set(key_type key, val_type val, size_type size) {
		// While there isn't enough space for the new pair, evict something and then go on
        //check if standard string CAN be deep copied. They're defined to behave as if they copy.  
        byte_type *copyavalue = new byte_type[size];
		int j =0;
		while (val[j] != '\0')
		{
			copyavalue[j] = val[j];
			j++;
		}
		if (map.find(key) != map.end()) //If two of same key overwrite
        {
			del(key);
        }
		if (usedmemory + size <= maximummemory)
		{
			storage_unit onepair = make_pair(copyavalue, size);
			map[key] = onepair; //might make things a bit iffy?
			usedmemory += size;
			map.reserve(map.size()); //rehashes it s.t. it has optimal bucket size for its size. May change order of iterations, shouldn't lose info.
			if (evictor_ != nullptr)
			{
				evictor_->touch_key(key);
			}
			
		}
		else //eviction cases. Default is get rid of first thing in map.
		{
			if (evictor_ == nullptr) //if nullptr, cache does not replace old items and disallows insertions that exceed maxmem
			{
				//Nothing happens
				delete[] copyavalue;
			}
			else{
				while (usedmemory + size > maximummemory)
				{
					//Fix this st. if the thing given by evictor isn't there, give call until it isn't.
					auto evictoristhere = del(evictor_->evict());
					while (!evictoristhere)
					{
						evictoristhere = del(evictor_->evict());
					}
					
				}
				storage_unit onepair = make_pair(copyavalue, size);
				map[key] = onepair; //might make things a bit iffy?
				usedmemory += size;
				map.reserve(map.size()); //rehashes it s.t. it has optimal bucket size for its size. May change order of iterations, shouldn't lose info.
					
			}
		}
	}
	
	
	val_type get(key_type key, size_type& val_size) const{
		
		if (map.find(key) == map.end()){
			return nullptr;
		}
		if (evictor_ != nullptr)
		{
			evictor_->touch_key(key);
		}
        val_size = map.at(key).second;
		return map.at(key).first;
		// Use map.at(key) instead of map[key] because .at() returns a reference, which is good
		// because val_type is a pointer.
        // Sets the actual size of the returned value (in bytes) in val_size.
		
		// Still not totally clear on what val_size is used for in this method
	}
	
	size_type space_used() const{
		return usedmemory;
	}
	
	void reset(){
		auto i = map.begin();
		while (i != map.end())
		{
			del(i->first);
			i = map.begin();
		}
		
		usedmemory = 0;
		map.clear();
		// Holdover from commenting out holdkeys from the constructor
		maximummemory = 0;
	}
};

// Constructor & Destructor:

// Create a new cache object with a given maximum memory capacity.
//Indexer, function, function

		// Commented out default values since they're already defined in the declaration
Cache::Cache(size_type maxmem, float max_load_factor, Evictor* evictor , hash_func hasher) : pImpl_(new Impl()) {
		
		if (hasher == nullptr)
		{
			//Use default hasher in unordered_map
			unordered_map<key_type, storage_unit, hash_func> altmap(4);
			pImpl_->map = altmap;
		}
		else{

			pImpl_->used_hasher =  hasher;
			unordered_map<key_type, storage_unit, hash_func> altmap(4, hasher);
			pImpl_->map = altmap;
		}
		

		//pImpl_->map = altmap;
		pImpl_->map.max_load_factor(max_load_factor);
		pImpl_->usedmemory =0;
		pImpl_->maximummemory = maxmem;
		pImpl_->evictor_ = evictor;
	// Cache constructor
}

Cache::~Cache() = default;//Maybe not default if we need to do some memory management?
	// Cache destructor
	// Don't need to explicitly 'delete pImpl_' because it's a unique_ptr,
	// and it'll delete itself once it's out of scope


// Methods:

void Cache::set(key_type key, val_type val, size_type size) {
	// Description taken from .hh file:
	//
	// Add a <key, value> pair to the cache.
	// If the key is already in the cache, its value gets overwritten by this new one
	// !!! Both the key and the value must be deep-copied, not just pointer-copied
	// If maxmem capacity is exceeded, sufficient (?) values will be removed
	// from the cache to accommodate the new value.
		// "Sufficient" just meaning "satisfying some definition we assign", I think
	pImpl_->set(key, val, size);
}

// Added 'Cache::' to type name since it's defined in the .hh
Cache::val_type Cache::get(key_type key, size_type& val_size) const {
	// Return a pointer to the value associated with the key in the cache; if not found, return null
	// Set the actual size of the returned value (in bytes) in val_size
	return pImpl_->get(key, val_size);
}

bool Cache::del(key_type key) {
	// If an object w/ key 'key' is in the cache, remove it
	return pImpl_->del(key);
}

// Added 'Cache::' to type name since it's defined in the .hh
Cache::size_type Cache::space_used() const {
	// Compute the total amount of memory used by all the current cache values (keys not included)
	return pImpl_->space_used();
}

void Cache::reset(){
	pImpl_->reset();
}
