#include "catch.hpp"
#include "processor.hpp"

ch8::byte prog[4096] = {0}; // Some memory for the test.
ch8::Memory m {prog, 4096}; // Memory abstraction for test, a total 4 KiB of memory.
TEST_CASE("Processor constructed state.", "[processor, initial_state]") {
    ch8::Processor p {m};
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
    ch8::Processor p {m};
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::SYS_A, 0x02, 0x00)); // SYS 0x200.
    REQUIRE(p.register_state(ch8::Processor::Register::PC) == 0x200); // State unchanged.
}

TEST_CASE("CLS clears screen.", "[processor, inst_cls]") {
    ch8::Processor p {m};
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::CLS, 0x00, 0xE0)); // CLS.
    REQUIRE(p.display_buffer()[0] == 0x00); // First pixel must be clear.
    REQUIRE(p.display_buffer()[64 * 32 - 1] == 0x00); // Last pixel must be clear.

    // Issue more tests later once DRW is implemented.
}

TEST_CASE("RET pops the stack and modifies PC.", "[processor, inst_ret]") {
    ch8::Processor p {m};
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
    ch8::Processor p {m};
    REQUIRE(p.register_state(ch8::Processor::Register::PC) == 0x200); // Default PC location.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::JP_A, 0x12, 0x08)); // JP 0x208.
    REQUIRE(p.register_state(ch8::Processor::Register::PC) == 0x208); // Needs to be at 0x208.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::JP_A, 0x1F, 0xFF)); // JP 0xFFF.
    REQUIRE(p.register_state(ch8::Processor::Register::PC) == 0xFFF); // Needs to be at 0xFFF.
}

TEST_CASE("CALL calls a subroutine, storing return address.", "[processor, inst_calla]") {
    ch8::Processor p {m};
    REQUIRE(p.register_state(ch8::Processor::Register::PC) == 0x200); // Default PC location.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::CALL_A, 0x22, 0x08)); // CALL 0x208.
    REQUIRE(p.register_state(ch8::Processor::Register::PC) == 0x208); // Needs to be at 0x208.
    REQUIRE(p.register_state(ch8::Processor::Register::SP) == 0x01); // Needs to have incremented SP.

    REQUIRE_NOTHROW(p.execute(ch8::Instruction::CALL_A, 0x2F, 0xFF)); // CALL 0xFFF.
    REQUIRE(p.register_state(ch8::Processor::Register::PC) == 0xFFF); // Needs to be at 0xFFF.
    REQUIRE(p.register_state(ch8::Processor::Register::SP) == 0x02); // Needs to have incremented SP.
}

TEST_CASE("SE compares register to constant, skips next instruction if same.", "[processor, inst_serc]") {
    ch8::Processor p {m};
    REQUIRE(p.register_state(ch8::Processor::Register::PC) == 0x200); // Default PC location.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::SE_RC, 0x30, 0x21)); // SE V0, 0x21 (V0 = 0x00).
    REQUIRE(p.register_state(ch8::Processor::Register::PC) == 0x200); // Should remain unchanged.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::SE_RC, 0x30, 0x00)); // SE V0, 0x00 (V0 = 0x00).
    REQUIRE(p.register_state(ch8::Processor::Register::PC) == 0x202); // Should increment to next instruction.
                                                                      // Note: will increment PC by 4 later.
}

TEST_CASE("SNE compares register to constant, skips next instruction if same.", "[processor, inst_snerc]") {
    ch8::Processor p {m};
    REQUIRE(p.register_state(ch8::Processor::Register::PC) == 0x200); // Default PC location.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::SNE_RC, 0x40, 0x21)); // SNE V0, 0x21 (V0 = 0x00).
    REQUIRE(p.register_state(ch8::Processor::Register::PC) == 0x202); // Should increment to next instruction.
                                                                      // Note: will increment PC by 4 later.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::SNE_RC, 0x40, 0x00)); // SE V0, 0x00 (V0 = 0x00).
    REQUIRE(p.register_state(ch8::Processor::Register::PC) == 0x202); // Should remain unchanged.
}

TEST_CASE("SE compares register to another register, skips if true.", "[processor, inst_serr]") {
    ch8::Processor p {m};
    REQUIRE(p.register_state(ch8::Processor::Register::PC) == 0x200); // Default PC location.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::SE_RR, 0x50, 0x00)); // SE V0, V1 (V0, V1 = 0x00).
    REQUIRE(p.register_state(ch8::Processor::Register::PC) == 0x202); // Should increment to next instruction.
                                                                      // Note: will increment PC by 4 later.
}

