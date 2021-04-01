#pragma once

#include <bits/stdc++.h>
#include "common.h"

namespace libfixeypointy {

/** Magic numbers for 128-bit division with specific constants. */
std::unordered_map<uint128_t, MagicNumber128, Unsigned128BitHash> MAGIC_CUSTOM_128BIT_CONSTANT_DIVISION = {
    {5, {0xcccccccccccccccc, 0xcccccccccccccccd, 130, AlgorithmType::OVERFLOW_SMALL}},
    {7, {0x2492492492492492, 0x4924924924924925, 131, AlgorithmType::OVERFLOW_LARGE}}};

/** Magic numbers for 256-bit division with specific constants. */
std::unordered_map<uint128_t, MagicNumber256, Unsigned128BitHash> MAGIC_CUSTOM_256BIT_CONSTANT_DIVISION = {
    {5,
     {0xcccccccccccccccc, 0xcccccccccccccccc, 0xcccccccccccccccc, 0xcccccccccccccccd, 258,
      AlgorithmType::OVERFLOW_SMALL}},
    {7,
     {0x2492492492492492, 0x4924924924924924, 0x9249249249249249, 0x2492492492492493, 259,
      AlgorithmType::OVERFLOW_LARGE}},
    {777,
     {0xa8b098e00a8b098e, 0x00a8b098e00a8b09, 0x8e00a8b098e00a8b, 0x098e00a8b098e00b, 265,
      AlgorithmType::OVERFLOW_SMALL}},
    {999,
     {0x6680a40106680a4, 0x0106680a40106680, 0xa40106680a401066, 0x80a40106680a4011, 266,
      AlgorithmType::OVERFLOW_LARGE}},
};

}  // namespace libfixeypointy