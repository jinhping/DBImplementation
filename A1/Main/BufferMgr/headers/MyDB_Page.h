#ifndef PAGE_H
#define PAGE_H

using namespace std;



class MyDB_Page {
	
public:
	
	MyDB_Page(size_t ps);
	
	
	~MyDB_Page();
	
	
	
	
	
	
private:
	size_t pageSize;
	bool dirty;
	bool pinned;
	unsigned int referenceCount;
	unsigned int LRUNumber;
	shared_ptr<char> data;
	
	
	
}


#endif 