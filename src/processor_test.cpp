#include "catch.hpp"
#include "processor.hpp"

TEST_CASE("Processor constructed state.", "[processor, initial_state]") {
    ch8::Processor p;
    REQUIRE(p.register_state(ch8::Processor::Register::V0) == 0x00);
    REQUIRE(p.register_state(ch8::Processor::Register::V8) == 0x00);
    REQUIRE(p.register_state(ch8::Processor::Register::VF) == 0x00);
    REQUIRE(p.register_state(ch8::Processor::Register::ST) == 0x00);
    REQUIRE(p.register_state(ch8::Processor::Register::DT) == 0x00);
    REQUIRE(p.register_state(ch8::Processor::Register::PC) == 0x200);
    REQUIRE(p.register_state(ch8::Processor::Register::I) == 0x0000);
    REQUIRE(p.register_state(ch8::Processor::Register::SP) == 0x00);
}

TEST_CASE("SYS ignored.", "[processor, inst_sys]") {
    ch8::Processor p;
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::SYS_A, 0x02, 0x00)); // SYS 0x200.
    REQUIRE(p.register_state(ch8::Processor::Register::PC) == 0x200); // State unchanged.
}

TEST_CASE("CLS clears screen.", "[processor, inst_cls]") {
    ch8::Processor p;
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::CLS, 0x00, 0xE0)); // CLS.
    REQUIRE(p.display_buffer()[0] == 0x00); // First pixel must be clear.
    REQUIRE(p.display_buffer()[64 * 32 - 1] == 0x00); // Last pixel must be clear.

    // Issue more tests later once DRW is implemented.
}

TEST_CASE("RET pops the stack and modifies PC.", "[processor, inst_ret]") {
    ch8::Processor p;
    REQUIRE(p.register_state(ch8::Processor::Register::PC) == 0x200); // Default.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::RET, 0x00, 0xEE)); // RET.
    REQUIRE(p.register_state(ch8::Processor::Register::PC) == 0x000); // Default stack content at SP.

    // Issue more tests later once CALL is implemented.
}
