#pragma once

/// @brief return greatest common divisor of two types
template <typename T>
static constexpr T gcd(T a, T b)
{
    // verify that T is a divisible type
    static_assert(std::is_integral_v<T> || std::is_floating_point_v<T>, "T must be an integral or floating point type");
    if (b == 0)
    {
        return a; // If b is zero, return a
    }
    
    while (b != 0)
    {
        T temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}
