
#ifndef STACK_H
#define STACK_H

// this is the node class used to build up the LIFO stack
template <class Data>
class Node {

private:

	Data holdMe;
	Node *next;
	
public:
	Node(){next = nullptr;}
	Data getData(){ return holdMe;}
	Node* getNext(){ return next;}
	void setData(Data d){holdMe = d;};
	void setNext(Node *n){next = n;};

};

// a simple LIFO stack
template <class Data> 
class Stack {

	Node <Data> *head;

public:

	// destroys the stack
	~Stack () { 
		while(!isEmpty()){
			pop();
		}
		delete head;
	}

	// creates an empty stack
	Stack () {
		head = new Node<Data>();
	}

	// adds pushMe to the top of the stack
	void push (Data d) {
		auto newNode = new Node<Data>();
		newNode->setData(d);
		newNode->setNext(head->getNext());
		head->setNext(newNode);
	}

	// return true if there are not any items in the stack
	bool isEmpty () { return !head->getNext(); }

	// pops the item on the top of the stack off, returning it...
	// if the stack is empty, the behavior is undefined
	Data pop () {
		if (auto top = head->getNext()){
			auto data = top->getData();
			head->setNext(top->getNext());
			delete top;
			return data;
		}
		else{
			throw std::out_of_range("stack is empty");
			return Data();
		}
	}
};

#endif
