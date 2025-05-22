#pragma once

#include <cstdint>
#include <bitset>

namespace SECS
{

#define MAX_COMP 32
#define MAX_ENT 256

using Entity = uint32_t;
using EntSignature = std::bitset<MAX_COMP>;

using ComponentID = uint8_t;
}

