# HW3-Cache-Testing
Hannah Hellerstein and Danny Riso's work on CS 389 HW 3, writing unit tests for our own cache and for others' using the same interface.

Note: For each test case, we have multiple Sections (under the Catch2 testcase structure) that tested for various small edgecases/details of a given function. In the event that code failed a test, where they failed would be listed.
Our Code:
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



