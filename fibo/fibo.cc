/** @file
 * @brief Fibo number implementation.
 */

#include "fibo.h"

#include <ostream>
#include <vector>
#include <cassert>
#include <string_view>
#include <boost/operators.hpp>


using std::vector;

void Fibo::normalize() {
	for (auto i = length() - 1; i-- > 0;) {
		normalize(i);
	}

	trim();
}

inline void Fibo::normalize(size_t pos) {
	size_t size = length();
	while (pos + 1 < size && value[pos] && value[pos + 1]) {
		value[pos] = false;
		value[pos + 1] = false;
		if (pos + 2 < size) {
			assert(!value[pos + 2]);
			value[pos + 2] = true;
		} else {
			value.push_back(true);
			break;
			// Na pewno nie ma co normalizowaÄ.
		}
		pos += 2;
	}
}

void Fibo::trim() {
	while (value.size() > 1 && value.back() == false) {
		value.pop_back();
	}
}

void Fibo::upsize(const Fibo &rhs) {
	value.resize(std::max(length(), rhs.length()), false);
}

bool Fibo::operator[](size_t pos) const {
	if (pos >= length()) {
		return false;
	}
	return value[pos];
}

Fibo::Fibo() : value({false}) {}

Fibo::Fibo(const std::string_view &str) {
	assert(!str.empty()); // Czy jest niepusta.
	assert(!(str.size() == 1 && str[0] == '0')); // Czy nie ma wiodÄcego zera.
	for (auto it = str.crbegin(); it != str.crend(); it++) {
		assert(*it == '0' || *it == '1');
		value.push_back(*it == '1');
	}
	normalize();
}

bool operator<(const Fibo &lhs, const Fibo &rhs) {
	if (lhs.length() != rhs.length()) {
		return lhs.length() < rhs.length();
	}
	for (size_t i = lhs.length(); i-- > 0;) {
		if (lhs.value[i] != rhs.value[i]) {
			return lhs.value[i] < rhs.value[i];
		}
	}
	return false;
}

bool operator==(const Fibo &lhs, const Fibo &rhs) {
	return lhs.value == rhs.value;
}

Fibo &Fibo::operator+=(const Fibo &rhs) {
	// Rozszerzenie do length + 1 Ĺźeby mĂłc pojedynczo znormalizowaÄ i 2 Ĺźeby mĂłc wpisaÄ wartoĹÄi w ostatnim kroku.
	value.resize(std::max(std::max(length(), rhs.length()) + 1, 2UL), false);
	size_t start = std::max(rhs.length() - 1, 1UL);
	assert(start < value.size());

	unsigned short int acc[3];
	acc[1] = value[start] + rhs[start];
	acc[2] = value[start - 1] + rhs[start - 1];

	for (size_t pos = start; pos >= 2; pos--) {
		acc[0] = acc[1];
		acc[1] = acc[2];
		acc[2] = value[pos - 2] + rhs[pos - 2];

		if (acc[0] > 0 && value[pos + 1]) {
			acc[0]--;
			value[pos + 1] = false;
			assert(value[pos + 2] == false);
			value[pos + 2] = true;
		}
		if (acc[0] >= 2) {
			acc[0] -= 2;
			acc[2]++;
			assert(value[pos + 1] == false);
			value[pos + 1] = true;
		}

		assert(acc[0] <= 1);
		value[pos] = acc[0];
		acc[0] = 0;
	}

	if (acc[1] > 0 && value[2]) {
		acc[1]--;
		value[2] = false;
		assert(value[3] == false);
		value[3] = true;
	}
	if (acc[2] > 0 && acc[1] > 0) {
		acc[1]--;
		acc[2]--;
		assert(value[2] == false);
		value[2] = true;
	}
	if (acc[2] >= 2) {
		acc[2] -= 2;
		acc[1]++;
	}
	if (acc[1] >= 2) {
		acc[1] -= 2;
		acc[2]++;
		assert(value[2] == false);
		value[2] = true;
	}

	assert(acc[2] <= 1);
	value[0] = acc[2];

	assert(acc[1] <= 1);
	value[1] = acc[1];

	normalize();
	return *this;
}

Fibo &Fibo::operator&=(const Fibo &rhs) {
	value.resize(std::min(length(), rhs.length()));
	for (size_t i = 0; i < length(); i++) {
		value[i] = value[i] & rhs.value[i];
	}
	trim();
	// Nie potrzeba normalizacji.
	return *this;
}

Fibo &Fibo::operator^=(const Fibo &rhs) {
	upsize(rhs);
	for (size_t i = 0; i < rhs.length(); i++) {
		value[i] = value[i] ^ rhs.value[i];
	}
	normalize();
	return *this;
}

Fibo &Fibo::operator|=(const Fibo &rhs) {
	upsize(rhs);
	for (size_t i = 0; i < rhs.length(); i++) {
		value[i] = value[i] | rhs.value[i];
	}
	normalize();
	return *this;
}

Fibo &Fibo::operator<<=(const size_t n) {
	value.resize(value.size() + n, false);
	for (size_t i = value.size(); i-- > n;) {
		value[i] = value[i - n];
	}
	for (size_t i = n; i-- > 0;) {
		value[i] = false;
	}
	trim();
	return *this;
}

[[nodiscard]] size_t Fibo::length() const {
	return value.size();
}

std::ostream &operator<<(std::ostream &stream, const Fibo &lhs) {
	for (auto it = lhs.value.rbegin(); it != lhs.value.rend(); it++) {
		stream << *it;
	}
	return stream;
}

const Fibo &Zero() {
	static const Fibo zero;
	return zero;
}

const Fibo &One() {
	static const Fibo one(1);
	return one;
}

