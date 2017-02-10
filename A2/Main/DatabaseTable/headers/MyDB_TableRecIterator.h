#include "MyDB_RecordIterator.h"


class MyDB_TableRecIterator : public MyDB_RecordIterator {

public:

	void getNext() override;
	
	bool hasNext() override;


};