
#ifndef BUFFER_MGR_C
#define BUFFER_MGR_C

#include "MyDB_BufferManager.h"


using namespace std; 

MyDB_PageHandle MyDB_BufferManager :: getPage (MyDB_TablePtr tbptr, long i) {
	incrementPageCount();
	auto pagePtr = make_shared<MyDB_Page>(tbptr->getStorageLoc(), i, pageSize, findNextAvailable(), false);
	pagePtr->setLRUNumber(this->LRUNumber);
	Lookup.insert(make_pair<ID, MyDB_Page*>(pagePtr->getKey(), pagePtr.get()));
	LRU.insert(pagePtr.get());
	return make_shared<MyDB_PageHandleBase>(pagePtr);		
}

MyDB_PageHandle MyDB_BufferManager :: getPage () {
	incrementPageCount();
	auto pagePtr = make_shared<MyDB_Page>(tempFile, pageSize, findNextAvailable(), fd, false);
	pagePtr->setLRUNumber(this->LRUNumber);
	Lookup.insert(make_pair<ID, MyDB_Page*>(pagePtr->getKey(), pagePtr.get()));
	LRU.insert(pagePtr.get());
	return make_shared<MyDB_PageHandleBase>(pagePtr);
}

MyDB_PageHandle MyDB_BufferManager :: getPinnedPage (MyDB_TablePtr tbptr, long i) {
	incrementPageCount();
	auto pagePtr = make_shared<MyDB_Page>(tbptr->getStorageLoc(), i, pageSize, findNextAvailable(), true);
	pagePtr->setLRUNumber(this->LRUNumber);
	Lookup.insert(make_pair<ID, MyDB_Page*>(pagePtr->getKey(), pagePtr.get()));
	LRU.insert(pagePtr.get());
	return make_shared<MyDB_PageHandleBase>(pagePtr);	
}

MyDB_PageHandle MyDB_BufferManager :: getPinnedPage () {
	incrementPageCount();
	auto pagePtr = make_shared<MyDB_Page>(tempFile, pageSize, findNextAvailable(), fd, true);
	pagePtr->setLRUNumber(this->LRUNumber);
	Lookup.insert(make_pair<ID, MyDB_Page*>(pagePtr->getKey(), pagePtr.get()));
	LRU.insert(pagePtr.get());
	return make_shared<MyDB_PageHandleBase>(pagePtr);	
}

void MyDB_BufferManager :: unpin (MyDB_PageHandle unpinMe) {
	unpinMe->unpinPage();
}

MyDB_BufferManager :: MyDB_BufferManager (size_t ps, size_t np, string tf) 
	: pageSize(ps), numPages(np), tempFile(tf){
	openTempFile();
	buffer = (char*)malloc(ps*np);
	nextAvailable = buffer;
	LRUNumber = 0;
}

MyDB_BufferManager :: ~MyDB_BufferManager () {
	free(buffer);
	close(fd);
}

void* MyDB_BufferManager :: findNextAvailable(){
	void* temp = nextAvailable;
	if (!isBufferFilled()){
		nextAvailable += pageSize;
	}
	else{
		auto evictPage = *LRU.begin();
		while (evictPage->isPinned()){
			LRU.erase(LRU.begin());
			evictPage->setLRUNumber(++(this->LRUNumber));
			LRU.insert(evictPage);
			evictPage = *LRU.begin();
		}
		temp = (char*)evictPage->getBytes();
		evict(evictPage);
		//TODO: Implement this after implement LRU
	}
	
	return temp;
}


void MyDB_BufferManager :: touch(MyDB_Page* page){
	LRU.erase(page);
	page->setLRUNumber(this->LRUNumber);
	LRU.insert(page);
}

void MyDB_BufferManager :: evict(MyDB_Page* page){
	
	Lookup.erase(make_pair(page->getLocation(), page->getIndex()));
	page->checkAndFree();
	
}

void MyDB_BufferManager :: openTempFile(){
	this->fd = open(tempFile.c_str(), O_CREAT | O_RDWR | O_SYNC, 0666);
}
	
#endif


