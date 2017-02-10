#ifndef TABLE_IT_H
#define TABLE_IT_H

#include "MyDB_RecordIterator.h"
#include "MyDB_TableReaderWriter.h"
#include "MyDB_PageRecIterator.h"
#include "MyDB_PageReaderWriter.h"

class MyDB_TableRecIterator : public MyDB_RecordIterator {

public:

	MyDB_TableRecIterator(MyDB_RecordPtr rec, MyDB_TablePtr tb, MyDB_BufferManagerPtr bm);

	void getNext() override;
	
	bool hasNext() override;

private: 
	MyDB_TablePtr _tbPtr;
	MyDB_BufferManagerPtr _bmPtr;
	MyDB_RecordPtr _recPtr;
	MyDB_RecordIteratorPtr _pageIterator;
	size_t _currentPage;

};

#endif