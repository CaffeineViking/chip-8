#include "catch.hpp"
#include "memory.hpp"

TEST_CASE("Memory validity.", "[memory]") {
    ch8::Memory m;
    REQUIRE(m.valid(0x000) == false); // Inside interpreter memory area.
    REQUIRE(m.valid(0x1FF) == false); // Also inside, right outside boundary.
    REQUIRE(m.valid(0x200) == true); // Just enough to be valid, program space.
    REQUIRE(m.valid(0xFFF) == true); // Still in program memory.
    REQUIRE(m.valid(0x1000) == false); // Everything above 0xFFF is invalid.
}

TEST_CASE("Writing/reading memory.", "[memory]") {
    ch8::Memory m;
    REQUIRE_THROWS(m.write(0x000, 0x42)); // Writing to invalid memory address.
    REQUIRE_THROWS(m.write(0x1FF, 0x42)); // Same, but at the boundary.
    REQUIRE_NOTHROW(m.write(0x200, 0x42)); // Should work, since we are inside program space.
    REQUIRE_NOTHROW(m.write(0xFFF, 0x42)); // Still inside program space, at the boundary.
    REQUIRE_THROWS(m.write(0x1000, 0x42)); // Should fail, outside program space.

    // Same arguments as above.
    REQUIRE_THROWS(m.read(0x000));
    REQUIRE_THROWS(m.read(0x1FF));
    REQUIRE_NOTHROW(m.read(0x200));
    REQUIRE_NOTHROW(m.read(0xFFF));
    REQUIRE_THROWS(m.read(0x1000));

    // Serves as a test for both reading and writing, writing should result in the value 0x42 and
    // reading from the same location should be 0x42. If anything else, something is wrong.
    REQUIRE(m.read(0x200) == 0x42); // Just at edge of interpreter space.
    REQUIRE(m.read(0xFFF) == 0x42); // Just at edge of program space.
}
