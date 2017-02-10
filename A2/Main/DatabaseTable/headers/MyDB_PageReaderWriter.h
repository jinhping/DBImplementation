
#ifndef PAGE_RW_H
#define PAGE_RW_H

#include "MyDB_TableReaderWriter.h"
#include "MyDB_PageRecIterator.h"
#include "MyDB_PageHandle.h"


class MyDB_PageReaderWriter {

public:
	// ANY OTHER METHODS YOU WANT HERE

	// empties out the contents of this page, so that it has no records in it
	// the type of the page is set to MyDB_PageType :: RegularPage
	void clear ();	

	// return an iterator over this page... each time returnVal->next () is
	// called, the resulting record will be placed into the record pointed to
	// by iterateIntoMe
	MyDB_RecordIteratorPtr getIterator (MyDB_RecordPtr iterateIntoMe);

	// appends a record to this page... return false is the append fails because
	// there is not enough space on the page; otherwise, return true
	bool append (MyDB_RecordPtr appendMe);

	// gets the type of this page... this is just a value from an enumeration
	// that is stored within the page
	MyDB_PageType getType ();

	// sets the type of the page
	void setType (MyDB_PageType toMe);
	
	void setPage (MyDB_PageHandle ph);
	
	void setPageSize (size_t ps);
	
private:
	MyDB_PageHandle page_handle;
	size_t page_size;
	//header  |pagetype|num_records|record|...|...|...|...
	constexpr size_t headerSize() const { return sizeof(MyDB_PageType) + sizeof(size_t);}
	size_t getCurrentSize(void *p) { return (size_t)(*((char*)p+sizeof(MyDB_PageType))); }
	void setCurrentSize(void* p, size_t s) { *((size_t*)((char*)p + sizeof(MyDB_PageType))) = s; }
	void* getFirstEmptyRecord(void* p) { return (char*)p + getCurrentSize(p); }
	
	
	// ANYTHING ELSE YOU WANT HERE
};

#endif
