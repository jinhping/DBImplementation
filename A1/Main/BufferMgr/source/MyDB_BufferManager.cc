
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
		incrementPageCount();
		auto pagePtr = make_shared<MyDB_Page>(tbptr->getStorageLoc(), i, pageSize, findNextAvailable(), false);
		pagePtr->setLRUNumber(++LRUNumber);
		auto info = LRU.insert(pagePtr);
		Lookup.insert(make_pair(pagePtr->getKey(), info.first));
		return make_shared<MyDB_PageHandleBase>(pagePtr);
	}
}

MyDB_PageHandle MyDB_BufferManager :: getPage () {
	incrementPageCount();
	auto pagePtr = make_shared<MyDB_Page>(tempFile, pageSize, findNextAvailable(), fd, false);
	pagePtr->setLRUNumber(++LRUNumber);
	auto info = LRU.insert(pagePtr);
	Lookup.insert(make_pair(pagePtr->getKey(), info.first));
	return make_shared<MyDB_PageHandleBase>(pagePtr);
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
		incrementPageCount();
		auto pagePtr = make_shared<MyDB_Page>(tbptr->getStorageLoc(), i, pageSize, findNextAvailable(), true);
		pagePtr->setLRUNumber(++LRUNumber);
		auto info = LRU.insert(pagePtr);
		Lookup.insert(make_pair(pagePtr->getKey(), info.first));
		return make_shared<MyDB_PageHandleBase>(pagePtr);	
	}
}

MyDB_PageHandle MyDB_BufferManager :: getPinnedPage () {
	incrementPageCount();
	auto pagePtr = make_shared<MyDB_Page>(tempFile, pageSize, findNextAvailable(), fd, true);
	pagePtr->setLRUNumber(++LRUNumber);
	auto info = LRU.insert(pagePtr);
	Lookup.insert(make_pair(pagePtr->getKey(), info.first));
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
		//cout<<"evicted: " << (char*)temp<<endl ;
	}
	return temp;
}


void MyDB_BufferManager :: touch(lruset::iterator page){
	auto temp = *page;
	LRU.erase(page);
	temp->setLRUNumber(++LRUNumber);
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
	
#endif


