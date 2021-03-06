/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ART_COMPILER_UTILS_ARM_CONSTANTS_ARM_H_
#define ART_COMPILER_UTILS_ARM_CONSTANTS_ARM_H_

#include <stdint.h>

#include <iosfwd>

#include "arch/arm/registers_arm.h"
#include "base/casts.h"
#include "base/logging.h"
#include "globals.h"

namespace art {
namespace arm {

// Defines constants and accessor classes to assemble, disassemble and
// simulate ARM instructions.
//
// Section references in the code refer to the "ARM Architecture
// Reference Manual ARMv7-A and ARMv7-R edition", issue C.b (24 July
// 2012).
//
// Constants for specific fields are defined in their respective named enums.
// General constants are in an anonymous enum in class Instr.

// 4 bits option for the dmb instruction.
// Order and values follows those of the ARM Architecture Reference Manual.
enum DmbOptions {
  SY = 0xf,
  ST = 0xe,
  ISH = 0xb,
  ISHST = 0xa,
  NSH = 0x7,
  NSHST = 0x6
};

enum ScaleFactor {
  TIMES_1 = 0,
  TIMES_2 = 1,
  TIMES_4 = 2,
  TIMES_8 = 3
};

// Values for double-precision floating point registers.
enum DRegister {  // private marker to avoid generate-operator-out.py from processing.
  D0  = 0,
  D1  = 1,
  D2  = 2,
  D3  = 3,
  D4  = 4,
  D5  = 5,
  D6  = 6,
  D7  = 7,
  D8  = 8,
  D9  = 9,
  D10 = 10,
  D11 = 11,
  D12 = 12,
  D13 = 13,
  D14 = 14,
  D15 = 15,
  D16 = 16,
  D17 = 17,
  D18 = 18,
  D19 = 19,
  D20 = 20,
  D21 = 21,
  D22 = 22,
  D23 = 23,
  D24 = 24,
  D25 = 25,
  D26 = 26,
  D27 = 27,
  D28 = 28,
  D29 = 29,
  D30 = 30,
  D31 = 31,
  kNumberOfDRegisters = 32,
  kNumberOfOverlappingDRegisters = 16,
  kNoDRegister = -1,
};
std::ostream& operator<<(std::ostream& os, const DRegister& rhs);

#ifdef MTK_ART_COMMON
// Values for double-precision floating point registers.
enum VRegister {  // private marker to avoid generate-operator-out.py from processing.
  Q0  = 0,
  Q1  = 1,
  Q2  = 2,
  Q3  = 3,
  Q4  = 4,
  Q5  = 5,
  Q6  = 6,
  Q7  = 7,
  Q8  = 8,
  Q9  = 9,
  Q10 = 10,
  Q11 = 11,
  Q12 = 12,
  Q13 = 13,
  Q14 = 14,
  Q15 = 15,
  kNumberOfQRegisters = 16,
  kNumberOfOverlappingQRegisters = 8,
  kNoQRegister = -1,
};
std::ostream& operator<<(std::ostream& os, const VRegister& rhs);
#endif

// Values for the condition field as defined in Table A8-1 "Condition
// codes" (refer to Section A8.3 "Conditional execution").
enum Condition {  // private marker to avoid generate-operator-out.py from processing.
  kNoCondition = -1,
  //           Meaning (integer)                      | Meaning (floating-point)
  //           ---------------------------------------+-----------------------------------------
  EQ = 0,   // Equal                                  | Equal
  NE = 1,   // Not equal                              | Not equal, or unordered
  CS = 2,   // Carry set                              | Greater than, equal, or unordered
  CC = 3,   // Carry clear                            | Less than
  MI = 4,   // Minus, negative                        | Less than
  PL = 5,   // Plus, positive or zero                 | Greater than, equal, or unordered
  VS = 6,   // Overflow                               | Unordered (i.e. at least one NaN operand)
  VC = 7,   // No overflow                            | Not unordered
  HI = 8,   // Unsigned higher                        | Greater than, or unordered
  LS = 9,   // Unsigned lower or same                 | Less than or equal
  GE = 10,  // Signed greater than or equal           | Greater than or equal
  LT = 11,  // Signed less than                       | Less than, or unordered
  GT = 12,  // Signed greater than                    | Greater than
  LE = 13,  // Signed less than or equal              | Less than, equal, or unordered
  AL = 14,  // Always (unconditional)                 | Always (unconditional)
  kSpecialCondition = 15,  // Special condition (refer to Section A8.3 "Conditional execution").
  kMaxCondition = 16,

