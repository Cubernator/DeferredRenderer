#ifndef HALF_HPP
#define HALF_HPP

#include <cstdint>

class half
{
public:
	using data_t = uint16_t;

	// cppcheck-suppress noExplicitConstructor
	half(float fval) : m_data(floatToHalf(fval)) { }
	explicit half(data_t data) : m_data(data) { }

	data_t data() const { return m_data; }
	float value() const { return halfToFloat(m_data); }

	operator float() { return value(); }

private:
	data_t m_data;

	static data_t floatToHalf(float fval);
	static float halfToFloat(data_t hval);
};

#endif // HALF_HPP