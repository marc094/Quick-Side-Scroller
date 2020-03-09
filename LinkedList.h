#ifndef __LINKED_LIST_H__
#define __LINKED_LIST_H__

#include "Defs.h"

template<class TYPE>
struct ListNode
{
	TYPE value = null;
	ListNode * prev = null;
	ListNode * next = null;

	ListNode(TYPE obj)
	{
		value = obj; 
	}

	~ListNode()
	{
		delete value;
	}
};

template<class TYPE>
struct Iterator
{
	ListNode<TYPE> * node = null;

	const Iterator & operator++()
	{
		node = node->next;
		return this;
	}

	const Iterator & operator--()
	{
		node = node->prev;
		return this;
	}

	const TYPE & operator*()
	{
		return node->value;
	}
};

template<class TYPE>
class LinkedList
{
private:
	ListNode * first = null;
	ListNode * last = null;

	uint64 size = 0;

	Iterator<TYPE> iterator;

	ListNode<TYPE> * Find(TYPE obj)
	{
		for (ListNode<TYPE> * n = first; n != null; n = n->next)
		{
			if (n->value == obj)
			{
				return n;
			}
		}

		return null;
	}


public:
	LinkedList(){}
	~LinkedList(){}

	void Add(TYPE obj)
	{
		ListNode* n = new ListNode<TYPE>(obj);
		n->prev = last;
		last = n;
		size++;
	}

	void Remove(TYPE obj)
	{
		ListNode<TYPE> * n = Find(obj);

		n->prev->next = n->next;
		n->next->prev = n->prev;
		size--;

		delete n;
	}

	const Iterator<TYPE> & Begin()
	{
		return (Iterator<TYPE>() { first; });
	}
};

#endif //__LINKED_LIST_H__