  HS = CS,  // HS (unsigned higher or same) is a synonym for CS.
  LO = CC   // LO (unsigned lower) is a synonym for CC.
};
std::ostream& operator<<(std::ostream& os, const Condition& rhs);


// Opcodes for Data-processing instructions (instructions with a type 0 and 1)
// as defined in section A3.4
enum Opcode {
  kNoOperand = -1,
  AND = 0,   // Logical AND
  EOR = 1,   // Logical Exclusive OR
  SUB = 2,   // Subtract
  RSB = 3,   // Reverse Subtract
  ADD = 4,   // Add
  ADC = 5,   // Add with Carry
  SBC = 6,   // Subtract with Carry
  RSC = 7,   // Reverse Subtract with Carry
  TST = 8,   // Test
  TEQ = 9,   // Test Equivalence
  CMP = 10,  // Compare
  CMN = 11,  // Compare Negated
  ORR = 12,  // Logical (inclusive) OR
  MOV = 13,  // Move
  BIC = 14,  // Bit Clear
  MVN = 15,  // Move Not
  ORN = 16,  // Logical OR NOT.
  kMaxOperand = 17
};
std::ostream& operator<<(std::ostream& os, const Opcode& rhs);

// Shifter types for Data-processing operands as defined in section A5.1.2.
enum Shift {
  kNoShift = -1,
  LSL = 0,  // Logical shift left
  LSR = 1,  // Logical shift right
  ASR = 2,  // Arithmetic shift right
  ROR = 3,  // Rotate right
  RRX = 4,  // Rotate right with extend.
  kMaxShift
};
std::ostream& operator<<(std::ostream& os, const Shift& rhs);

// Constants used for the decoding or encoding of the individual fields of
// instructions. Based on the "Figure 3-1 ARM instruction set summary".
enum InstructionFields {  // private marker to avoid generate-operator-out.py from processing.
  kConditionShift = 28,
  kConditionBits = 4,
  kTypeShift = 25,
  kTypeBits = 3,
  kLinkShift = 24,
  kLinkBits = 1,
  kUShift = 23,
  kUBits = 1,
  kOpcodeShift = 21,
  kOpcodeBits = 4,
  kSShift = 20,
  kSBits = 1,
  kRnShift = 16,
  kRnBits = 4,
  kRdShift = 12,
  kRdBits = 4,
  kRsShift = 8,
  kRsBits = 4,
  kRmShift = 0,
  kRmBits = 4,

  // Immediate instruction fields encoding.
  kRotateShift = 8,
  kRotateBits = 4,
  kImmed8Shift = 0,
  kImmed8Bits = 8,

  // Shift instruction register fields encodings.
  kShiftImmShift = 7,
  kShiftRegisterShift = 8,
  kShiftImmBits = 5,
  kShiftShift = 5,
  kShiftBits = 2,

  // Load/store instruction offset field encoding.
  kOffset12Shift = 0,
  kOffset12Bits = 12,
  kOffset12Mask = 0x00000fff,

  // Mul instruction register fields encodings.
  kMulRdShift = 16,
  kMulRdBits = 4,
  kMulRnShift = 12,
  kMulRnBits = 4,

  kBranchOffsetMask = 0x00ffffff
};

// Size (in bytes) of registers.
const int kRegisterSize = 4;

// List of registers used in load/store multiple.
typedef uint16_t RegList;

// The class Instr enables access to individual fields defined in the ARM
// architecture instruction set encoding as described in figure A3-1.
//
// Example: Test whether the instruction at ptr does set the condition code
// bits.
//
// bool InstructionSetsConditionCodes(uint8_t* ptr) {
//   Instr* instr = Instr::At(ptr);
//   int type = instr->TypeField();
//   return ((type == 0) || (type == 1)) && instr->HasS();
// }
//
class Instr {
 public:
  enum {
    kInstrSize = 4,
    kInstrSizeLog2 = 2,
    kPCReadOffset = 8
  };

  bool IsBreakPoint() {
    return IsBkpt();
  }

  // Get the raw instruction bits.
  int32_t InstructionBits() const {
    return *reinterpret_cast<const int32_t*>(this);
  }