TEST_CASE("LD loads constant to register.", "[processor, inst_ldrc]") {
    ch8::Processor p {m};
    REQUIRE(p.register_state(ch8::Processor::Register::V0) == 0x00); // Zero per- default.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::LD_RC, 0x60, 0x42)); // LD V0, 0x42.
    REQUIRE(p.register_state(ch8::Processor::Register::V0) == 0x42); // Should contain the meaning of life.
}

TEST_CASE("ADD adds a constant to register.", "[processor, inst_addrc]") {
    ch8::Processor p {m};
    REQUIRE(p.register_state(ch8::Processor::Register::V0) == 0x00); // Zero per- default.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::LD_RC, 0x60, 0x42)); // LD V0, 0x42.
    REQUIRE(p.register_state(ch8::Processor::Register::V0) == 0x42); // Should contain the meaning of life.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::ADD_RC, 0x70, 0x03)); // ADD V0, 0x03.
    REQUIRE(p.register_state(ch8::Processor::Register::V0) == 0x45); // Random number.
}

TEST_CASE("LD loads content from x to y.", "[processor, inst_ldrr]") {
    ch8::Processor p {m};
    REQUIRE(p.register_state(ch8::Processor::Register::V0) == 0x00); // Zero per- default.
    REQUIRE(p.register_state(ch8::Processor::Register::V1) == 0x00); // Zero per- default.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::LD_RC, 0x61, 0x32)); // LD V1, 0x32.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::LD_RR, 0x80, 0x10)); // LD V0, V1.
    REQUIRE(p.register_state(ch8::Processor::Register::V0) == 0x32); // Both should be equal.
    REQUIRE(p.register_state(ch8::Processor::Register::V1) == 0x32); // Both should be equal.
}

TEST_CASE("OR performs bitwise or on both registers.", "[processor, inst_orrr]") {
    ch8::Processor p {m};
    REQUIRE(p.register_state(ch8::Processor::Register::V0) == 0x00); // Zero per- default.
    REQUIRE(p.register_state(ch8::Processor::Register::V1) == 0x00); // Zero per- default.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::LD_RC, 0x60, 0x04)); // LD V1, 0x04.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::LD_RC, 0x61, 0x02)); // LD V0, 0x02.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::OR_RR, 0x80, 0x11)); // OR V0, V1.
    REQUIRE(p.register_state(ch8::Processor::Register::V0) == 0x06); // Bitwise OR of 0b100 and 0b010 = 0b110.
}

TEST_CASE("AND performs bitwise and on both registers.", "[processor, inst_andrr]") {
    ch8::Processor p {m};
    REQUIRE(p.register_state(ch8::Processor::Register::V0) == 0x00); // Zero per- default.
    REQUIRE(p.register_state(ch8::Processor::Register::V1) == 0x00); // Zero per- default.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::LD_RC, 0x60, 0x06)); // LD V1, 0x06.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::LD_RC, 0x61, 0x02)); // LD V0, 0x02.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::AND_RR, 0x80, 0x12)); // AND V0, V1.
    REQUIRE(p.register_state(ch8::Processor::Register::V0) == 0x02); // Bitwise AND of 0b110 and 0b010 = 0b010.
}

TEST_CASE("XOR performs bitwise xor on both registers.", "[processor, inst_xorrr]") {
    ch8::Processor p {m};
    REQUIRE(p.register_state(ch8::Processor::Register::V0) == 0x00); // Zero per- default.
    REQUIRE(p.register_state(ch8::Processor::Register::V1) == 0x00); // Zero per- default.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::LD_RC, 0x60, 0x07)); // LD V1, 0x07.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::LD_RC, 0x61, 0x03)); // LD V0, 0x03.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::XOR_RR, 0x80, 0x13)); // XOR V0, V1.
    REQUIRE(p.register_state(ch8::Processor::Register::V0) == 0x04); // Bitwise XOR of 0b111 and 0b011 = 0b100.
}

