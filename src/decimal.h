#include <bits/stdc++.h>

using int128_t = __int128;
using uint128_t = unsigned __int128;
using hash_t = uint64_t;


//===----------------------------------------------------------------------===//
//
// Fixed point decimals
//
//===----------------------------------------------------------------------===//

/**
 * A generic fixed point decimal value. This only serves as a storage container for decimals of various sizes.
 * Operations on decimals require a scale.
 *
 * The decimals do not support specifying a precision.
 * The precision is always maximum (38) and the scale ranges from 0 to MAX_SCALE (38).
 */
class Decimal {
 public:
  /** Underlying native data type. */
  using NativeType = int128_t;

  /** The default scale for a Decimal. */
  static constexpr uint32_t DEFAULT_SCALE = 18;
  /** The maximum scale supported by a Decimal. */
  static constexpr uint32_t MAX_SCALE = 38;

  /**
   * Create a decimal value using the given raw underlying encoded value.
   * @param value The value to set this decimal to.
   */
  explicit Decimal(const NativeType &value) : value_(value) {}

  /**
   * Empty constructor.
   */
  Decimal() = default;

  /**
   * Convert an input string into a decimal representation.
   * @param input       The input string to convert.
   *                    If the input string has more digits than the specified scale, the value is rounded up.
   * @param scale       Number of significant digits.
   *                    The scale must be <= MAX_SCALE.
   */
  Decimal(std::string input, uint32_t scale);

  /**
   * Convert an input string into a decimal representation, taking as many digits as possible.
   * @param input           The input string to convert.
   * @param[out] scale      The scale that the decimal was read with.
   */
  Decimal(std::string input, uint32_t *scale);

  /**
   * @return The raw underlying encoded decimal value.
   */
  operator NativeType() const { return value_; }  // NOLINT

  /**
   * Compute the hash value of this decimal instance.
   * @param seed The value to seed the hash with.
   * @return The hash value for this decimal instance.
   */
  hash_t Hash(const hash_t seed) const {
    uint128_t x = value_;
    const uint64_t k_mul = 0x9ddfea08eb382d69ULL;
    uint128_t low_mask = 0xFFFFFFFFFFFFFFFF;
    uint64_t a = ((x & low_mask) ^ (x >> 64)) * k_mul;
    a ^= (a >> 47);
    uint64_t b = ((x >> 64) ^ a) * k_mul;
    b ^= (b >> 47);
    b *= k_mul;
    return b;
  }

  /**
   * @return The hash value of this decimal instance.
   */
  hash_t Hash() const { return Hash(0); }

  /**
   * Add the encoded decimal value @em that to this decimal value.
   *
   * @warning   The other decimal value MUST be of the same scale.
   *            This is currently resolved at runtime in the execution engine VM.
   *
   * @param that The value to add.
   * @return This decimal value.
   */
  const Decimal &operator+=(const NativeType &that) {
    value_ += that;
    return *this;
  }

  /**
   * Subtract the encoded decimal value @em that from this decimal value.
   *
   * @warning   The other decimal value MUST be of the same scale.
   *            This is currently resolved at runtime in the execution engine VM.
   *
   * @param that The value to subtract.
   * @return This decimal value.
   */
  const Decimal &operator-=(const NativeType &that) {
    value_ -= that;
    return *this;
  }


  /**
   * Get the string representation of the current decimal. Requires knowing the scale.
   *
   * @param scale The scale of the current decimal. This must be accurate!
   * @return The string representation of this decimal.
   */
  std::string ToString(uint32_t scale) const;

  /** @return The native representation of the decimal. */
  NativeType ToNative() const { return value_; }

  /**
   * Divide the current decimal by the given decimal.
   * The result is in the numerator's (current decimal's) scale.
   *
   * @param denominator             The decimal to divide by.
   * @param denominator_scale       The scale of the denominator.
   */
  void SignedDivideWithDecimal(Decimal denominator, uint32_t denominator_scale);

