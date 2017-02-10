
#ifndef TABLE_RW_C
#define TABLE_RW_C

#include <fstream>
#include "MyDB_PageReaderWriter.h"
#include "MyDB_TableReaderWriter.h"

using namespace std;

MyDB_TableReaderWriter :: MyDB_TableReaderWriter (MyDB_TablePtr tb, MyDB_BufferManagerPtr bm) {
	_tbPtr = tb;
	_bmPtr = bm;
	_numPages = 0;
	//_record(_tbPtr->getSchema());
}

MyDB_PageReaderWriter &MyDB_TableReaderWriter :: operator [] (size_t t) {
	static MyDB_PageReaderWriter temp;
	auto pageHandle = _bmPtr->getPage(_tbPtr, t);
	temp.setPage(pageHandle);
	temp.setPageSize(_bmPtr->getPageSize());
	temp.setType(MyDB_PageType::RegularPage);
	return temp;	
}

MyDB_RecordPtr MyDB_TableReaderWriter :: getEmptyRecord () {
	return make_shared<MyDB_Record>(_tbPtr->getSchema());
}

MyDB_PageReaderWriter &MyDB_TableReaderWriter :: last () {
	return (*this)[_numPages-1];
}


void MyDB_TableReaderWriter :: append (MyDB_RecordPtr cur) {
	
	if (_tbPtr->lastPage() == -1 || !(*this)[_tbPtr->lastPage()].append(cur)){
		_tbPtr->setLastPage(_numPages++);
		(this->last()).append(cur);
	}
	
}

void MyDB_TableReaderWriter :: loadFromTextFile (string s) {
	auto _rec = this->getEmptyRecord();
	std::ifstream infile(s);
	string line;
	while(std::getline(infile, line)){
		//cout<<line<<endl;
		_rec->fromText(line);
		//cout<<"add to rec"<<_rec << endl;
		this->append(_rec);
		//cout<<"append"<<endl;
	}
	infile.close();
}

MyDB_RecordIteratorPtr MyDB_TableReaderWriter :: getIterator (MyDB_RecordPtr rec) {
	return make_shared<MyDB_TableRecIterator>(rec, _tbPtr, _bmPtr);
}

void MyDB_TableReaderWriter :: writeIntoTextFile (string s) {
	auto _rec = this->getEmptyRecord();
	auto _it = this->getIterator(_rec);
	std::ofstream outfile(s);
	while(_it->hasNext()){
		_it->getNext();
		outfile << _rec << '\n';
	}
	outfile.close();
}

#endif

