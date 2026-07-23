

#ifndef DECK
#define DECK

#include <stdexcept>
#include <algorithm>
#include <iostream>

template <size_t max_size>
class Deck {

public:

	Deck();
	~Deck();

	int size;
	
	int data[max_size];

	void add(int card);

	void _add(int card);

	void empty();

	bool has(int card) const;

	bool _has(int card) const;

	int find_index(int card) const;

	void replace(int card_to_delete, int card_to_replace);

	void del(int card);

	void show();
	
	int& operator[](int i) {
		return data[i];
	};
};



template<size_t max_size>
Deck<max_size>::Deck() : size(0), data()
{

	std::fill_n(data, max_size, 0);

}

template<size_t max_size>
Deck<max_size>::~Deck()
{
}

template<size_t max_size>
void Deck<max_size>::add(int card)
{

	if (size >= max_size) throw std::runtime_error("La taille du deck est pleine");

	data[size] = card;
	int i = size;

	while (data[i] < data[i - 1] && i > 0) {
		data[i] = data[i - 1];
		data[i - 1] = card;
		i--;
	}

	size += 1;


}

template<size_t max_size>
void Deck<max_size>::_add(int card)
{

	data[size] = card;
	int i = size;

	while (data[i] < data[i - 1] && i > 0) {
		data[i] = data[i - 1];
		data[i - 1] = card;
		i--;
	}

	size += 1;
}

template<size_t max_size>
bool Deck<max_size>::has(int card) const
{

	if (size == 0) return false;

	int low = 0;
	int up = size;
	int m;

	while (up - low >= 0) {
		m = (up + low) / 2;
		if (data[m] == card) return true;
		else {
			if (data[m] < card) low = m + 1;
			else up = m - 1;
		}
	}

	return false;
}

template<size_t max_size>
bool Deck<max_size>::_has(int card) const
{

	for (int i = 0; i < size; i++) {
		if (card == data[i]) return true;
	}

	return false;
}

template<size_t max_size>
int Deck<max_size>::find_index(int card) const
{
	int low = 0;
	int up = size;
	int m;

	while (up - low >= 0) {
		m = (up + low) / 2;
		if (data[m] == card) return m;
		else {
			if (data[m] < card) low = m + 1;
			else up = m - 1;
		}
	}

	return -1;
}

template<size_t max_size>
void Deck<max_size>::replace(int card_to_delete, int card_to_replace)
{

	int i = find_index(card_to_delete);

	data[i] = card_to_replace;

	while (i > 0 && data[i - 1] > card_to_replace) {
		data[i] = data[i - 1];
		data[i - 1] = card_to_replace;
		i--;
	}

	while (i < size - 1 && data[i + 1] < card_to_replace) {
		data[i] = data[i + 1];
		data[i + 1] = card_to_replace;
		i++;
	}


}


// Perform no check
template<size_t max_size>
void Deck<max_size>::del(int card)
{

	int i = find_index(card);

	while (i < size - 1) {

		data[i] = data[i + 1];
		i++;
	};


	size -= 1;
}

template<size_t max_size>
inline void Deck<max_size>::show()
{

	std::cout << "\n Deck ";


	for (int i = 0; i < size; i++) {
		std::cout << " | " << data[i] << " | ";
	};
}


template<size_t max_size>
void Deck<max_size>::empty()
{
	size = 0;
	return;
}

#endif // !DECK


