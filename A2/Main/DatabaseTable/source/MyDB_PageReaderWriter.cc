
#ifndef PAGE_RW_C
#define PAGE_RW_C

#include "MyDB_PageReaderWriter.h"
#include <cstring>

void MyDB_PageReaderWriter :: clear () {
	//memset(page_handle->getBytes(), 0, page_size);
	setType(MyDB_PageType::RegularPage);
	setCurrentSize(page_handle->getBytes(), headerSize());
	page_handle->wroteBytes();
}

MyDB_PageType MyDB_PageReaderWriter :: getType () {
	return *((MyDB_PageType*)(page_handle->getBytes()));
}

MyDB_RecordIteratorPtr MyDB_PageReaderWriter :: getIterator (MyDB_RecordPtr recPtr) {
	
	return make_shared<MyDB_PageRecIterator>(recPtr, page_handle);
}

void MyDB_PageReaderWriter :: setType (MyDB_PageType toMe) {
	*((MyDB_PageType*)(page_handle->getBytes())) = toMe;
	page_handle->wroteBytes();
}

bool MyDB_PageReaderWriter :: append (MyDB_RecordPtr recPtr) {
	void* page = page_handle->getBytes();
	size_t currentSize = getCurrentSize(page);
	size_t addingSize = recPtr->getBinarySize();
	if ( currentSize + addingSize > page_size) return false;
	
	recPtr->toBinary(getFirstEmptyRecord(page));
	setCurrentSize( page, currentSize + addingSize);
	page_handle->wroteBytes();
	
	return true;
}

void MyDB_PageReaderWriter :: setPage (MyDB_PageHandle ph){
	page_handle = ph;
}

void MyDB_PageReaderWriter :: setPageSize (size_t t) {
	page_size = t;
}

#endif
