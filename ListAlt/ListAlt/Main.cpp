#include <iostream>

using namespace std;

class List
{
private:
	class Node
	{
	private:
		int value = 0;
		Node* next = nullptr;

	public:

		Node()
		{
			value = 0;
			next = 0;
		}

		Node* getNext()
		{
			return next;
		}

		void setNext(Node* nxt)
		{
			next = nxt;
		}

		int getValue()
		{
			return value;
		}

		void setValue(int newVal)
		{
			value = newVal;
		}
	};

	Node* head = nullptr;
	Node* tail = nullptr;
	int size = 0;
	Node* current = head;

	Node* getHead()
	{
		return head;
	}

	Node* getTail()
	{
		return tail;
	}


public:

	List()
	{
		head = nullptr;
		tail = nullptr;
		size = 0;
		current = head;
	}

	int getSize()
	{
		return size;
	}

	void moveTail()
	{
		current = this->getTail();
	}

	void moveHead()
	{
		current = this->getHead();
	}

	bool moveNext()
	{
		if (current->getNext() != nullptr)
		{
			current = current->getNext();
			return true;
		}
		return false;
	}

	int getCurrent()
	{
		if(current != nullptr)
			return current->getValue();
	}

	void setCurrent(int newVal)
	{
		if(current != nullptr)
			current->setValue(newVal);
	}

	void addElementBeforeHead(int val)
	{
		if (size == 0)
		{
			head = tail = new Node();
			head->setValue(val);
			current = head;
		}
		else
		{
			Node* temp = new Node();
			temp->setValue(val);
			temp->setNext(head);
			head = temp;
		}
		size++;
	}

	void addElementAfterTail(int val)
	{
		if (size == 0)
		{
			head = tail = new Node();
			head->setValue(val);
			current = head;
		}
		else
		{
			Node* temp = new Node();
			temp->setValue(val);
			tail->setNext(temp);
			tail = temp;
		}
		size++;
	}

	void addElementAfterCurrent(int val)
	{
		if (size == 0)
		{
			head = tail = new Node();
			head->setValue(val);
			current = head;
		}
		else
		{
			Node* temp = new Node();
			temp->setValue(val);
			temp->setNext(current->getNext());
			current->setNext(temp);
		}
		size++;
	}

	void deleteAfterCurrentElement()
	{
		Node* temp = current->getNext();
		current->setNext(current->getNext()->getNext());
		delete temp;
		size--;
	}

	void deleteHeadElement()
	{
		Node* temp = head->getNext();
		delete head;
		head = temp;
		size--;
	}

	void deleteTailElement()
	{
		Node* temp = head;
		for (int i = 0; i < size - 1; i++)
		{
			temp = temp->getNext();
		}
		delete tail;
		tail = temp;
		size--;
	}
};

int main()
{
	return 0;
}