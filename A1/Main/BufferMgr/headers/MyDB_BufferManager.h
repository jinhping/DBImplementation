
#ifndef BUFFER_MGR_H
#define BUFFER_MGR_H

#include "MyDB_PageHandle.h"
#include "MyDB_Table.h"
#include <unordered_map>
#include <set>
#include <functional>
#include <utility>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>

using namespace std;

using PagePtr = shared_ptr<MyDB_Page>;

struct pageCompare{
	bool operator()(const PagePtr left, const PagePtr right) const {return left->getLRUNumber() < right->getLRUNumber();}
};

struct pair_hash {
    template <class T1, class T2>
    std::size_t operator () (const std::pair<T1,T2> &p) const {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);
		
        return h1 ^ h2;  
    }
};

class MyDB_BufferManager {

public:

	// THESE METHODS MUST APPEAR AND THE PROTOTYPES CANNOT CHANGE!

	// gets the i^th page in the table whichTable... note that if the page
	// is currently being used (that is, the page is current buffered) a handle 
	// to that already-buffered page should be returned
	MyDB_PageHandle getPage (MyDB_TablePtr whichTable, long i);

	// gets a temporary page that will no longer exist (1) after the buffer manager
	// has been destroyed, or (2) there are no more references to it anywhere in the
	// program.  Typically such a temporary page will be used as buffer memory.
	// since it is just a temp page, it is not associated with any particular 
	// table
	MyDB_PageHandle getPage ();

	// gets the i^th page in the table whichTable... the only difference 
	// between this method and getPage (whicTable, i) is that the page will be 
	// pinned in RAM; it cannot be written out to the file
	MyDB_PageHandle getPinnedPage (MyDB_TablePtr whichTable, long i);

	// gets a temporary page, like getPage (), except that this one is pinned
	MyDB_PageHandle getPinnedPage ();

	// un-pins the specified page
	void unpin (MyDB_PageHandle unpinMe);

	// creates an LRU buffer manager... params are as follows:
	// 1) the size of each page is pageSize 
	// 2) the number of pages managed by the buffer manager is numPages;
	// 3) temporary pages are written to the file tempFile
	MyDB_BufferManager (size_t pageSize, size_t numPages, string tempFile);
	
	// when the buffer manager is destroyed, all of the dirty pages need to be
	// written back to disk, any necessary data needs to be written to the catalog,
	// and any temporary files need to be deleted
	~MyDB_BufferManager ();

	// FEEL FREE TO ADD ADDITIONAL PUBLIC METHODS 

private:
	char* buffer;
	size_t pageSize;
	size_t numPages;
	string tempFile;
	char* nextAvailable;
	
	size_t pageCount;
	
	int fd;
	
	void incrementPageCount(){ ++pageCount; ++LRUNumber;}
	bool isBufferFilled() {return (nextAvailable - buffer) >= (pageSize*numPages);}
	void* findNextAvailable();
	
	long LRUNumber;
	using lruset = set<PagePtr, pageCompare>;
	lruset LRU;
	using ID = pair<string, long>;
	unordered_map<ID, lruset::iterator, pair_hash> Lookup;
	
	
	void touch(lruset::iterator);
	void* evict();
	void openTempFile();


};

#endif