TEST_CASE("ADD between registers, can result in overflow.", "[processor, inst_addrr]") {
    ch8::Processor p {m};
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::LD_RC, 0x60, 0x07)); // LD V0, 0x07.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::LD_RC, 0x61, 0x03)); // LD V1, 0x03.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::ADD_RR, 0x80, 0x14)); // ADD V0, V1.
    REQUIRE(p.register_state(ch8::Processor::Register::V0) == 0x0A); // 7 + 3 = 10, obviously.
    REQUIRE(p.register_state(ch8::Processor::Register::VF) == 0x00); // Overflow flag should NOT be set.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::LD_RC, 0x60, 0xFE)); // LD V0, 0xfe.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::LD_RC, 0x61, 0x05)); // LD V1, 0x05.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::ADD_RR, 0x80, 0x14)); // ADD V0, V1.
    REQUIRE(p.register_state(ch8::Processor::Register::V0) == 0x03); // 254 + 5 = 3, obviously :p.
    REQUIRE(p.register_state(ch8::Processor::Register::VF) == 0x01); // Overflow flag should be set.
}

TEST_CASE("SUB between registers, can result in borrow.", "[processor, inst_subrr]") {
    ch8::Processor p {m};
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::LD_RC, 0x60, 0x07)); // LD V0, 0x07.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::LD_RC, 0x61, 0x03)); // LD V1, 0x03.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::SUB_RR, 0x80, 0x15)); // SUB V0, V1.
    REQUIRE(p.register_state(ch8::Processor::Register::V0) == 0x04); // 7 - 3 = 4, obviously.
    REQUIRE(p.register_state(ch8::Processor::Register::VF) == 0x01); // Borrow flag should NOT be set.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::LD_RC, 0x60, 0x07)); // LD V0, 0x07.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::LD_RC, 0x61, 0x03)); // LD V1, 0x03.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::SUB_RR, 0x81, 0x05)); // SUB V1, V0.
    REQUIRE(p.register_state(ch8::Processor::Register::V1) == 0xFC); // 3 - 7 = 252, obviously :p.
    REQUIRE(p.register_state(ch8::Processor::Register::VF) == 0x00); // Borrow flag should be set.
}

TEST_CASE("SHR shifts first register right by one.", "[processor, inst_shrrr]") {
    ch8::Processor p {m};
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::LD_RC, 0x60, 0x08)); // LD V0, 0x02.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::SHR_RR, 0x80, 0x16)); // SHR V0, {VF}.
    REQUIRE(p.register_state(ch8::Processor::Register::V0) == 0x04); // 0x08 / 2 = 0x04.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::SHR_RR, 0x80, 0x16)); // SHR V0, {VF}.
    REQUIRE(p.register_state(ch8::Processor::Register::V0) == 0x02); // 0x04 / 2 = 0x02.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::SHR_RR, 0x80, 0x16)); // SHR V0, {VF}.
    REQUIRE(p.register_state(ch8::Processor::Register::V0) == 0x01); // 0x02 / 2 = 0x01.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::SHR_RR, 0x80, 0x16)); // SHR V0, {VF}.
    REQUIRE(p.register_state(ch8::Processor::Register::V0) == 0x00); // 0x01 / 2 = 0x01.
    REQUIRE(p.register_state(ch8::Processor::Register::VF) == 0x01); // Right shift flag set.
}

TEST_CASE("SUBN between inverse registers, can result in borrow.", "[processor, inst_subnrr]") {
    ch8::Processor p {m};
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::LD_RC, 0x61, 0x07)); // LD V1, 0x07.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::LD_RC, 0x60, 0x03)); // LD V0, 0x03.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::SUBN_RR, 0x80, 0x17)); // SUBN V0, V1.
    REQUIRE(p.register_state(ch8::Processor::Register::V0) == 0x04); // 7 - 3 = 4, obviously.
    REQUIRE(p.register_state(ch8::Processor::Register::VF) == 0x01); // Borrow flag should NOT be set.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::LD_RC, 0x61, 0x07)); // LD V1, 0x07.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::LD_RC, 0x60, 0x03)); // LD V0, 0x03.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::SUBN_RR, 0x81, 0x07)); // SUBN V1, V0.
    REQUIRE(p.register_state(ch8::Processor::Register::V1) == 0xFC); // 3 - 7 = 252, obviously :p.
    REQUIRE(p.register_state(ch8::Processor::Register::VF) == 0x00); // Borrow flag should be set.
}

