#ifndef PAGE_H
#define PAGE_H

#include <memory>
#include <utility>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

using namespace std;



class MyDB_Page{
	
public:
	
	MyDB_Page(string& loc, size_t ps, void* mem, int fd, bool pin); //used for anonymous page construction
	
	MyDB_Page(string& loc, long i, size_t ps, void* mem, bool pin);
	
	~MyDB_Page(){};
	
	void incrementRef(){
		++referenceCount;
	}
	
	void decrementRef(){
		--referenceCount;
		if (referenceCount == 0) pinned = false;
	}
	
	bool isPinned(){
		return pinned;
	}
	
	void setPin(bool pin){
		pinned = pin;
	}
	
	
	void* getBytes(){
		return data;
	}
	
	void wroteBytes(){
		markDirty();
	}
	
	void readPage();
	void writeBack();
	void checkAndFree();
	
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
	int fd = 0;
	
	void markDirty(){
		dirty = true;
	}
	
};


#endif 