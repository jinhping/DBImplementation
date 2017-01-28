#ifndef DB_PAGE_C
#define DB_PAGE_C

#include "MyDB_Page.h"



using namespace std;

MyDB_Page::MyDB_Page(string& loc, size_t ps, void* mem, int fd, bool pin = false)
	:location(&loc), index(-1), pageSize(ps), dirty(false), pinned(pin), referenceCount(0), LRUNumber(0), data((char*)mem), fd(fd){
		
}

MyDB_Page::MyDB_Page(string& loc, long i, size_t ps, void* mem, bool pin = false)
:location(&loc), index(i), pageSize(ps), dirty(false), pinned(pin), referenceCount(0), LRUNumber(0), data((char*)mem), fd(0){
	readPage();
}

	
void MyDB_Page::readPage(){
	auto file = open(location->c_str(), O_CREAT | O_RDWR | O_SYNC, 0666);
	lseek(file, index*pageSize, SEEK_SET);
	read(file, data, pageSize);
	close(file);
}

void MyDB_Page::writeBack(){
	if (fd){
		write(fd, data, pageSize);
	}
	else{
		auto file = open(location->c_str(), O_CREAT | O_RDWR | O_SYNC, 0666);
		lseek(file, index*pageSize, SEEK_SET);
		write(file, data, pageSize);
		close(file);
	}
	dirty = false;
}

void MyDB_Page::checkAndFree(){
	if (dirty){
		//cout<<"-------------------" << referenceCount << " " << (char*)data << endl;
		writeBack();
	}
}



#endif