TEST_CASE("SHL shifts first register left by one.", "[processor, inst_shlrr]") {
    ch8::Processor p {m};
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::LD_RC, 0x60, 0x08)); // LD V0, 0x02.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::SHL_RR, 0x80, 0x1E)); // SHL V0, {VF}.
    REQUIRE(p.register_state(ch8::Processor::Register::V0) == 0x10); // 0x08 * 2 = 0x10.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::SHL_RR, 0x80, 0x16)); // SHR V0, {VF}.
    REQUIRE(p.register_state(ch8::Processor::Register::V0) == 0x20); // 0x10 * 2 = 0x20.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::SHL_RR, 0x80, 0x1E)); // SHL V0, {VF}.
    REQUIRE(p.register_state(ch8::Processor::Register::V0) == 0x40); // 0x20 * 2 = 0x40.

    REQUIRE_NOTHROW(p.execute(ch8::Instruction::LD_RC, 0x60, 0x80)); // LD V0, 0x80 = 128.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::SHL_RR, 0x80, 0x1E)); // SHL V0, {VF}.
    REQUIRE(p.register_state(ch8::Processor::Register::V0) == 0x00); // 0x80 * 2 = 0x00 (shift out).
    REQUIRE(p.register_state(ch8::Processor::Register::VF) == 0x01); // Left shift flag set.
}

TEST_CASE("SNE compares register to register, skips next instruction if same.", "[processor, inst_snerr]") {
    ch8::Processor p {m};
    REQUIRE(p.register_state(ch8::Processor::Register::PC) == 0x200); // Default PC location.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::LD_RC, 0x60, 0x08)); // LD V0, 0x08.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::LD_RC, 0x61, 0x38)); // LD V1, 0x38.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::SNE_RR, 0x90, 0x10)); // SNE V0, V1
    REQUIRE(p.register_state(ch8::Processor::Register::PC) == 0x202); // Should increment to next instruction.
                                                                      // Note: will increment PC by 4 later.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::LD_RC, 0x60, 0x08)); // LD V0, 0x08.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::LD_RC, 0x61, 0x08)); // LD V1, 0x08.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::SNE_RR, 0x90, 0x10)); // SNE V0, V1
    REQUIRE(p.register_state(ch8::Processor::Register::PC) == 0x202); // Should remain unchanged.
}

TEST_CASE("LD loads a certain address to the I register.", "[processor, inst_ldia]") {
    ch8::Processor p {m};
    REQUIRE(p.register_state(ch8::Processor::Register::I) == 0x000);
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::LD_IA, 0xA1, 0x23)); // LD I, 0x123.
    REQUIRE(p.register_state(ch8::Processor::Register::I) == 0x123);
}

TEST_CASE("JP jumps to target address plus offset.", "[processor, inst_jpv0a]") {
    ch8::Processor p {m};
    REQUIRE(p.register_state(ch8::Processor::Register::PC) == 0x200); // Default PC location.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::JP_V0A, 0xB2, 0x08)); // JP 0x208 (V0 = 0x00).
    REQUIRE(p.register_state(ch8::Processor::Register::PC) == 0x208); // Needs to be at 0x208.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::JP_V0A, 0xBF, 0xFF)); // JP 0xFFF (V0 = 0x00).
    REQUIRE(p.register_state(ch8::Processor::Register::PC) == 0xFFF); // Needs to be at 0xFFF.

    REQUIRE_NOTHROW(p.execute(ch8::Instruction::LD_RC, 0x60, 0x02)); // LD V0, 0x02.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::JP_V0A, 0xB1, 0x23)); // JP 0x123 (V0 = 0x02).
    REQUIRE(p.register_state(ch8::Processor::Register::PC) == 0x125); // Needs to be at 0x125.
}

TEST_CASE("RND assigns a random value to register, limited to constant.", "[processor, inst_rndrc]") {
    ch8::Processor p {m};
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::RND_RC, 0xC0, 0x0F)); // RND V0, 0x0F (0-15).
    REQUIRE(p.register_state(ch8::Processor::Register::V0) <= 15); // Needs to be between 0 and 15.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::RND_RC, 0xC0, 0xFF)); // RND V0, 0xFF (0-255).
    REQUIRE(p.register_state(ch8::Processor::Register::V0) <= 255); // Needs to be between 0 and 255.
}