  // Set the raw instruction bits to value.
  void SetInstructionBits(int32_t value) {
    *reinterpret_cast<int32_t*>(this) = value;
  }

  // Read one particular bit out of the instruction bits.
  int Bit(int nr) const {
    return (InstructionBits() >> nr) & 1;
  }

  // Read a bit field out of the instruction bits.
  int Bits(int shift, int count) const {
    return (InstructionBits() >> shift) & ((1 << count) - 1);
  }


  // Accessors for the different named fields used in the ARM encoding.
  // The naming of these accessor corresponds to figure A3-1.
  // Generally applicable fields
  Condition ConditionField() const {
    return static_cast<Condition>(Bits(kConditionShift, kConditionBits));
  }
  int TypeField() const { return Bits(kTypeShift, kTypeBits); }

  Register RnField() const { return static_cast<Register>(
                                        Bits(kRnShift, kRnBits)); }
  Register RdField() const { return static_cast<Register>(
                                        Bits(kRdShift, kRdBits)); }

  // Fields used in Data processing instructions
  Opcode OpcodeField() const {
    return static_cast<Opcode>(Bits(kOpcodeShift, kOpcodeBits));
  }
  int SField() const { return Bits(kSShift, kSBits); }
  // with register
  Register RmField() const {
    return static_cast<Register>(Bits(kRmShift, kRmBits));
  }
  Shift ShiftField() const { return static_cast<Shift>(
                                        Bits(kShiftShift, kShiftBits)); }
  int RegShiftField() const { return Bit(4); }
  Register RsField() const {
    return static_cast<Register>(Bits(kRsShift, kRsBits));
  }
  int ShiftAmountField() const { return Bits(kShiftImmShift,
                                                    kShiftImmBits); }
  // with immediate
  int RotateField() const { return Bits(kRotateShift, kRotateBits); }
  int Immed8Field() const { return Bits(kImmed8Shift, kImmed8Bits); }

  // Fields used in Load/Store instructions
  int PUField() const { return Bits(23, 2); }
  int  BField() const { return Bit(22); }
  int  WField() const { return Bit(21); }
  int  LField() const { return Bit(20); }
  // with register uses same fields as Data processing instructions above
  // with immediate
  int Offset12Field() const { return Bits(kOffset12Shift,
                                                 kOffset12Bits); }
  // multiple
  int RlistField() const { return Bits(0, 16); }
  // extra loads and stores
  int SignField() const { return Bit(6); }
  int HField() const { return Bit(5); }
  int ImmedHField() const { return Bits(8, 4); }
  int ImmedLField() const { return Bits(0, 4); }

  // Fields used in Branch instructions
  int LinkField() const { return Bits(kLinkShift, kLinkBits); }
  int SImmed24Field() const { return ((InstructionBits() << 8) >> 8); }

  // Fields used in Supervisor Call instructions
  uint32_t SvcField() const { return Bits(0, 24); }

  // Field used in Breakpoint instruction
  uint16_t BkptField() const {
    return ((Bits(8, 12) << 4) | Bits(0, 4));
  }

  // Field used in 16-bit immediate move instructions
  uint16_t MovwField() const {
    return ((Bits(16, 4) << 12) | Bits(0, 12));
  }

  // Field used in VFP float immediate move instruction
  float ImmFloatField() const {
    uint32_t imm32 = (Bit(19) << 31) | (((1 << 5) - Bit(18)) << 25) |
                     (Bits(16, 2) << 23) | (Bits(0, 4) << 19);
    return bit_cast<float, uint32_t>(imm32);
  }

  // Field used in VFP double immediate move instruction
  double ImmDoubleField() const {
    uint64_t imm64 = (Bit(19)*(1LL << 63)) | (((1LL << 8) - Bit(18)) << 54) |
                     (Bits(16, 2)*(1LL << 52)) | (Bits(0, 4)*(1LL << 48));
    return bit_cast<double, uint64_t>(imm64);
  }

  // Test for data processing instructions of type 0 or 1.
  // See "ARM Architecture Reference Manual ARMv7-A and ARMv7-R edition",
  // section A5.1 "ARM instruction set encoding".
  bool IsDataProcessing() const {
    CHECK_NE(ConditionField(), kSpecialCondition);
    CHECK_EQ(Bits(26, 2), 0);  // Type 0 or 1.
    return ((Bits(20, 5) & 0x19) != 0x10) &&
      ((Bit(25) == 1) ||  // Data processing immediate.
       (Bit(4) == 0) ||  // Data processing register.
       (Bit(7) == 0));  // Data processing register-shifted register.
  }

