
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
#include <queue>


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

using ID = pair<string, long>;
using lruset = set<PagePtr, pageCompare>;

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

	// return the next available buffer slot either by increasing the pointer or evicting the existing page 
	void* findNextAvailable();
	
	// page can use this method to put itself in the LRU again
	// in case that a page object still has handles exists but page content was evicted
	void reEnterLRU(shared_ptr<MyDB_Page> page);

	// page can use this method to touch itself in the LRU
	void externTouch(ID key){
		touch(Lookup[key]);
	}
	
	// page can return the disk slot to buffer manager
	void returnDiskSlot(long);
private:

	//essential elements for buffer manager
	//such as pointer to buffer, page size and etc.
	char* buffer;
	size_t pageSize;
	size_t numPages;
	string tempFile;
	
	//pointer to the beginning of the next available slot in the buffer
	char* nextAvailable;
	
	//how many pages we have allocated so far
	size_t pageCount;
	
	//file descriptor for temp file
	int fd;

	//max number of anonymous pages in the memory so far
	long numAnonymous;
	
	//LRU number
	long LRUNumber;

	//LRU and Lookup table
	lruset LRU;
	unordered_map<ID, lruset::iterator, pair_hash> Lookup;
	
	//pool to recycle anonymous page disk slots
	queue<long> anonymousPool;
	
	//********helper functions section***************
	//when assigning new page, increase the page count also increase LRUNumber
	void incrementPageCount(){ ++pageCount; ++LRUNumber;}
	
	//check if buffer is filled
	bool isBufferFilled() {return (nextAvailable - buffer) >= (pageSize*numPages);}
	
	//touch a page by its iterator in the set
	void touch(lruset::iterator);
	
	//evict a page and return its buffer slot
	void* evict();
	
	//open the temp file
	void openTempFile();
	
	//make a page object and return its page handle
	MyDB_PageHandle makePage(string&, long, size_t, void*, bool, bool anonymous = false);
	
	//get next disk slot that can be used for anonymous page
	long getNextAnonymousSlot();

};

#endif


