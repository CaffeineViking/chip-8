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
    REQUIRE(p.register_state(ch8::Processor::Register::PC) == 0x200); // Default PC location.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::RET, 0x00, 0xEE)); // RET.
    REQUIRE(p.register_state(ch8::Processor::Register::PC) == 0x000); // Default stack content at SP.
    REQUIRE(p.register_state(ch8::Processor::Register::SP) == 0x00); // Don't have negative SP.

    // Issue more tests later once CALL is implemented.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::CALL_A, 0x22, 0x08)); // CALL 0x208.
    REQUIRE(p.register_state(ch8::Processor::Register::PC) == 0x208); // Needs to be at 0x208.
    REQUIRE(p.register_state(ch8::Processor::Register::SP) == 0x01); // Needs to have incremented SP.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::RET, 0x00, 0xEE)); // RET.
    REQUIRE(p.register_state(ch8::Processor::Register::PC) == 0x000); // Needs to be back at prev. address.
    REQUIRE(p.register_state(ch8::Processor::Register::SP) == 0x00); // Needs to have decremented SP.
}

TEST_CASE("JP jumps to target address.", "[processor, inst_jpa]") {
    ch8::Processor p;
    REQUIRE(p.register_state(ch8::Processor::Register::PC) == 0x200); // Default PC location.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::JP_A, 0x12, 0x08)); // JP 0x208.
    REQUIRE(p.register_state(ch8::Processor::Register::PC) == 0x208); // Needs to be at 0x208.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::JP_A, 0x1F, 0xFF)); // JP 0xFFF.
    REQUIRE(p.register_state(ch8::Processor::Register::PC) == 0xFFF); // Needs to be at 0xFFF.
}


TEST_CASE("CALL calls a subroutine, storing return address.", "[processor, inst_calla]") {
    ch8::Processor p;
    REQUIRE(p.register_state(ch8::Processor::Register::PC) == 0x200); // Default PC location.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::CALL_A, 0x22, 0x08)); // CALL 0x208.
    REQUIRE(p.register_state(ch8::Processor::Register::PC) == 0x208); // Needs to be at 0x208.
    REQUIRE(p.register_state(ch8::Processor::Register::SP) == 0x01); // Needs to have incremented SP.

    REQUIRE_NOTHROW(p.execute(ch8::Instruction::CALL_A, 0x2F, 0xFF)); // CALL 0xFFF.
    REQUIRE(p.register_state(ch8::Processor::Register::PC) == 0xFFF); // Needs to be at 0xFFF.
    REQUIRE(p.register_state(ch8::Processor::Register::SP) == 0x02); // Needs to have incremented SP.
}

TEST_CASE("SE compares register to constant, skips next instruction if same.", "[processor, inst_serc]") {
    ch8::Processor p;
    REQUIRE(p.register_state(ch8::Processor::Register::PC) == 0x200); // Default PC location.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::SE_RC, 0x30, 0x21)); // SE V0, 0x21 (V0 = 0x00).
    REQUIRE(p.register_state(ch8::Processor::Register::PC) == 0x200); // Should remain unchanged.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::SE_RC, 0x30, 0x00)); // SE V0, 0x00 (V0 = 0x00).
    REQUIRE(p.register_state(ch8::Processor::Register::PC) == 0x202); // Should increment to next instruction.
                                                                      // Note: will increment PC by 4 later.
}

TEST_CASE("SNE compares register to constant, skips next instruction if same.", "[processor, inst_snerc]") {
    ch8::Processor p;
    REQUIRE(p.register_state(ch8::Processor::Register::PC) == 0x200); // Default PC location.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::SNE_RC, 0x40, 0x21)); // SNE V0, 0x21 (V0 = 0x00).
    REQUIRE(p.register_state(ch8::Processor::Register::PC) == 0x202); // Should increment to next instruction.
                                                                      // Note: will increment PC by 4 later.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::SNE_RC, 0x40, 0x00)); // SE V0, 0x00 (V0 = 0x00).
    REQUIRE(p.register_state(ch8::Processor::Register::PC) == 0x202); // Should remain unchanged.
}

TEST_CASE("SE compares register to another register, skips if true.", "[processor, inst_serr]") {
    ch8::Processor p;
    REQUIRE(p.register_state(ch8::Processor::Register::PC) == 0x200); // Default PC location.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::SE_RR, 0x50, 0x00)); // SE V0, V1 (V0, V1 = 0x00).
    REQUIRE(p.register_state(ch8::Processor::Register::PC) == 0x202); // Should increment to next instruction.
                                                                      // Note: will increment PC by 4 later.
}

TEST_CASE("LD loads constant to register.", "[processor, inst_ldrc]") {
    ch8::Processor p;
    REQUIRE(p.register_state(ch8::Processor::Register::V0) == 0x00); // Zero per- default.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::LD_RC, 0x60, 0x42)); // LD V0, 0x42.
    REQUIRE(p.register_state(ch8::Processor::Register::V0) == 0x42); // Should contain the meaning of life.
}

TEST_CASE("ADD adds a constant to register.", "[processor, inst_addrc]") {
    ch8::Processor p;
    REQUIRE(p.register_state(ch8::Processor::Register::V0) == 0x00); // Zero per- default.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::LD_RC, 0x60, 0x42)); // LD V0, 0x42.
    REQUIRE(p.register_state(ch8::Processor::Register::V0) == 0x42); // Should contain the meaning of life.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::ADD_RC, 0x70, 0x03)); // ADD V0, 0x03.
    REQUIRE(p.register_state(ch8::Processor::Register::V0) == 0x45); // Random number.
}
