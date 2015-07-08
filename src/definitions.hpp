#ifndef C8_DEFINITIONS_HPP
#define C8_DEFINITIONS_HPP

#include <cstddef>

namespace c8 {
    using byte = unsigned char; // Most registers are 8-bit, and thus of this type.
    using word = unsigned short; // Some special registers like the PC and I register are word sized.
    using addr = word; // Easier to read addresses, same thing as the 16-bit word.
}

#endif
