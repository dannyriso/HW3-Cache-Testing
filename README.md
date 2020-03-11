# HW3-Cache-Testing
Hannah Hellerstein and Danny Riso's work on CS 389 HW 3, writing unit tests for our own cache and for others' using the same interface.

Note: For each test case, we have multiple Sections (under the Catch2 testcase structure) that tested for various small edgecases/details of a given function. In the event that code failed a test, where they failed would be listed.
Note: In writing our tests, we noticed some bugs in our code (in the transfer over from HW2 to HW3) that we changed here. Our code test below is based on the HW3 code in this repository. However, we gave our HW2 code for everyone else to test, so we expect our code to fail in some respects in other specific tests (such as our evictor failing to output "" at points). 
### Our Code:
| Test Name     | Description   |Pass/Fail|
| ------------- |:-------------:| -----:|
| Testing that set works| Test that the set function for cache can add a key value pair, can overwrite an old value, does not insert new values if maxmem was exceeded (assuming no evictor) | Pass |
| Testing that get works      | Tests that accessing a key not in cache returns nullptr, that correct values are returned from caches of size 1 or larger, checks that valsize changes when get is called      | Pass |
| Testing that spaceused works | Tests that an empty cache has spaceused = 0, else it returns the sum of all the value sizes       | Pass |
| Testing that del works | Tests that deleting from empty cache returns false, that deleting a nonexistant value returns false, that deleting something that has already been deleted does not crash and returns false, that deleting something in the cache returns true and cache reacts appropriately      | Pass |
| Testing that reset works | Tests that spaceused = 0 and getting any previous keys = nullptr      | Pass |
| Testing Generic FIFO Evictor without a Cache | Tests that when there is nothing in evictor, evictor returns(""), that touchkey works, and that eviction works when a key has been touched      | Pass |
| Testing for Touch FIFO | Tests that get touches a key and that set touches a key      | Pass |
| Testing for Evict FIFO | Tests that once a cache is full, the first thing set gets deleted. Tests that evicting something that has already been evicted returns expected result and does not crash. Tests that getting and overwriting do not change eviction order because FIFO      | Pass |

## Testing Other Peoples Code
Note: For ease sake, we took each groups cache_lib.cc, fifo_evictor.cc, and fifo_evictor.hh to test. 
### Prasun + Hrishee
What compilation/linking issues have you encountered (if any)? 
Some issues came up from them not capitalizing their I in pImpl_. They also had their Fifo_Evictor be initialized as FifoEvictor, which caused some problems with our code.
There was also issue where, upon first trying to test this code, the test would not get past set. We learned that this was because we prompted the destruction of their evictor, and temporarily changed their code to comment out that one line such that it wouldn't segfault before going through all the tests. 

| Test Name     |Pass/Fail|
| ------------- | -----:|
| Testing that set works| Pass |
| Testing that get works | Pass |
| Testing that spaceused works | Pass |
| Testing that del works | Pass |
| Testing that reset works | Pass |
| Testing Generic FIFO Evictor without a Cache| Pass |
| Testing for Touch FIFO | Pass |
| Testing for Evict FIFO | Fail, in multiple places "Simple deletion without outside meddling", "Evicting something that has been deleted", and "Testing for proper eviction order". The last one actually caused a segfault because we think it's related to the fact that they did not account for the implicit '\0' character at the end of every value-string, which might cause problems.   |

### Kaiyan + Jiarong	
What compilation/linking issues have you encountered (if any)? 


| Test Name     |Pass/Fail|
| ------------- | -----:|
| Testing that set works| ? |
| Testing that get works | ? |
| Testing that spaceused works | ? |
| Testing that del works | ? |
| Testing that reset works | ? |
| Testing Generic FIFO Evictor without a Cache| ? |
| Testing for Touch FIFO | ? |
| Testing for Evict FIFO | ? |


### Mason+Sebastian
What compilation/linking issues have you encountered (if any)? 
During compilation, there was an inssue in that they used Fifo_evictor for constructor instead of our Fifo_Evictor. Also in their Cache.get, they had an unused variable which registered as an error to us. No other compilation errors, nor did we get any linking errors. 

| Test Name     |Pass/Fail|
| ------------- | -----:|
| Testing that set works| Pass |
| Testing that get works | Fail, in "sizetype is getting changed", as val_size does not get changed in their implementation |
| Testing that spaceused works | Pass |
| Testing that del works | Pass |
| Testing that reset works | Pass |
| Testing Generic FIFO Evictor without a Cache| Fail, in "Does eviction work when there is nothing in evictor?" because they never output a "" in their fifo_evictor.cc |
| Testing for Touch FIFO | Pass |
| Testing for Evict FIFO | Pass |
