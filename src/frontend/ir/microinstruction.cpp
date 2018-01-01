/* This file is part of the dynarmic project.
 * Copyright (c) 2016 MerryMage
 * This software may be used and distributed according to the terms of the GNU
 * General Public License version 2 or any later version.
 */

#include <algorithm>

#include "common/assert.h"
#include "frontend/ir/microinstruction.h"

namespace Dynarmic {
namespace IR {

bool Inst::IsArithmeticShift() const {
    return op == Opcode::ArithmeticShiftRight;
}

bool Inst::IsCircularShift() const {
    return op == Opcode::RotateRight ||
           op == Opcode::RotateRightExtended;
}

bool Inst::IsLogicalShift() const {
    switch (op) {
    case Opcode::LogicalShiftLeft:
    case Opcode::LogicalShiftRight:
    case Opcode::LogicalShiftRight64:
        return true;

    default:
        return false;
    }
}

bool Inst::IsShift() const {
    return IsArithmeticShift() ||
           IsCircularShift()   ||
           IsLogicalShift();
}

bool Inst::IsSharedMemoryRead() const {
    switch (op) {
    case Opcode::A32ReadMemory8:
    case Opcode::A32ReadMemory16:
    case Opcode::A32ReadMemory32:
    case Opcode::A32ReadMemory64:
        return true;

    default:
        return false;
    }
}

bool Inst::IsSharedMemoryWrite() const {
    switch (op) {
    case Opcode::A32WriteMemory8:
    case Opcode::A32WriteMemory16:
    case Opcode::A32WriteMemory32:
    case Opcode::A32WriteMemory64:
        return true;

    default:
        return false;
    }
}

bool Inst::IsSharedMemoryReadOrWrite() const {
    return IsSharedMemoryRead() || IsSharedMemoryWrite();
}

bool Inst::IsExclusiveMemoryWrite() const {
    switch (op) {
    case Opcode::A32ExclusiveWriteMemory8:
    case Opcode::A32ExclusiveWriteMemory16:
    case Opcode::A32ExclusiveWriteMemory32:
    case Opcode::A32ExclusiveWriteMemory64:
        return true;

    default:
        return false;
    }
}

bool Inst::IsMemoryRead() const {
    return IsSharedMemoryRead();
}

bool Inst::IsMemoryWrite() const {
    return IsSharedMemoryWrite() || IsExclusiveMemoryWrite();
}

bool Inst::IsMemoryReadOrWrite() const {
    return IsMemoryRead() || IsMemoryWrite();
}

bool Inst::ReadsFromCPSR() const {
    switch (op) {
    case Opcode::A32GetCpsr:
    case Opcode::A32GetNFlag:
    case Opcode::A32GetZFlag:
    case Opcode::A32GetCFlag:
    case Opcode::A32GetVFlag:
    case Opcode::A32GetGEFlags:
        return true;

    default:
        return false;
    }
}

bool Inst::WritesToCPSR() const {
    switch (op) {
    case Opcode::A32SetCpsr:
    case Opcode::A32SetCpsrNZCV:
    case Opcode::A32SetCpsrNZCVQ:
    case Opcode::A32SetNFlag:
    case Opcode::A32SetZFlag:
    case Opcode::A32SetCFlag:
    case Opcode::A32SetVFlag:
    case Opcode::A32OrQFlag:
    case Opcode::A32SetGEFlags:
    case Opcode::A32SetGEFlagsCompressed:
        return true;

    default:
        return false;
    }
}

bool Inst::ReadsFromCoreRegister() const {
    switch (op) {
    case Opcode::A32GetRegister:
    case Opcode::A32GetExtendedRegister32:
    case Opcode::A32GetExtendedRegister64:
        return true;

    default:
        return false;
    }
}

bool Inst::WritesToCoreRegister() const {
    switch (op) {
    case Opcode::A32SetRegister:
    case Opcode::A32SetExtendedRegister32:
    case Opcode::A32SetExtendedRegister64:
    case Opcode::A32BXWritePC:
        return true;

    default:
        return false;
    }
}

bool Inst::ReadsFromFPSCR() const {
    switch (op) {
    case Opcode::A32GetFpscr:
    case Opcode::A32GetFpscrNZCV:
    case Opcode::FPAbs32:
    case Opcode::FPAbs64:
    case Opcode::FPAdd32:
    case Opcode::FPAdd64:
    case Opcode::FPCompare32:
    case Opcode::FPCompare64:
    case Opcode::FPDiv32:
    case Opcode::FPDiv64:
    case Opcode::FPMul32:
    case Opcode::FPMul64:
    case Opcode::FPNeg32:
    case Opcode::FPNeg64:
    case Opcode::FPSqrt32:
    case Opcode::FPSqrt64:
    case Opcode::FPSub32:
    case Opcode::FPSub64:
        return true;

    default:
        return false;
    }
}

bool Inst::WritesToFPSCR() const {
    switch (op) {
    case Opcode::A32SetFpscr:
    case Opcode::A32SetFpscrNZCV:
    case Opcode::FPAbs32:
    case Opcode::FPAbs64:
    case Opcode::FPAdd32:
    case Opcode::FPAdd64:
    case Opcode::FPCompare32:
    case Opcode::FPCompare64:
    case Opcode::FPDiv32:
    case Opcode::FPDiv64:
    case Opcode::FPMul32:
    case Opcode::FPMul64:
    case Opcode::FPNeg32:
    case Opcode::FPNeg64:
    case Opcode::FPSqrt32:
    case Opcode::FPSqrt64:
    case Opcode::FPSub32:
    case Opcode::FPSub64:
        return true;

    default:
        return false;
    }
}

bool Inst::CausesCPUException() const {
    return op == Opcode::Breakpoint ||
           op == Opcode::A32CallSupervisor;
}

bool Inst::AltersExclusiveState() const {
    return op == Opcode::A32ClearExclusive ||
           op == Opcode::A32SetExclusive   ||
           IsExclusiveMemoryWrite();
}

bool Inst::IsCoprocessorInstruction() const {
    switch (op) {
    case Opcode::A32CoprocInternalOperation:
    case Opcode::A32CoprocSendOneWord:
    case Opcode::A32CoprocSendTwoWords:
    case Opcode::A32CoprocGetOneWord:
    case Opcode::A32CoprocGetTwoWords:
    case Opcode::A32CoprocLoadWords:
    case Opcode::A32CoprocStoreWords:
        return true;

    default:
        return false;
    }
}

bool Inst::MayHaveSideEffects() const {
    return op == Opcode::PushRSB  ||
           CausesCPUException()   ||
           WritesToCoreRegister() ||
           WritesToCPSR()         ||
           WritesToFPSCR()        ||
           AltersExclusiveState() ||
           IsMemoryWrite()        ||
           IsCoprocessorInstruction();
}

bool Inst::AreAllArgsImmediates() const {
    return std::all_of(args.begin(), args.begin() + NumArgs(), [](const auto& value){ return value.IsImmediate(); });
}

bool Inst::HasAssociatedPseudoOperation() const {
    return carry_inst || overflow_inst || ge_inst;
}

Inst* Inst::GetAssociatedPseudoOperation(Opcode opcode) {
    // This is faster than doing a search through the block.
    switch (opcode) {
    case IR::Opcode::GetCarryFromOp:
        ASSERT(!carry_inst || carry_inst->GetOpcode() == Opcode::GetCarryFromOp);
        return carry_inst;
    case IR::Opcode::GetOverflowFromOp:
        ASSERT(!overflow_inst || overflow_inst->GetOpcode() == Opcode::GetOverflowFromOp);
        return overflow_inst;
    case IR::Opcode::GetGEFromOp:
        ASSERT(!ge_inst || ge_inst->GetOpcode() == Opcode::GetGEFromOp);
        return ge_inst;
    default:
        break;
    }

    ASSERT_MSG(false, "Not a valid pseudo-operation");
    return nullptr;
}

Type Inst::GetType() const {
    if (op == Opcode::Identity)
        return args[0].GetType();
    return GetTypeOf(op);
}

Value Inst::GetArg(size_t index) const {
    ASSERT(index < GetNumArgsOf(op));
    ASSERT(!args[index].IsEmpty());

    return args[index];
}

void Inst::SetArg(size_t index, Value value) {
    ASSERT(index < GetNumArgsOf(op));
    ASSERT(AreTypesCompatible(value.GetType(), GetArgTypeOf(op, index)));

    if (!args[index].IsImmediate()) {
        UndoUse(args[index]);
    }
    if (!value.IsImmediate()) {
        Use(value);
    }

    args[index] = value;
}

void Inst::Invalidate() {
    for (auto& value : args) {
        if (!value.IsImmediate()) {
            UndoUse(value);
        }
        value = {};
    }
    op = Opcode::Void;
}

void Inst::ReplaceUsesWith(Value replacement) {
    Invalidate();

    op = Opcode::Identity;

    if (!replacement.IsImmediate()) {
        Use(replacement);
    }

    args[0] = replacement;
}

void Inst::Use(const Value& value) {
    value.GetInst()->use_count++;

    switch (op){
    case Opcode::GetCarryFromOp:
        ASSERT_MSG(!value.GetInst()->carry_inst, "Only one of each type of pseudo-op allowed");
        value.GetInst()->carry_inst = this;
        break;
    case Opcode::GetOverflowFromOp:
        ASSERT_MSG(!value.GetInst()->overflow_inst, "Only one of each type of pseudo-op allowed");
        value.GetInst()->overflow_inst = this;
        break;
    case Opcode::GetGEFromOp:
        ASSERT_MSG(!value.GetInst()->ge_inst, "Only one of each type of pseudo-op allowed");
        value.GetInst()->ge_inst = this;
        break;
    default:
        break;
    }
}

void Inst::UndoUse(const Value& value) {
    value.GetInst()->use_count--;

    switch (op){
    case Opcode::GetCarryFromOp:
        ASSERT(value.GetInst()->carry_inst->GetOpcode() == Opcode::GetCarryFromOp);
        value.GetInst()->carry_inst = nullptr;
        break;
    case Opcode::GetOverflowFromOp:
        ASSERT(value.GetInst()->overflow_inst->GetOpcode() == Opcode::GetOverflowFromOp);
        value.GetInst()->overflow_inst = nullptr;
        break;
    case Opcode::GetGEFromOp:
        ASSERT(value.GetInst()->ge_inst->GetOpcode() == Opcode::GetGEFromOp);
        value.GetInst()->ge_inst = nullptr;
        break;
    default:
        break;
    }
}

} // namespace IR
} // namespace Dynarmic