  /**
   * Multiply the current decimal by the given decimal.
   * The result is in the higher scale of the current decimal and the multiplier.
   *
   * @param multiplier          The decimal to multiply by.
   * @param lower_scale         The lower scale of the two decimals.
   */
  void SignedMultiplyWithDecimal(Decimal multiplier, uint32_t lower_scale);

  /**
   * Match the scales of the two decimals by rescaling the less precise of the inputs to the higher scale.
   *
   * @param left                The left decimal value.
   * @param right               The right decimal value.
   * @param left_scale          The scale of the left decimal value.
   * @param right_scale         The scale of the right decimal value.
   */
  static void MatchScales(Decimal *left, Decimal *right, uint32_t left_scale, uint32_t right_scale) {
    // TODO(Rohan): Optimize this by performing a binary search.
    int128_t intermediate_value;
    if (left_scale < right_scale) {
      intermediate_value = left->ToNative();
      for (uint32_t i = 0; i < right_scale - left_scale; i++) {
        intermediate_value *= 10;
      }
      *left = Decimal(intermediate_value);
    } else {
      intermediate_value = right->ToNative();
      for (uint32_t i = 0; i < left_scale - right_scale; i++) {
        intermediate_value *= 10;
      }
      *right = Decimal(intermediate_value);
    }
  }

 private:
  // The encoded decimal value
  NativeType value_;

  /** A mask for the bottom half of a 128-bit decimal. */
  static constexpr const uint128_t BOTTOM_MASK = (uint128_t{1} << 64) - 1;
  /** A mask for the top half of a 128-bit decimal. */
  static constexpr const uint128_t TOP_MASK = ~BOTTOM_MASK;

  /** @return The negative version of the current 128-bit decimal. */
  Decimal GetNegation();

  /** @return The absolute value (always positive) version of the current 128-bit decimal. */
  Decimal GetAbs();

  /**
   * Divide the current decimal by the unsigned 128-bit constant supplied.
   *
   * @warning Assumes that the current decimal is non-negative.
   *
   * @param constant The unsigned 128-bit constant to divide by.
   */
  void UnsignedDivideConstant128Bit(uint128_t constant);

  /**
   * Divide the 256-bit unsigned dividend with a 128-bit unsigned divisor.
   *
   * @warning Assumes that the magic number required is already present in MAGIC_MAP256_BIT_CONSTANT_DIVISION.
   *
   * @param unsigned_dividend   The 256-bit unsigned dividend, represented as an array of 128 bit numbers,
   *                            where each 128-bit number only has the lower 64 bits set.
   * @param unsigned_constant   The 128-bit unsigned divisor.
   * @return                    The result of the division.
   */
  static uint128_t UnsignedMagicDivideConstantNumerator256Bit(const uint128_t (&unsigned_dividend)[4],
                                                              uint128_t unsigned_constant);

  /**
   * Divide the current decimal by the divisor provided.
   *
   * @param divisor         The divisor to divide by.
   */
  void SignedDivideWithConstant(int64_t divisor);

  /**
   * Multiply the current decimal with an unsigned decimal.
   * The scale of the result depends on the scale provided.
   *
   * @param unsigned_input  The input decimal to multiply against. Must be unsigned!
   * @param scale           The number of significant digits after the decimal point.
   *                        To obtain higher scale result, pass in the lower scale of the operands.
   *                        To obtain lower scale result, pass in the higher scale of the operands.
   */
  void MultiplyAndSet(const Decimal &unsigned_input, uint32_t scale);

  /** Signed version of MultiplyAndSet with a constant
   * @param input the constant to be multiplied with. */
  void SignedMultiplyWithConstant(int64_t input);

  /**
   * Divide the current positive unsigned 128-bit integer by a power of ten.
   *
   * @warning   The current decimal is assumed to be positive, unsigned, and 128-bit.
   *
   * @param exponent The exponent of the power of ten to divide by. (i in 10^i)
   */
  void UnsignedDivideConstant128BitPowerOfTen(uint32_t exponent);
};