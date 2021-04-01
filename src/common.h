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

/**
 * @brief A hash function for uint128_t values.
 */
struct Unsigned128BitHash {
  /**
   * @param value The value to be hashed.
   * @return The hashed value.
   */
  hash_t operator()(const uint128_t &value) const {
    uint128_t x = value;
    const uint64_t k_mul = 0x9ddfea08eb382d69ULL;
    uint128_t low_mask = 0xFFFFFFFFFFFFFFFF;
    uint64_t a = ((x & low_mask) ^ (x >> 64)) * k_mul;
    a ^= (a >> 47);
    uint64_t b = ((x >> 64) ^ a) * k_mul;
    b ^= (b >> 47);
    b *= k_mul;
    return b;
  }
};

// ------------------------------------------------------------------
// Magic Numbers are used for fast integer division.
// See Hacker's Delight [2E]
// ------------------------------------------------------------------

/**
 * @brief 128-bit magic numbers.
 */
class MagicNumber128 {
 public:
  uint64_t chunk0_;     /// Upper half of 128 bit magic number
  uint64_t chunk1_;     /// Lower half of 128 bit magic number
  uint32_t p_;          /// p as defined in magic division.
  AlgorithmType algo_;  /// The overflow algorithm type.
};

/**
 * @brief 256-bit magic numbers.
 */
class MagicNumber256 {
 public:
  uint64_t chunk0_;     /// Highest 64 bits.
  uint64_t chunk1_;     /// High middle 64 bits.
  uint64_t chunk2_;     /// Low middle 64 bits.
  uint64_t chunk3_;     /// Lowest 64 bits.
  uint32_t p_;          /// p as defined in magic division.
  AlgorithmType algo_;  /// The overflow algorithm type.
};

}  // namespace libfixeypointy
