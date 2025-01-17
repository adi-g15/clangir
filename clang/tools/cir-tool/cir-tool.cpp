//===- cir-tool.cpp - CIR optimizationa and analysis driver -----*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// Similar to MLIR/LLVM's "opt" tools but also deals with analysis and custom
// arguments. TODO: this is basically a copy from MlirOptMain.cpp, but capable
// of module emission as specified by the user.
//
//===----------------------------------------------------------------------===//

#include "mlir/Dialect/Arithmetic/IR/Arithmetic.h"
#include "mlir/Dialect/CIR/IR/CIRDialect.h"
#include "mlir/Dialect/CIR/Passes.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/Dialect/LLVMIR/LLVMDialect.h"
#include "mlir/Dialect/MemRef/IR/MemRef.h"
#include "mlir/InitAllPasses.h"
#include "mlir/Pass/PassRegistry.h"
#include "mlir/Tools/mlir-opt/MlirOptMain.h"
#include "clang/CIR/Passes.h"

int main(int argc, char **argv) {
  // TODO: register needed MLIR passes for CIR?
  mlir::DialectRegistry registry;
  registry.insert<mlir::BuiltinDialect, mlir::arith::ArithmeticDialect,
                  mlir::cir::CIRDialect, mlir::memref::MemRefDialect,
                  mlir::LLVM::LLVMDialect>();

  ::mlir::registerPass([]() -> std::unique_ptr<::mlir::Pass> {
    return cir::createConvertCIRToFuncPass();
  });
  ::mlir::registerPass([]() -> std::unique_ptr<::mlir::Pass> {
    return cir::createConvertCIRToLLVMPass();
  });
  ::mlir::registerPass([]() -> std::unique_ptr<::mlir::Pass> {
    return cir::createConvertCIRToMemRefPass();
  });
  ::mlir::registerPass([]() -> std::unique_ptr<::mlir::Pass> {
    return mlir::createLifetimeCheckPass();
  });
  ::mlir::registerPass([]() -> std::unique_ptr<::mlir::Pass> {
    return mlir::createMergeCleanupsPass();
  });

  mlir::registerTransformsPasses();

  return failed(MlirOptMain(argc, argv,
                            "Clang IR analysis and optimization tool\n",
                            registry, /*preloadDialectsInContext=*/true));
}
