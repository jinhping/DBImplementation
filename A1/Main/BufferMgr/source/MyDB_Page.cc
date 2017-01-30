#ifndef DB_PAGE_C
#define DB_PAGE_C

#include "MyDB_Page.h"
#include "MyDB_BufferManager.h"



using namespace std;


MyDB_Page::MyDB_Page(string& loc, long i, size_t ps, void* mem, MyDB_BufferManager* m, bool pin = false)
:location(&loc), index(i), pageSize(ps), dirty(false), pinned(pin), referenceCount(0), LRUNumber(0), data((char*)mem){
	manager = m;
	freed = false;
	readPage();
}

	
void MyDB_Page::readPage(){
	auto file = open(location->c_str(), O_CREAT | O_RDWR | O_SYNC, 0666);
	lseek(file, index*pageSize, SEEK_SET);
	read(file, data, pageSize);
	close(file);
}

void MyDB_Page::writeBack(){

	auto file = open(location->c_str(), O_CREAT | O_RDWR | O_SYNC, 0666);
	lseek(file, index*pageSize, SEEK_SET);
	write(file, data, pageSize);
	close(file);

	dirty = false;
}

void MyDB_Page::checkAndFree(){
	if (dirty){
		writeBack();
	}
	freed = true;
}

void* MyDB_Page::getBytes(){
	if (freed){
		data = (char*)manager->findNextAvailable();
		readPage();
		manager->reEnterLRU(shared_from_this());
		freed = false;
	}
	else{
		manager->externTouch(this->getKey());
	}
	
	return data;
}



#endif