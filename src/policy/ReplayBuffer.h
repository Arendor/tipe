

#ifndef REPLAYBUFFER

#define REPLAYBUFFER

#include "../utils/utils.h"
#include <random>
#include <memory>
#include <numeric>

extern std::mt19937 gen;

template<class MemoryStorage, size_t max_size>
class ReplayBuffer {

public:

	ReplayBuffer();
	~ReplayBuffer();

	std::vector<MemoryStorage> buffer;
	int indice_actuel;
	// Nombre d'éléments réellement présents
	int size;

	MemoryStorage& create_transition();
	MemoryStorage& add_transition(MemoryStorage transition);
	MemoryStorage& get_transition(int indice);
	MemoryStorage& get_current_transition();


	std::vector<int> sample_indexes(int sample_size);

	void clear();

	MemoryStorage& operator[](int i) {
		return buffer[i];
	};

	/*
	struct CustomIterator
	{
	public:
		using iterator_category = std::forward_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = MemoryStorage;
		using pointer = int*;  
		using reference = int&; 

		CustomIterator(pointer ptr, ReplayBuffer<MemoryStorage,max_size>* bufptr) : _ptr(ptr), _bufptr(bufptr) {}

		reference operator*() const { return _bufptr->buffer.at(*_ptr); }
		pointer operator->() { return _ptr; }

		CustomIterator& operator++() { _ptr++; return *this; }
		CustomIterator operator++(int) { CustomIterator tmp = *this; ++(*this); return tmp; }

		friend bool operator== (const CustomIterator& a, const CustomIterator& b) { return a._ptr == b._ptr && a._bufptr == b._bufptr; };
		friend bool operator!= (const CustomIterator& a, const CustomIterator& b) { return a._ptr != b._ptr || a._bufptr != b._bufptr; };

	private:

		pointer _ptr;
		ReplayBuffer<MemoryStorage, max_size>* _bufptr;

	};


	CustomIterator begin(int* ptr) { return CustomIterator(ptr, this); }
	CustomIterator end(int* ptr) { return CustomIterator(ptr, this); }*/


};

template<class MemoryStorage, size_t max_size>
inline ReplayBuffer<MemoryStorage, max_size>::ReplayBuffer() : buffer(0), indice_actuel(-1), size(0)
{
	buffer.resize(max_size);
}

template<class MemoryStorage, size_t max_size>
inline ReplayBuffer<MemoryStorage, max_size>::~ReplayBuffer()
{
}

template<class MemoryStorage, size_t max_size>
inline MemoryStorage& ReplayBuffer<MemoryStorage, max_size>::create_transition()
{
	indice_actuel = (indice_actuel + 1) % max_size;
	if (size != max_size) size += 1;
	buffer[indice_actuel] = MemoryStorage();
	
	return buffer[indice_actuel];
}

template<class MemoryStorage, size_t max_size>
inline  MemoryStorage& ReplayBuffer<MemoryStorage, max_size>::add_transition(MemoryStorage transition)
{
	indice_actuel = (indice_actuel + 1) % max_size;
	if (size != max_size) size += 1;
	buffer[indice_actuel] = transition;

	return buffer[indice_actuel];
}

template<class MemoryStorage, size_t max_size>
inline MemoryStorage& ReplayBuffer<MemoryStorage, max_size>::get_transition(int indice)
{
	return buffer[indice];
}

template<class MemoryStorage, size_t max_size>
inline MemoryStorage& ReplayBuffer<MemoryStorage, max_size>::get_current_transition()
{
	return buffer[indice_actuel];
}

template<class MemoryStorage, size_t max_size>
inline void ReplayBuffer<MemoryStorage, max_size>::clear()
{
	indice_actuel = -1;
	size = 0;
}


template<class MemoryStorage, size_t max_size>
inline std::vector<int> ReplayBuffer<MemoryStorage, max_size>::sample_indexes(int sample_size)
{
	if (size == 0) return std::vector<int>();

	if (size < sample_size) sample_size = size;

	std::vector<int> numberAr(size);
	std::iota(numberAr.begin(), numberAr.end(), 0);
	std::shuffle(numberAr.begin(), numberAr.end(), gen);

	std::vector<int> newVec(sample_size);
	std::copy(numberAr.begin(), numberAr.begin() + sample_size, newVec.begin());

	return newVec;
};

/*
class ReplayBufferHelper {

public:

	static std::vector<int> randomArray(int actual_size, int sample_size) {

		if (actual_size == 0) return std::vector<int>();

		if (actual_size < sample_size) sample_size = actual_size;

		std::vector<int> numberAr(actual_size);
		std::iota(numberAr.begin(), numberAr.end(), 0);
		std::shuffle(numberAr.begin(), numberAr.end(), gen);

		std::vector<int> newVec(sample_size);
		std::copy(numberAr.begin(), numberAr.begin() + sample_size, newVec.begin());

		return newVec;

	}

};*/

#endif // !REPLAYBUFFER