TEST_CASE("DRW draws a sprite from memory to the display buffer.", "[processor, inst_drwrrc]") {
    ch8::Processor p {m};
    const ch8::byte* dbuffer {p.display_buffer()};
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::CLS, 0x00, 0xEE)); // CLS.

    // Check that the screen is clear.
    REQUIRE(dbuffer[0] == 0);
    REQUIRE(dbuffer[64] == 0);
    REQUIRE(dbuffer[2 * 64] == 0);
    REQUIRE(dbuffer[3 * 64] == 0);

    // Write a sprite to memory.
    m.write(0x200, 0x80);
    m.write(0x201, 0x80);
    m.write(0x202, 0x80);
    m.write(0x203, 0x80);

    REQUIRE_NOTHROW(p.execute(ch8::Instruction::LD_IA, 0xA2, 0x00)); // LD I, 0x200.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::LD_RC, 0x60, 0x00)); // LD V0, 0x00.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::LD_RC, 0x61, 0x00)); // LD V1, 0x00.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::DRW_RRC, 0xD0, 0x14)); // DRW V0, V1, 4.

    // Check that the sprite has been written.
    REQUIRE(dbuffer[0] == 1);
    REQUIRE(dbuffer[64] == 1);
    REQUIRE(dbuffer[2 * 64] == 1);
    REQUIRE(dbuffer[3 * 64] == 1);
    REQUIRE(p.register_state(ch8::Processor::Register::VF) == 0x00); // No collision should have happened.

    // Check that the screen is clear.
    REQUIRE(dbuffer[4 * 64] == 0);
    REQUIRE(dbuffer[5 * 64] == 0);
    REQUIRE(dbuffer[6 * 64] == 0);
    REQUIRE(dbuffer[7 * 64] == 0);

    // Write sprite a bit lower, just below first sprite.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::LD_IA, 0xA2, 0x00)); // LD I, 0x200.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::LD_RC, 0x60, 0x00)); // LD V0, 0x00.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::LD_RC, 0x61, 0x04)); // LD V1, 0x04.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::DRW_RRC, 0xD0, 0x14)); // DRW V0, V1, 4.

    // Check that the sprite has been written.
    REQUIRE(dbuffer[4 * 64] == 1);
    REQUIRE(dbuffer[5 * 64] == 1);
    REQUIRE(dbuffer[6 * 64] == 1);
    REQUIRE(dbuffer[7 * 64] == 1);
    REQUIRE(p.register_state(ch8::Processor::Register::VF) == 0x00); // No collision should have happened.

    // Check that the screen is clear.
    REQUIRE(dbuffer[8 * 64] == 0);
    REQUIRE(dbuffer[9 * 64] == 0);
    REQUIRE(dbuffer[10 * 64] == 0);

    // Write sprite a bit lower, now colliding with the second sprite.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::LD_IA, 0xA2, 0x00)); // LD I, 0x200.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::LD_RC, 0x60, 0x00)); // LD V0, 0x00.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::LD_RC, 0x61, 0x07)); // LD V1, 0x07.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::DRW_RRC, 0xD0, 0x14)); // DRW V0, V1, 4.

    // Check that the sprite has been written.
    REQUIRE(dbuffer[7 * 64] == 0); // Should have been overwritten.
    REQUIRE(dbuffer[8 * 64] == 1);
    REQUIRE(dbuffer[9 * 64] == 1);
    REQUIRE(dbuffer[10 * 64] == 1);
    REQUIRE(p.register_state(ch8::Processor::Register::VF) == 0x01); // Collision should have happened.

    // Some kind of ball?!
    m.write(0x200, 0x18);
    m.write(0x201, 0x18);

    // Draw ball shaped thing at (8, 8).
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::LD_IA, 0xA2, 0x00)); // LD I, 0x200.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::LD_RC, 0x60, 0x08)); // LD V0, 0x08.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::LD_RC, 0x61, 0x08)); // LD V1, 0x08.
    REQUIRE_NOTHROW(p.execute(ch8::Instruction::DRW_RRC, 0xD0, 0x12)); // DRW V0, V1, 2.

    REQUIRE(dbuffer[8 * 64 + 8 + 3] == 1);
    REQUIRE(dbuffer[8 * 64 + 8 + 4] == 1);
    REQUIRE(dbuffer[9 * 64 + 8 + 3] == 1);
    REQUIRE(dbuffer[9 * 64 + 8 + 4] == 1);
    REQUIRE(p.register_state(ch8::Processor::Register::VF) == 0x00); // Collision should NOT have happened.
}
