
#ifndef BUFFER_MGR_C
#define BUFFER_MGR_C

#include "MyDB_BufferManager.h"


using namespace std; 

MyDB_PageHandle MyDB_BufferManager :: getPage (MyDB_TablePtr tbptr, long i) {
	auto storage = tbptr->getStorageLoc();
	auto index = i;
	auto key = make_pair<string, long>(std::move(storage), std::move(index));
	
	if (Lookup.find(key)!=Lookup.end()){
		touch(Lookup[key]);
		return make_shared<MyDB_PageHandleBase>(*Lookup[key]);
	}
	else{
		return makePage(tbptr->getStorageLoc(), i, pageSize, findNextAvailable(), false);
	}
}

MyDB_PageHandle MyDB_BufferManager :: getPage () {
	return makePage(tempFile, ++numAnonymous, pageSize, findNextAvailable(), false);
}

MyDB_PageHandle MyDB_BufferManager :: getPinnedPage (MyDB_TablePtr tbptr, long i) {
	auto storage = tbptr->getStorageLoc();
	auto index = i;
	auto key = make_pair<string, long>(std::move(storage), std::move(index));
	
	if (Lookup.find(key)!=Lookup.end()){
		touch(Lookup[key]);
		return make_shared<MyDB_PageHandleBase>(*Lookup[key]);
	}
	else{
		return makePage(tbptr->getStorageLoc(), i, pageSize, findNextAvailable(), true);
	}
}

MyDB_PageHandle MyDB_BufferManager :: getPinnedPage () {
	return makePage(tempFile, ++numAnonymous, pageSize, findNextAvailable(), true);
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
	numAnonymous = 0;
}

MyDB_BufferManager :: ~MyDB_BufferManager () {
	
	for (auto it = LRU.begin(); it != LRU.end(); it++){
		(*it)->writeBack();
	}
	free(buffer);
	close(fd);
}

void* MyDB_BufferManager :: findNextAvailable(){
	void* temp = nextAvailable;
	if (!isBufferFilled()){
		nextAvailable += pageSize;
	}
	else{
		temp = (char*)evict();
	}
	return temp;
}


void MyDB_BufferManager :: touch(lruset::iterator page){
	auto temp = *page;
	temp->setLRUNumber(++LRUNumber);
	LRU.erase(page);
	LRU.insert(temp);
}

void* MyDB_BufferManager :: evict(){
	auto evictPage = LRU.begin();
	while((*evictPage)->isPinned()){
		evictPage++;
	}
	void* bufferLoc = (*evictPage)->getBytes();
	(*evictPage)->checkAndFree();
	Lookup.erase((*evictPage)->getKey());
	LRU.erase(evictPage);
	return bufferLoc;
}

void MyDB_BufferManager :: openTempFile(){
	this->fd = open(tempFile.c_str(), O_CREAT | O_RDWR | O_SYNC, 0666);
}

MyDB_PageHandle MyDB_BufferManager :: makePage(string& location, long index, size_t pageSize, void* mem, bool pin){
	incrementPageCount();
	auto pagePtr = make_shared<MyDB_Page>(location, index, pageSize, mem, this, pin);
	pagePtr->setLRUNumber(++LRUNumber);
	auto info = LRU.insert(pagePtr->getPtr());
	Lookup.insert(make_pair(pagePtr->getKey(), info.first));
	return make_shared<MyDB_PageHandleBase>(pagePtr);	
}

void MyDB_BufferManager :: reEnterLRU(shared_ptr<MyDB_Page> ptr){
	incrementPageCount();
	ptr->setLRUNumber(++LRUNumber);
	auto info = LRU.insert(ptr);
	Lookup.insert(make_pair(ptr->getKey(), info.first));
}
	
#endif


