// This file provides the interface for the Loop Profiler pass.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_PROFILER_H
#define LLVM_PROFILER_H

#include "llvm/Analysis/LoopPass.h"
#include "llvm/Transforms/Instrumentation.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/LoopAnalysisManager.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/PassRegistry.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Transforms/Scalar/LoopPassManager.h"
#include <algorithm>
#include <string>
#include <unordered_map>

using namespace llvm;

namespace llvm {

// Count total number of loops in the program (including artifical loops)
class CountLoops {
public:
  bool runOnLoop(Loop *L);
};

// New pass manger driver class

class CountLoopsPass : public PassInfoMixin<CountLoopsPass> {
  public:
  PreservedAnalyses run(Loop &L, LoopAnalysisManager &AM,
                        LoopStandardAnalysisResults &AR, LPMUpdater &U) {
    CountLoops CL;
    CL.runOnLoop(&L);
    return PreservedAnalyses::all();
  }
};

// Legacy PM Driver
class CountLoopsLegacy : public LoopPass {

public:
  static char ID;
  CountLoopsLegacy() : LoopPass(ID) {
    initializeCountLoopsLegacyPass(*PassRegistry::getPassRegistry());
  }
  void getAnalysisUsage(AnalysisUsage &AU) const;
  bool runOnLoop(Loop *L, LPPassManager &) override;
};

// Insert instrumentation in header and in exit block of loop
class InsertTimer {
  uint64_t iter = 0;
  public:
  bool runOnLoop(Loop *L);
};

struct InsertTimerLegacy : public LoopPass {
  static char ID;
  uint64_t iter = 0;
  InsertTimerLegacy() : LoopPass(ID) {
    initializeInsertTimerLegacyPass(*PassRegistry::getPassRegistry());
  }
  void getAnalysisUsage(AnalysisUsage &AU) const override;
  bool runOnLoop(Loop *L, LPPassManager &) override;
};

// New PM Driver

class InsertTimerPass : public PassInfoMixin<InsertTimerPass> {
  public:
  PreservedAnalyses run(Loop &L, LoopAnalysisManager &AM,
                        LoopStandardAnalysisResults &AR, LPMUpdater &U) {
    InsertTimer IT;
    if (IT.runOnLoop(&L))
      return PreservedAnalyses::none();
    else 
      return PreservedAnalyses::all();
  }
};

// Declare/define the variables or functions for a code
// like Printf(...), PrintProfilerValues(...), array to store clock values etc

class InsertStr {
  uint64_t g = 0;
  public:
  bool runOnModule(Module &M);
};

class InsertStrPass : public PassInfoMixin<InsertStrPass> {
  public:
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &MM) {
    InsertStr IS;
    if (IS.runOnModule(M))
      return PreservedAnalyses::none();
    else
      return PreservedAnalyses::all();
  }
};


struct InsertStrLegacy : public ModulePass {
  static char ID;
  uint64_t g = 0;
  InsertStrLegacy() : ModulePass(ID) {
    initializeInsertStrLegacyPass(*PassRegistry::getPassRegistry());
  }
  bool runOnModule(Module &M) override;
};

} // llvm namespace
#endif
