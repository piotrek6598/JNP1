/** @file
 * @brief Fibo number interface.
 *
 * Class representing big-nums with Zeckendorf representation and helper functions.
 */
#ifndef FIBO_H
#define FIBO_H

#include <ostream>
#include <string_view>
#include <vector>
#include <cassert>
#include <boost/operators.hpp>

/** @brief Class representing Fibo number.
 */
class Fibo :
		boost::addable<Fibo>,
		boost::bitwise<Fibo>,
		boost::totally_ordered<Fibo>,
		boost::left_shiftable<Fibo, size_t> {
private:
	/** @brief Represents Fibo value by normalized form.
	 */
	std::vector<bool> value;

	/** @brief Normalize Fibo value.
	 */
	void normalize();

	/** @brief Normalize two next positions of Fibo value.
	 * Normalizes [pos] and [pos + 1]. Also normalizes any changes made on positions > pos.
	 * Requirements: representation is normalized from [pos + 1] upwards.
	 * @param[in] pos   - first position to normalize.
	 */
	void normalize(size_t pos);

	/** @brief Removes all leading zeros.
	 */
	void trim();

	/** @brief Resize current Fibo @p value vector by adding 'false'.
	 * If current Fibo @p value vector is shorter than rhs Fibo value @p value
	 * vector then add 'false' values to current Fibo @p value vector until
	 * both @p value vectors have same size. Otherwise nothing happens.
	 * @param[in] rhs   - reference to other Fibo.
	 */
	void upsize(const Fibo &rhs);

	/** @brief Return value of given position in @p value vector.
	 * If the given position is greater than @p value vector size then return
	 * @p false.
	 * @param[in] pos   - position represented by non-negative integer.
	 * @return Value of position pos in @p value vector. If pos is greater than
	 * @p value size then @p false.
	 */
	bool operator[](size_t pos) const;

public:
	/** @brief Create new Fibo with initial value 0.
	 */
	Fibo();

	template<typename T, std::enable_if_t<
			std::is_integral<T>::value
			&& !std::is_same<char, T>::value
			&& !std::is_same<bool, T>::value, int> = 0>
	/** @brief Fibo constructor creates new Fibo with given initial value.
	 * Constructor is enabled only for integral types excluding char and bool.
	 * @param[in] n   - initial Fibo value represented by non-negative integer.
	 */
	Fibo(T n);

	/** @brief Creates new Fibo with given initial value.
	 * @param[in] str   - initial Fibo value represented by description in fibonacci system.
	 */
	explicit Fibo(const std::string_view &str);

	/** @brief Standard copy constructor.
	 * @param[in] rhs   - Fibo to copy.
	 */
	Fibo(const Fibo &rhs) = default;

	/** @brief Standard move constructor.
	 * @param[in] rhs   - Fibo to move.
	 */
	Fibo(Fibo &&rhs) noexcept = default;

	/** @brief Standard copy assignment operator.
	 * @param[in] rhs   - Fibo to copy.
	 * @return Reference to Fibo with assigned value.
	 */
	Fibo &operator=(const Fibo &rhs) = default;

	/** @brief Standard move assignment operator.
	 * @param[in] rhs   - Fibo to move.
	 * @return Reference to Fibo with assigned value.
	 */
	Fibo &operator=(Fibo &&rhs) noexcept = default;

	/** @brief Compares two Fibo numbers.
	 * @param[in] lhs   - reference to first compared Fibo.
	 * @param[in] rhs   - reference to second compared Fibo.
	 * @return @p true if first Fibo is smaller than second,
	 * otherwise @p false.
	 */
	friend bool operator<(const Fibo &lhs, const Fibo &rhs);

	/** @brief Compares two Fibo numbers.
	 * @param[in] lhs   - reference to first compared Fibo.
	 * @param[in] rhs   - reference to second compared Fibo.
	 * @return @p true if given numbers are equal, otherwise @p false.
	 */
	friend bool operator==(const Fibo &lhs, const Fibo &rhs);

	/** @brief Adds value to current Fibo.
	 * @param[in] rhs   - value to add represented by reference to Fibo.
	 * @return Reference to current Fibo with added value.
	 */
	Fibo &operator+=(const Fibo &rhs);

	/** @brief Changes current Fibo by making 'and' operation
	 * on every fibit at normalized form with given Fibo.
	 * @param[in] rhs   - reference to Fibo.
	 * @return Reference to current changed Fibo.
	 */
	Fibo &operator&=(const Fibo &rhs);

	/** @brief Changes current Fibo by making 'or' operation
	 * on every fibit at normalized form with given Fibo.
	 * @param[in] rhs   - reference Fibo.
	 * @return Reference to current changed Fibo.
	 */
	Fibo &operator|=(const Fibo &rhs);

	/** @brief Changes current Fibo by making 'xor' operation
	 * on evry fibit at normalized form with given Fibo.
	 * @param[in] rhs   - reference to Fibo.
	 * @return Reference to current changed Fibo.
	 */
	Fibo &operator^=(const Fibo &rhs);

	/** @brief Changes current Fibo by shifting all fibits left.
	 * @param[in] n   - non-negative integer places to shift left.
	 * @return Reference to current changed Fibo.
	 */
	Fibo &operator<<=(size_t n);

	/** @brief Returns Fibo normalized form length.
	 * @return Fibo normalized form length.
	 */
	[[nodiscard]] size_t length() const;

	/** @brief operator << Prints to given stream, given Fibo normalized form.
	 * @param[in] stream   - reference to stream
	 * @param[in] lhs      - reference to Fibo
	 * @return Reference to stream.
	 */
	friend std::ostream &operator<<(std::ostream &stream, const Fibo &lhs);
};

template<typename T, std::enable_if_t<
		std::is_integral<T>::value
		&& !std::is_same<char, T>::value
		&& !std::is_same<bool, T>::value, int>>
Fibo::Fibo(T n) {
	assert(n >= 0);
	T f1 = 1;
	T f2 = 1;
	size_t pos = 0;
	while (n >= f2 && n - f2 >= f1) {
		pos++;
		T tmp = f2;
		f2 = f2 + f1;
		f1 = tmp;
	}
	value.resize(pos + 1);
	while (n > 0) {
		if (n >= f2) {
			value[pos] = true;
			n -= f2;
		}
		pos--;
		T tmp = f1;
		f1 = f2 - f1;
		f2 = tmp;
	}
	// Nie trzeba normalizowaÄ.
}

/** @brief Creates static Fibo with value 0.
 * @return Reference to Fibo.
 */
const Fibo &Zero();

/** @brief Creates static Fibo with value 1.
 * @return Reference to Fibo.
 */
const Fibo &One();

#endif /* FIBO_H */

