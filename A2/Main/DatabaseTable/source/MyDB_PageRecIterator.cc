#include "MyDB_PageRecIterator.h"

MyDB_PageRecIterator :: MyDB_PageRecIterator(MyDB_RecordPtr recptr, MyDB_PageHandle page) : _recptr(recptr), _pagehandle(page){
	void* ptr = _pagehandle->getBytes();
	
	currentPos = (char*)ptr + headerSize();
}


void MyDB_PageRecIterator :: getNext() {
	currentPos = _recptr->fromBinary(currentPos);
}


bool MyDB_PageRecIterator :: hasNext() {
	return (getFirstEmptyRecord(_pagehandle->getBytes()) != currentPos);
}