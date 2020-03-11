#define CATCH_CONFIG_MAIN

#include "evictor.hh"
#include "lru_evictor.hh"
#include "fifo_evictor.hh"
#include "cache.hh"
#include <cassert>
#include "catch.hpp"

/*For testing purposes, assume the data payload is always of type C string, and don't forget to account for the last '\0' character 
at the end of the line. It will be easier to debug and print data of this type.
*/

using namespace std;

int return_number(key_type str){
    size_t length = str.length();
    return length;
}

function<size_t(key_type)> myhasher = return_number;  //takes str turns it into its str size

/*
No interdependencies between tests.
No stateful tests--each test is standalone. Your test suite should use a single (global) Cache object, but reset it at the end or beginning of each test.
One piece of functionality tested per test (function). You can extract common set up code to a fixture object.
"Black box" testing only: no testing of internal functionality, optional parameters, or implementation details.
*/


//auto FIFO = Fifo_Evictor();


//auto cache2 = Cache(40, 0.75, FIFO, myhasher);
//Testing for cache.hh



TEST_CASE("Testing that Set works"){
    //Gonna have to assume that get works here
    auto cache = Cache(40, 0.75, nullptr, myhasher);
    Cache::size_type size;
    Cache::val_type val1 = "value_1";
    Cache::val_type val2 = "value_2alright";
    Cache::val_type valr = "reject?";
    Cache::val_type val3 = "Floccinaucinihilipilification";  //It's 29 characters!


    SECTION("Can add a key-value pair"){
        cache.set("key1", val1, 8);
        REQUIRE(*cache.get("key1", size) == *val1);
    }
    SECTION("Overwrites old value if key exists"){
        cache.set("key1", val1, 8);
        cache.set("key1", val2, 16);
        REQUIRE(*cache.get("key1", size) == *val2); 
    }
    SECTION("If maxmem is exceeded, new value is not inserted"){
        //Evictor is tested elsewhere as by implication from cache.hh, once cache is created evictor cannot be reassigned.
        cache.set("key1", val3, 30);
        cache.set("key2", valr, 8);
        REQUIRE(*cache.get("key1", size) == *val3);
        REQUIRE(*cache.get("key2", size) == *valr);
        cache.set("key3", val2, 16); //Should not get inserted
        REQUIRE(cache.get("key3", size) == nullptr);
        
    }
    cache.reset();
    //Instructions said to reset after every test, but... reset is a function?
}

TEST_CASE("Testing that Get works"){
    //We somewhat Need to use set/be interdependent on set for part of Get, since set set is how we add things to cache
    auto cache = Cache(40, 0.75, nullptr, myhasher);
    Cache::size_type size;
    Cache::val_type val1 = "value_1";
    Cache::val_type val2 = "value_2alright";
    Cache::val_type valr = "reject?";

    SECTION("Getting nullptr when trying to access nothing"){ //
        REQUIRE(cache.get("anykey", size) == nullptr);
    }
    SECTION("Getting correct value from cache of size 1"){
        cache.set("key1", val1, 8);
        REQUIRE(*cache.get("key1", size) == *val1);
    }
    SECTION("Getting correct value from cache of multiple objects"){
        cache.set("key1", val1, 8);
        cache.set("key2", val2, 16);
        cache.set("key3", valr, 8);
        REQUIRE(*cache.get("key2", size) == *val2);
        REQUIRE(*cache.get("key1", size) == *val1);
        REQUIRE(*cache.get("key3", size) == *valr); 

    }
    SECTION("size_type size is getting changed"){
        cache.set("key1", val1, 8);
        cache.get("key1", size);
        auto sizebefore = size;
        cache.set("key2", val2, 16);
        cache.get("key2", size);
        REQUIRE(sizebefore != size);

    }
    cache.reset();
}




TEST_CASE("Testing that SpaceUsed works"){
    auto cache = Cache(40, 0.75, nullptr, myhasher);
    Cache::val_type val1 = "value_1";
    Cache::val_type val2 = "value_2alright";
    Cache::val_type valr = "reject?";

    SECTION("Empty cache has spaceused size of 0"){
        REQUIRE(cache.space_used() == 0);
    }
    SECTION("Returns sum of size of one value cache"){
        cache.set("key1", val1, 8);
        REQUIRE(cache.space_used() == 8);
    }
    SECTION("Returns sum of size of all values"){
        cache.set("key1", val1, 8);
        cache.set("key2", val2, 16);
        cache.set("keyr", valr, 8);
        REQUIRE(cache.space_used() == 32);
    }

    cache.reset();
}

