#include "catch.hpp"
#include "interpreter.hpp"

TEST_CASE("Parsing instructions.", "[interpreter, instruction_parser]") {
    REQUIRE(ch8::Interpreter::parse(0x00, 0xE0) == ch8::Instruction::CLS);
    REQUIRE(ch8::Interpreter::parse(0x00, 0xEE) == ch8::Instruction::RET);

    REQUIRE(ch8::Interpreter::parse(0x0A, 0xAA) == ch8::Instruction::SYS_A);
    REQUIRE(ch8::Interpreter::parse(0x1A, 0xAA) == ch8::Instruction::JP_A);
    REQUIRE(ch8::Interpreter::parse(0x2A, 0xAA) == ch8::Instruction::CALL_A);

    REQUIRE(ch8::Interpreter::parse(0x3B, 0xCC) == ch8::Instruction::SE_RC);
    REQUIRE(ch8::Interpreter::parse(0x4B, 0xCC) == ch8::Instruction::SNE_RC);
    REQUIRE(ch8::Interpreter::parse(0x5B, 0xB0) == ch8::Instruction::SE_RR);
    REQUIRE(ch8::Interpreter::parse(0x6B, 0xCC) == ch8::Instruction::LD_RC);
    REQUIRE(ch8::Interpreter::parse(0x7B, 0xCC) == ch8::Instruction::ADD_RC);

    REQUIRE(ch8::Interpreter::parse(0x8B, 0xB0) == ch8::Instruction::LD_RR);
    REQUIRE(ch8::Interpreter::parse(0x8B, 0xB1) == ch8::Instruction::OR_RR);
    REQUIRE(ch8::Interpreter::parse(0x8B, 0xB2) == ch8::Instruction::AND_RR);
    REQUIRE(ch8::Interpreter::parse(0x8B, 0xB3) == ch8::Instruction::XOR_RR);
    REQUIRE(ch8::Interpreter::parse(0x8B, 0xB4) == ch8::Instruction::ADD_RR);
    REQUIRE(ch8::Interpreter::parse(0x8B, 0xB5) == ch8::Instruction::SUB_RR);
    REQUIRE(ch8::Interpreter::parse(0x8B, 0xB6) == ch8::Instruction::SHR_RR);
    REQUIRE(ch8::Interpreter::parse(0x8B, 0xB7) == ch8::Instruction::SUBN_RR);
    REQUIRE(ch8::Interpreter::parse(0x8B, 0xBE) == ch8::Instruction::SHL_RR);
    REQUIRE(ch8::Interpreter::parse(0x9B, 0xB0) == ch8::Instruction::SNE_RR);

    REQUIRE(ch8::Interpreter::parse(0xAA, 0xAA) == ch8::Instruction::LD_IA);
    REQUIRE(ch8::Interpreter::parse(0xBA, 0xAA) == ch8::Instruction::JP_V0A);
    REQUIRE(ch8::Interpreter::parse(0xCB, 0xCC) == ch8::Instruction::RND_RC);
    REQUIRE(ch8::Interpreter::parse(0xDB, 0xBC) == ch8::Instruction::DRW_RRC);
    REQUIRE(ch8::Interpreter::parse(0xEB, 0x9E) == ch8::Instruction::SKP_R);
    REQUIRE(ch8::Interpreter::parse(0xFB, 0x07) == ch8::Instruction::LD_RD);

    REQUIRE(ch8::Interpreter::parse(0xFB, 0x0A) == ch8::Instruction::LD_RK);
    REQUIRE(ch8::Interpreter::parse(0xFB, 0x15) == ch8::Instruction::LD_DR);
    REQUIRE(ch8::Interpreter::parse(0xFB, 0x18) == ch8::Instruction::LD_SR);
    REQUIRE(ch8::Interpreter::parse(0xFB, 0x1E) == ch8::Instruction::ADD_IR);
    REQUIRE(ch8::Interpreter::parse(0xFB, 0x29) == ch8::Instruction::LD_FR);
    REQUIRE(ch8::Interpreter::parse(0xFB, 0x33) == ch8::Instruction::LD_BR);
    REQUIRE(ch8::Interpreter::parse(0xFB, 0x55) == ch8::Instruction::LD_IAR);
    REQUIRE(ch8::Interpreter::parse(0xFB, 0x65) == ch8::Instruction::LD_RAI);
}

TEST_CASE("Parsing arguments for instruction.", "[interpretr, argument_parser]") {
}
