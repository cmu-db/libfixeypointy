#pragma once

namespace libfixeypointy {

using int128_t = __int128;
using uint128_t = unsigned __int128;
using hash_t = uint64_t;

/**
 * @enum libfixeypointy::AlgorithmType
 * @brief Which overflow algorithm to use when using magic numbers
 */
enum class AlgorithmType : uint8_t { OVERFLOW_SMALL = 0, OVERFLOW_LARGE = 1 };

}  // namespace libfixeypointy