TEST_CASE("Testing that Del works"){
    auto cache = Cache(40, 0.75, nullptr, myhasher);
    Cache::size_type size;
    Cache::val_type val1 = "value_1";
    Cache::val_type val2 = "value_2alright";
    Cache::val_type valr = "reject?";

    SECTION("Returns false if attempting to delete something from empty cache"){
        REQUIRE(cache.del("key1") == false);
    }
    SECTION("Attempt to delete something that is not in cache when has other things in it"){
        cache.set("key1", val1, 8);
        cache.set("key2", val2, 16);
        cache.set("keyr", valr, 8);
        REQUIRE(cache.del("key3") == false);
    }
    SECTION("Successfully delete something that is in the cache"){
        cache.set("key1", val1, 8);
        cache.set("key2", val2, 16);
        cache.set("keyr", valr, 8);
        auto boolresult = cache.del("keyr");
        REQUIRE(boolresult == true);
        REQUIRE(cache.get("keyr", size) == nullptr);
        REQUIRE(cache.space_used() != 32);

    }
    SECTION("Attempt to delete something that has already been deleted"){
        //Should return false
        cache.set("key1", val1, 8);
        cache.set("key2", val2, 16);
        cache.set("keyr", valr, 8);
        cache.del("keyr");
        REQUIRE(cache.del("keyr") == false);
    }
    cache.reset();
}
TEST_CASE("Testing that reset works"){
    auto cache = Cache(40, 0.75, nullptr, myhasher);
    Cache::size_type size;
    Cache::val_type val1 = "value_1";
    Cache::val_type val2 = "value_2alright";
    Cache::val_type valr = "reject?";
    cache.set("key1", val1, 8);
    cache.set("key2", val2, 16);
    cache.set("keyr", valr, 8);
    REQUIRE(cache.space_used() != 0);
    cache.reset();
    REQUIRE(cache.space_used() == 0);
    REQUIRE(cache.get("key2", size) == nullptr);
    REQUIRE(cache.get("key1", size) == nullptr);
    REQUIRE(cache.get("keyr", size) == nullptr);    

}

//Testing for evictor.hh functionality

//Note: Below case specifically for FIFO
TEST_CASE("Testing for Touch: FIFO"){
    auto FIFO = Fifo_Evictor();
    auto cache = Cache(40, 0.75, &FIFO, myhasher);
    SECTION("Touch happened in Get"){

    }
    SECTION("Touch happened in Set"){

    }



}

TEST_CASE("Testing for Evict: FIFO"){
    auto FIFO = Fifo_Evictor();
    auto cache = Cache(40, 0.75, &FIFO, myhasher); 

    
}

//Note: Below case specifically for FIFO
TEST_CASE("Testing for Touch: LRU"){
    auto LRU = LRU_Evictor();
    auto cache = Cache(40, 0.75, &LRU, myhasher);
    SECTION("Touch happened in Get"){

    }
    SECTION("Touch happened in Set"){

    }



}

TEST_CASE("Testing for Evict: LRU"){
    auto LRU = LRU_Evictor();
    auto cache = Cache(40, 0.75, &LRU, myhasher); 

    
}


