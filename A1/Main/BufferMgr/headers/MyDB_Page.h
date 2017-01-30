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
	
	MyDB_Page(string& loc, long i, size_t ps, void* mem, MyDB_BufferManager* manager, bool pin);
	
	~MyDB_Page(){ dirty = false;};
	
	void incrementRef(){
		++referenceCount;
	}
	
	void decrementRef(){
		if (--referenceCount == 0) {
			setPin(false);
		}
	}
	
	bool isPinned(){
		return pinned;
	}
	
	void setPin(bool pin){
		pinned = pin;
	}
	
	
	void* getBytes();
	
	void wroteBytes(){
		markDirty();
	}
	
	void readPage();
	void writeBack();
	void checkAndFree();
	shared_ptr<MyDB_Page> getPtr() { return shared_from_this();}
	
	unsigned int getLRUNumber() const {return LRUNumber;}
	void setLRUNumber(unsigned int n){LRUNumber = n;}
	string getLocation() {return *location;}
	long getIndex() {return index;}
	pair<string, long> getKey(){ auto ind = index == -1 ? LRUNumber : index; string temp = *location; return make_pair<string, long>(std::move(temp), std::move(ind));};
	
private:
	string* location;
	long index;
	size_t pageSize;
	bool dirty;
	bool pinned;
	unsigned int referenceCount;
	unsigned int LRUNumber;
	char* data;
	
	void markDirty(){
		dirty = true;
	}
	
	bool freed;
	MyDB_BufferManager* manager;
	
};


#endif 