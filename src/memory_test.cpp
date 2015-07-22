#include "catch.hpp"
#include "memory.hpp"
#include "definitions.hpp"

TEST_CASE("Memory validity.", "[memory, validity]") {
    ch8::Memory m {nullptr, 0};
    REQUIRE(m.valid(0x000) == false); // Inside interpreter memory area.
    REQUIRE(m.valid(0x1FF) == false); // Also inside, right outside boundary.
    REQUIRE(m.valid(0x200) == true); // Just enough to be valid, program space.
    REQUIRE(m.valid(0xFFF) == true); // Still in program memory.
    REQUIRE(m.valid(0x1000) == false); // Everything above 0xFFF is invalid.
}

TEST_CASE("Writing/reading memory.", "[memory, reading, writing]") {
    ch8::Memory m {nullptr, 0};
    REQUIRE_THROWS(m.write(0x000, 0x42)); // Writing to invalid memory address.
    REQUIRE_THROWS(m.write(0x1FF, 0x42)); // Same, but at the boundary.
    REQUIRE_NOTHROW(m.write(0x200, 0x42)); // Should work, since we are inside program space.
    REQUIRE_NOTHROW(m.write(0xFFF, 0x42)); // Still inside program space, at the boundary.
    REQUIRE_THROWS(m.write(0x1000, 0x42)); // Should fail, outside program space.

    // Same arguments as above.
    REQUIRE_THROWS(m.read(0x1FF));
    REQUIRE_NOTHROW(m.read(0x200));
    REQUIRE_NOTHROW(m.read(0xFFF));
    REQUIRE_THROWS(m.read(0x1000));

    REQUIRE_NOTHROW(m.read(0x000)); // Font space, should be able to read.
    REQUIRE_NOTHROW(m.read(0x4F)); // Still font space, should be able to read.
    REQUIRE_THROWS(m.read(0x50)); // Invalid interpreter space, shouldn't be valid.


    // Serves as a test for both reading and writing, writing should result in the value 0x42 and
    // reading from the same location should be 0x42. If anything else, something is wrong.
    REQUIRE(m.read(0x200) == 0x42); // Just at edge of interpreter space.
    REQUIRE(m.read(0xFFF) == 0x42); // Just at edge of program space.
}

TEST_CASE("Copying existing program to memory.", "[memory, copying]") {
    const ch8::byte program[4] = {0xC0, 0xDE, 0xBE, 0xEF};
    ch8::Memory memory {program, sizeof(program)}; // Should contain 0xCODEBEEF at 0x200.

    // If anything else, wrong.
    REQUIRE(memory.read(0x200) == 0xC0);
    REQUIRE(memory.read(0x201) == 0xDE);
    REQUIRE(memory.read(0x202) == 0xBE);
    REQUIRE(memory.read(0x203) == 0xEF);
}

TEST_CASE("Making sure font is copied.", "[memory, font]") {
    ch8::Memory memory {nullptr, 0}; // Empty memory should still have font.
    REQUIRE(memory.read(0x000) == 0xF0); // First part of 0.
    REQUIRE(memory.read(0x04f) == 0x80); // Last part of F.
}
