#ifndef TABLE_IT_C
#define TABLE_IT_C


#include "MyDB_TableRecIterator.h"


MyDB_TableRecIterator :: MyDB_TableRecIterator(MyDB_RecordPtr rec, MyDB_TablePtr tb, MyDB_BufferManagerPtr bm){

	_tbPtr = tb;
	_bmPtr = bm;
	_recPtr = rec;
	_currentPage = 0;
	MyDB_PageReaderWriter _pageReader;
	_pageReader.setPage(_bmPtr->getPage(_tbPtr, _currentPage));
	_pageIterator = _pageReader.getIterator(_recPtr);
}

void MyDB_TableRecIterator :: getNext() {
	_pageIterator->getNext();
}


bool MyDB_TableRecIterator :: hasNext() {

	if (!_pageIterator->hasNext() && _currentPage < _tbPtr->lastPage()){
		++_currentPage;
		MyDB_PageReaderWriter _pr;
		_pr.setPage(_bmPtr->getPage(_tbPtr, _currentPage));
		_pageIterator = _pr.getIterator(_recPtr);
	}	

	return _pageIterator->hasNext();
}

#endif