/*int main(){
    auto cache = Cache(40, 0.75, nullptr, myhasher);
    auto defaultedcache = Cache(32); //has no given hasher, uses its own default

    //Testing set
    Cache::size_type size;
    Cache::val_type val1 = "value_1";
    Cache::val_type val2 = "value_2alright";
    Cache::val_type valr = "reject?";

    assert(cache.space_used()==0);
    cache.set("key1", val1, 8);
    cache.set("key2", val2, 16);
    cache.set("key3", valr, 8); //Maybe reject this.

    //test get
    assert(*cache.get("key1", size) == *val1);
    assert(*cache.get("key2", size) == *val2);
    assert(*cache.get("key3", size) == *valr);
    //assert(cache.get("key4", 8) == nullptr);

    //test overwriting
   // cache.set("key3", "value4", 8); //write something new
    cache.set("key2", val1, 8); //swap things
    cache.set("key1", val2, 16);
    assert(*cache.get("key2", size) == *val1);
    assert(*cache.get("key1", size) == *val2);

    //test hasher
    //make defaulted cache be init w/same values as cache
    defaultedcache.set("key1", "value_1", 8);
    defaultedcache.set("key2", "value_2alright", 16);
    defaultedcache.set("key3", "reject?", 8); //Maybe reject this.w
    defaultedcache.set("key2", "value_1", 8); //swap things
    defaultedcache.set("key1", "value_2alright", 16);
    //The two caches cache and defaultedcache should have different buckets for the same "key1"

    //cout << cache.pImpl_->map.bucket("key1") << '\n'; //printing out, should have different numbers
    //cout << defaultedcache.pImpl_->map.bucket("key1") << '\n';
    //We don't have access to pImpl_ as it is private... but, if we set pImpl_ to public, then the two above numbers should be different.

    //If we use this, (presuming we use open addressing), we intentionally hash different values to same bucket, check if size of that bucket is 1.
    

    // Test that the unordered_map class does innately dynamically resize when the load factor
    // reaches beyond its max.
    
    // Like the above, the following will only compile if pImpl_ is made public.
    auto small_cache = Cache(50, 0.75);


    

    //test del  
    size = 0;
    cache.del("key1");
    assert(cache.get("key1", size) == nullptr);
    cache.del("key2");
    assert(cache.get("key2", size) == nullptr);
    cache.del("key3");
    assert(cache.get("key3", size) == nullptr);
    assert(cache.space_used() == 0);

    //test reset
    cache.set("key1", "value_1", 8); //testing readding/overwriting here as well
    cache.set("key2", "value_2alright", 16);
    cache.set("key3", "reject?", 8);
    assert(cache.space_used()!=0);
    cache.reset();
    assert(cache.get("key1", size) == nullptr);
    assert(cache.get("key2", size) == nullptr);
    assert(cache.get("key3", size) == nullptr);
    assert(cache.space_used() == 0);
    //test space used
    
    auto cache2 = Cache(100); //Changing the size for ease of access
    assert(cache2.space_used() ==0); //Making sure that values are added and space_used is increasing as well
    cache2.set("key1", "value_1", 8);
    assert(cache2.space_used() == 8);
    cache2.set("key2", "value_2alright", 16);
    assert(cache2.space_used()  == 24);
    cache2.set("key3", "value_3", 8);
    assert(cache2.space_used() == 32);

    Cache::size_type altsize = 0;
    cache2.get("key1", altsize);//check if it properly edits altsize
    assert(altsize == 8);
    cache2.get("key2", altsize);
    assert(altsize == 16);
    cache2.get("key3", altsize);
    assert(altsize == 8);
    
    assert(cache2.space_used() == 32);
    cache2.set("key1", "komencopaleontologiokonservatoriacestrigontaj", 46); //Overwrite entries, change sizes
    cache2.get("key1", altsize);
    //assert(altsize = 8);
    assert(altsize == 46);
    assert(cache2.space_used() == 70);
    cache2.set("key3", "waaay2lon", 11); //Overwrite entries, change sizes
    cache2.get("key3", altsize);
    assert(altsize == 11);
    assert(cache2.space_used() == 73);
    
    //eviction policy test

    Cache::val_type valev1 = "abcdefghijklmnopqrstuvwxyz";
    Cache::val_type valev2 = "I";
    Cache::val_type valev3 = "thisisatest";
    Cache::val_type valev4 = "object";
        //FIFO test
    auto FIFO = Fifo_Evictor();
    auto cache3 = Cache(30, 0.75, &FIFO);
    cache3.set("key1", valev1, 27);
    cache3.set("key2", valev2, 2); //make sure that chache is not instantly getting rid of things
    assert(*cache3.get("key1", altsize) == *valev1);
    assert(*cache3.get("key2", altsize) == *valev2);
    cache3.set("key3", valev3, 12);
    assert(cache3.get("key1", altsize) == nullptr);
    assert(*cache3.get("key2", altsize) == *valev2);
    assert(*cache3.get("key3", altsize) == *valev3);

        //LRU TEST
    auto LRU  = LRU_Evictor();
    auto cache4 = Cache(40, 0.75, &LRU);
    cache4.set("key1", valev1, 27);
    cache4.set("key2", valev2, 2); //make sure that chache is not instantly getting rid of things
    assert(*cache4.get("key2", altsize) == *valev2);
    assert(*cache4.get("key1", altsize) == *valev1);
    
    cache4.set("key4", valev4, 7); 
    assert(cache4.space_used() == 36);
    cache4.set("key3", valev3, 12);
    assert(cache4.get("key1", altsize) == nullptr); //Both key1 and key2 should have been gotten rid of for key3. 
    assert(cache4.get("key2", altsize) == nullptr);
    assert(*cache4.get("key3", altsize) == *valev3);
    assert(*cache4.get("key4", altsize) == *valev4);
    cache4.set("key5", valev1, 27); 
    assert(cache4.get("key3", altsize) == nullptr); //Since we need to make some space and key3 was touched sooner, should be gotten rid of
    assert(*cache4.get("key5", altsize) == *valev1);
    assert(*cache4.get("key4", altsize) == *valev4);
    

    // Delete all remaining caches
    defaultedcache.reset();
    cache2.reset();
    cache3.reset();
    cache4.reset();
    
    //small_cache.reset();
    return 0;
}*/