  // Tests for special encodings of type 0 instructions (extra loads and stores,
  // as well as multiplications, synchronization primitives, and miscellaneous).
  // Can only be called for a type 0 or 1 instruction.
  bool IsMiscellaneous() const {
    CHECK_EQ(Bits(26, 2), 0);  // Type 0 or 1.
    return ((Bit(25) == 0) && ((Bits(20, 5) & 0x19) == 0x10) && (Bit(7) == 0));
  }
  bool IsMultiplyOrSyncPrimitive() const {
    CHECK_EQ(Bits(26, 2), 0);  // Type 0 or 1.
    return ((Bit(25) == 0) && (Bits(4, 4) == 9));
  }

  // Test for Supervisor Call instruction.
  bool IsSvc() const {
    return ((InstructionBits() & 0xff000000) == 0xef000000);
  }

  // Test for Breakpoint instruction.
  bool IsBkpt() const {
    return ((InstructionBits() & 0xfff000f0) == 0xe1200070);
  }

  // VFP register fields.
  SRegister SnField() const {
    return static_cast<SRegister>((Bits(kRnShift, kRnBits) << 1) + Bit(7));
  }
  SRegister SdField() const {
    return static_cast<SRegister>((Bits(kRdShift, kRdBits) << 1) + Bit(22));
  }
  SRegister SmField() const {
    return static_cast<SRegister>((Bits(kRmShift, kRmBits) << 1) + Bit(5));
  }
  DRegister DnField() const {
    return static_cast<DRegister>(Bits(kRnShift, kRnBits) + (Bit(7) << 4));
  }
  DRegister DdField() const {
    return static_cast<DRegister>(Bits(kRdShift, kRdBits) + (Bit(22) << 4));
  }
  DRegister DmField() const {
    return static_cast<DRegister>(Bits(kRmShift, kRmBits) + (Bit(5) << 4));
  }

  // Test for VFP data processing or single transfer instructions of type 7.
  bool IsVFPDataProcessingOrSingleTransfer() const {
    CHECK_NE(ConditionField(), kSpecialCondition);
    CHECK_EQ(TypeField(), 7);
    return ((Bit(24) == 0) && (Bits(9, 3) == 5));
    // Bit(4) == 0: Data Processing
    // Bit(4) == 1: 8, 16, or 32-bit Transfer between ARM Core and VFP
  }

  // Test for VFP 64-bit transfer instructions of type 6.
  bool IsVFPDoubleTransfer() const {
    CHECK_NE(ConditionField(), kSpecialCondition);
    CHECK_EQ(TypeField(), 6);
    return ((Bits(21, 4) == 2) && (Bits(9, 3) == 5) &&
            ((Bits(4, 4) & 0xd) == 1));
  }

  // Test for VFP load and store instructions of type 6.
  bool IsVFPLoadStore() const {
    CHECK_NE(ConditionField(), kSpecialCondition);
    CHECK_EQ(TypeField(), 6);
    return ((Bits(20, 5) & 0x12) == 0x10) && (Bits(9, 3) == 5);
  }

  // Special accessors that test for existence of a value.
  bool HasS() const { return SField() == 1; }
  bool HasB() const { return BField() == 1; }
  bool HasW() const { return WField() == 1; }
  bool HasL() const { return LField() == 1; }
  bool HasSign() const { return SignField() == 1; }
  bool HasH() const { return HField() == 1; }
  bool HasLink() const { return LinkField() == 1; }

  // Instructions are read out of a code stream. The only way to get a
  // reference to an instruction is to convert a pointer. There is no way
  // to allocate or create instances of class Instr.
  // Use the At(pc) function to create references to Instr.
  static Instr* At(uintptr_t pc) { return reinterpret_cast<Instr*>(pc); }
  Instr* Next() { return this + kInstrSize; }

 private:
  // We need to prevent the creation of instances of class Instr.
  DISALLOW_IMPLICIT_CONSTRUCTORS(Instr);
};

}  // namespace arm
}  // namespace art

#endif  // ART_COMPILER_UTILS_ARM_CONSTANTS_ARM_H_
