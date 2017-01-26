
#ifndef PAGE_HANDLE_C
#define PAGE_HANDLE_C

#include <memory>
#include <iostream> 
#include "MyDB_PageHandle.h"

void *MyDB_PageHandleBase :: getBytes () {
	return page->getBytes();
}

void MyDB_PageHandleBase :: wroteBytes () {
	page->wroteBytes();
}

void MyDB_PageHandleBase :: unpinPage(){
	page->setPin(false);
}

MyDB_PageHandleBase :: ~MyDB_PageHandleBase () {
	cout<<"destructor"<<endl;
	page->decrementRef();
}

MyDB_PageHandleBase :: MyDB_PageHandleBase(shared_ptr<MyDB_Page> ptr) : page(ptr) {
	
	page->incrementRef();
}

#endif

