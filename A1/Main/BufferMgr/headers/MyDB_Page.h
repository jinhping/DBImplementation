#ifndef PAGE_H
#define PAGE_H

#include <memory>
#include <utility>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
using namespace std;


class MyDB_BufferManager;

class MyDB_Page: public enable_shared_from_this<MyDB_Page>{
	
public:
	//Page object secondary constructor
	MyDB_Page(string& loc, long i, size_t ps, void* mem, MyDB_BufferManager* manager, bool pin);
	
	//Page object destructor
	~MyDB_Page();
	
	//Increment the reference  count of the page
	void incrementRef(){
		++referenceCount;
	}
	//Decrement the reference count of the page and check if the page has no reference count
	//Unpin the page
	void decrementRef(){
		if (--referenceCount == 0) {
			setPin(false);
		}
	}
	//return whether the page is pinned or unpinned 
	bool isPinned(){
		return pinned;
	}
	//change pinned status of a page
	void setPin(bool pin){
		pinned = pin;
	}
	
	//get bytes of a page
	void* getBytes();
	//mark the page has been written
	void wroteBytes(){
		markDirty();
	}
	//read a page
	void readPage();
	//write back to page data
	void writeBack();
	//check whether the page is dirty or not, if it is dirty, write back and change the freed bit to true
	void checkAndFree();

	shared_ptr<MyDB_Page> getPtr() { return shared_from_this();}
	//get LRU number of the page
	unsigned int getLRUNumber() const {return LRUNumber;}
	//set the page LRU number
	void setLRUNumber(unsigned int n){LRUNumber = n;}
	//get table info
	string getLocation() {return *location;}
	//get table index
	long getIndex() {return index;}

	pair<string, long> getKey(){ auto ind = index == -1 ? LRUNumber : index; string temp = *location; return make_pair<string, long>(std::move(temp), std::move(ind));};
	//set anonymous bit to true
	void setAnonymous() {isAnonymous = true;}
private:
	//used to store table information
	string* location;
	//which index in that table
	long index;
	//page size
	size_t pageSize;
	//whether the page is dirty or not
	bool dirty;
	//whether the page is pinned or not
	bool pinned;
	//how many page handlers point to this page now
	unsigned int referenceCount;
	//LRU Number of this page
	unsigned int LRUNumber;
	//Page data
	char* data;
	//whether this page is anonymous or not
	bool isAnonymous;
	//private function to set the dirty bit to true
	void markDirty(){
		dirty = true;
	}
	//mark if the page is freed
	bool freed;
	//buffer manager
	MyDB_BufferManager* manager;
	
};


#endif 