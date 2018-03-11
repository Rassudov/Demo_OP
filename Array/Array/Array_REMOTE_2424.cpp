#include <iostream>
#include <stdexcept>

#pragma once

template <class T> class Array
{
private:
	T* _storage = nullptr;
	int _size = 0;
	int _capacity = 512;

	void doubleCapacity()
	{
		auto newStorage = new T[2 * _capacity];
		for (size_t i = 0; i < _capacity; i++)
		{
			newStorage[i] = _storage[i];
		}
		delete[] _storage;
		_storage = newStorage;
		_capacity *= 2;
	}

	void fillDefault(size_t l, size_t r)
	{
		for (size_t i = l; i <= r; i++)
		{
			_storage[i] = 0;
		}
	}

	//Sort methods begin

	void createPyramid(int cur, int end, int delta)
	{
		int child;
		while ((cur - delta) <= (end - delta) / 2)
		{
			if ((cur - delta) * 2 + 1 <= (end - delta))
			{
				if ((cur - delta) * 2 + 2 <= (end - delta) && _storage[(cur - delta) * 2 + 2 + delta] > _storage[(cur - delta) * 2 + 1 + delta])
				{
					child = (cur - delta) * 2 + 2 + delta;
				}
				else
				{
					child = (cur - delta) * 2 + 1 + delta;
				}
				if (_storage[cur] < _storage[child])
				{
					swap(cur, child);
					cur = child;
				}
				else
				{
					break;
				}
			}
			else
			{
				break;
			}
		}
	}
	
	void phaseOne(int l, int r, int delta)
	{
		for (int i = (l + r) / 2; i >= l; i--)
		{
			createPyramid(i, r, delta);
		}
	}
	
	void phaseTwo(int l, int r, int delta)
	{
		int end = r;
		while (end != l)
		{
			swap(l, end);
			end--;
			createPyramid(l, end, delta);
		}
	}

	//Sort methods end

public:

	int getSize() const
	{
		return _size;
	}

	Array()
	{
		_capacity = 512;
		_size = 0;
		_storage = new T[_capacity];
	}

	Array(int size)
	{
		if (size < 0)
		{
			std::cout << "Bad array size. Size set by abs. value\n";
			size = abs(size);
		}
		if (size > _capacity)
		{
			_capacity = 2 * size;
		}
		_size = size;
		_storage = new T[_capacity];
		fillDefault(0, _size - 1);
	}

	~Array()
	{
		delete[] _storage;
	}

	T& operator[](int index)
	{
		if (index < 0 || index >= _size)
		{
			throw std::out_of_range("Index out of range!");
		}
		return _storage[index];
	}

	void pushBack(T value)
	{
		if (_size + 1 > _capacity)
		{
			doubleCapacity();
		}
		_storage[_size++] = value;
	}

	T popBack()
	{
		if (_size == 0)
		{
			throw std::out_of_range("Array is empty");
		}
		_size--;
		return _storage[_size];
	}

	bool isEmpty()
	{
		return (_size > 0) ? false : true;
	}

	void resize(int newSize)
	{
		if (newSize < 0)
		{
			std::cout << "Negative size! Size token by abs. value\n";
			newSize = abs(newSize);
		}
		if (newSize > _capacity)
		{
			_capacity = 2 * newSize;
		}
		auto newStorage = new T[_capacity];
		for (size_t i = 0; i < newSize && i < _size; i++)
		{
			newStorage[i] = _storage[i];
		}
		delete[] _storage;
		_storage = newStorage;
		if (newSize > _size)
		{
			fillDefault(_size, newSize - 1);
		}
		_size = newSize;
	}

	void clear()
	{
		_capacity = 512;
		resize(0);
	}

	int find(T value)
	{
		int index = -1;
		for (size_t i = 0; i < _size; i++)
		{
			if (_storage[i] = value)
			{
				index = i;
				break;
			}
		}
		return index;
	}

	void swap(int index1, int index2)
	{
		if (index1 < 0 || index2 < 0)
		{
			std::cout << "Negative index! Index token by abs. value\n";
			index1 = abs(index1);
			index2 = abs(index2);
		}
		T temp = _storage[index1];
		_storage[index1] = _storage[index2];
		_storage[index2] = temp;
	}

	void sort(int l, int r)
	{
		if (l < 0 || r < 0 || l > r || l >= _size || r >= _size)
		{
			std::cout << "Negative index! OR Index out of range\n";
			throw std::invalid_argument("Negative index! OR Index out of range\n");
		}
		int delta = l;
		phaseOne(l, r, delta);
		phaseTwo(l, r, delta);
	}

	void reverse(int l, int r)
	{
		if (l < 0 || r < 0 || l > r || l >= _size || r >= _size)
		{
			std::cout << "Negative index! OR Index out of range\n";
			throw std::invalid_argument("Negative index! OR Index out of range\n");
		}
		for (int i = 0; i < (r - l + 1) / 2; i++)
		{
			swap(l + i, r - i);
		}
	}

	void insert(int index, T value) {
		if (index > _size) {
			throw std::out_of_range("Index out of range!");
		}
		if (index < 0) {
			std::cout << "Negative index! Index token by abs. value\n";
			index = abs(index);
		}
		pushBack(value);
		for (int i = _size - 2; i >= index; --i) {
			T temp = _storage[i];
			_storage[i] = _storage[i + 1];
			_storage[i + 1] = temp;
		}
	}

	void erase(int index) {
		if (index > _size) {
			throw std::out_of_range("Index out of range!");
		}
		if (index < 0) {
			std::cout << "Negative index! Index token by abs. value\n";
			index = abs(index);
		}
		for (int i = index; i < _size - 1; i++) {
			T Temp = _storage[i];
			_storage[i] = _storage[i + 1];
			_storage[i + 1] = Temp;
		}
		popBack();
	}
};

template <class T> std::ostream& operator<< (std::ostream& out, Array<T>& arr)
{
	for (size_t i = 0; i < arr.getSize(); i++)
	{
		out << arr[i] << " ";
	}
	out << "\n";
	return out;
}