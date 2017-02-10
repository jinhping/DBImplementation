#ifndef PAGE_IT_H
#define PAGE_IT_H

#include "MyDB_RecordIterator.h"
#include "MyDB_Record.h"
#include "MyDB_PageType.h"
#include "MyDB_PageHandle.h"

class MyDB_PageRecIterator : public MyDB_RecordIterator {

public:

	MyDB_PageRecIterator (MyDB_RecordPtr, MyDB_PageHandle);

	void getNext() override;

	bool hasNext() override;

private: 
	
	MyDB_RecordPtr _recptr;
	
	MyDB_PageHandle _pagehandle;

	void* currentPos;
	
	constexpr size_t headerSize() const { return sizeof(MyDB_PageType) + sizeof(size_t);}
	size_t getCurrentSize(void *p) { return (size_t)(*((char*)p+sizeof(MyDB_PageType))); }
	void setCurrentSize(void* p, size_t s) { *((size_t*)((char*)p + sizeof(MyDB_PageType))) = s; }
	void* getFirstEmptyRecord(void* p) { return (char*)p + getCurrentSize(p); }
	
};


#endif