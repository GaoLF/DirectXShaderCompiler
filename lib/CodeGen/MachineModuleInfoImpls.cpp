//===-- llvm/CodeGen/MachineModuleInfoImpls.cpp ---------------------------===//
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// MachineModuleInfoImpls.cpp                                                //
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
// Licensed under the MIT license. See COPYRIGHT in the project root for     //
// full license information.                                                 //
//                                                                           //
// This file implements object-file format specific implementations of       //
// MachineModuleInfoImpl.                                                    //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "llvm/CodeGen/MachineModuleInfoImpls.h"
#include "llvm/MC/MCSymbol.h"
using namespace llvm;

//===----------------------------------------------------------------------===//
// MachineModuleInfoMachO
//===----------------------------------------------------------------------===//

// Out of line virtual method.
void MachineModuleInfoMachO::anchor() {}
void MachineModuleInfoELF::anchor() {}


// HLSL Change: changed calling convention to __cdecl
static int __cdecl SortSymbolPair(const void *LHS, const void *RHS) {
  typedef std::pair<MCSymbol*, MachineModuleInfoImpl::StubValueTy> PairTy;
  const MCSymbol *LHSS = ((const PairTy *)LHS)->first;
  const MCSymbol *RHSS = ((const PairTy *)RHS)->first;
  return LHSS->getName().compare(RHSS->getName());
}

MachineModuleInfoImpl::SymbolListTy MachineModuleInfoImpl::getSortedStubs(
    DenseMap<MCSymbol *, MachineModuleInfoImpl::StubValueTy> &Map) {
  MachineModuleInfoImpl::SymbolListTy List(Map.begin(), Map.end());

  if (!List.empty())
    qsort(&List[0], List.size(), sizeof(List[0]), SortSymbolPair);

  Map.clear();
  return List;
}
