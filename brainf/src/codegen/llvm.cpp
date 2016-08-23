#include "generator.hpp"
#include "llvm/IR/TypeBuilder.h"
#include <iostream>

using namespace llvm;

namespace EsoFarm {
namespace BrainF {

template<typename Fn, GlobalValue::LinkageTypes Link>
static Function *makeFunction(LLVMContext &c,
                              Module *m,
                              IRBuilder<> &b,
                              std::string name,
                              std::vector<std::string> arg_names) {
  auto *fn_t = TypeBuilder<Fn, false>::get(c);
  Function *fn = Function::Create(fn_t, Link, name, m);
  auto i = 0;
  for (auto &arg : fn->args()) {
    arg.setName(arg_names[i++]);
  }
  return fn;
}

LLVMGenerator::LLVMGenerator(const std::string &name) : ctx{}, builder{ctx}, module{new Module{name, ctx}} {
  i8Ty = builder.getInt8Ty();
  i32Ty = builder.getInt32Ty();
  i64Ty = builder.getInt64Ty();
  boolTy = builder.getInt1Ty();
  voidTy = builder.getVoidTy();
  i8PtrTy = builder.getInt8PtrTy();

  read = cast<Function>(module->getOrInsertFunction("getchar", i32Ty, nullptr));
  print = cast<Function>(module->getOrInsertFunction("putchar", i32Ty, i32Ty, nullptr));
  memset = Intrinsic::getDeclaration(module.get(), Intrinsic::memset, {i8PtrTy, i64Ty});
  auto puts = cast<Function>(module->getOrInsertFunction("puts", i32Ty, i8PtrTy, nullptr));

  if (debugMode) {
    // Create Function `void DMSG(i8 *message)'
    dmsg = makeFunction<void(int8_t *), Function::InternalLinkage>
        (ctx, module.get(), builder, "DMSG", {"message"});
    auto *dmsg_bb = BasicBlock::Create(ctx, "", dmsg);
    builder.SetInsertPoint(dmsg_bb);
    Value *msg = &*dmsg->arg_begin();
    builder.CreateCall(puts, msg);
    builder.CreateRetVoid();

    // Create Function `void DST(i8 unit, i32 ptr)'
    auto *printf_t = TypeBuilder<int(char *, ...), false>::get(ctx);
    auto *printf = cast<Function>(module->getOrInsertFunction("printf", printf_t));
    dst = makeFunction<void(int8_t, int32_t), Function::InternalLinkage>
        (ctx, module.get(), builder, "DST", {"uval", "pval"});
    auto argi = dst->arg_begin();
    Value *f_unit = &*argi;
    argi++;
    Value *f_ptr = &*argi;
    auto dst_bb = BasicBlock::Create(ctx, "", dst);
    builder.SetInsertPoint(dst_bb);
    auto *fmt = builder.CreateGlobalStringPtr("ST P = %d U = %d\n");
    builder.CreateCall(printf, {fmt, f_ptr, f_unit});
    builder.CreateRetVoid();
  }

  // Create Function `i32 BrainF(i8 *units)'
  std::vector<Type *> brainf_args{i8PtrTy};
  auto bf_t = FunctionType::get(i32Ty, brainf_args, false);
  auto bf = Function::Create(bf_t, Function::ExternalLinkage, "BrainF", module.get());
  for (auto &arg : bf->args()) {
    arg.setName("units");
  }

  // Create Function `i32 main(i32 argc, i8** argv)'
  std::vector<Type *> main_args{i32Ty, i8PtrTy->getPointerTo()};
  auto main_t = FunctionType::get(i32Ty, main_args, false);
  auto main = Function::Create(main_t, Function::ExternalLinkage, "main", module.get());
  auto i = 0;
  std::string main_arg_name[] = {"argc", "argv"};
  for (auto &arg : main->args()) {
    arg.setName(main_arg_name[i++]);
  }
  auto main_bs = BasicBlock::Create(ctx, "", main);
  builder.SetInsertPoint(main_bs);
  // units = malloc(30000);
  auto alloc = CallInst::CreateMalloc(main_bs, i64Ty, i8Ty, builder.getInt64(0), builder.getInt64(30000));
  alloc->setName("units");
  main_bs->getInstList().push_back(alloc);
  // memset(units, 0, 30000);
  auto init = builder.CreateCall
      (memset, {alloc, builder.getInt8(0), builder.getInt64(30000), builder.getInt32(1), builder.getInt1(true)});
  // BrainF(units)
  auto callbf = builder.CreateCall(bf, {alloc});
  // free units;
  auto free = CallInst::CreateFree(alloc, main_bs);
  builder.GetInsertBlock()->getInstList().push_back(free);
  // return 0
  builder.CreateRet(builder.getInt32(0));

  // set insert point to BrainF
  auto bs = BasicBlock::Create(ctx, "", bf);
  builder.SetInsertPoint(bs);
}

Value *LLVMGenerator::on_arithmetic(Arithmetic *ast) {
  Value *result;
  char opr = '+';
  if (ast->opr.type == OperatorType::Increase) {
    result = builder.CreateAdd(getCurrentUnit(), builder.getInt8(int8_t(ast->opr.repeat)));
  } else {
    opr = '-';
    result = builder.CreateSub(getCurrentUnit(), builder.getInt8(int8_t(ast->opr.repeat)));
  }
  if (debugMode) {
    auto *dbg_msg = builder.CreateGlobalStringPtr(std::to_string(ast->opr.repeat) + opr);
    builder.CreateCall(dmsg, dbg_msg);
    builder.CreateCall(dst, {result, getPtr()});
  }
  builder.CreateStore(result, getCurrentUnitPtr());
  unitUpdated();
  return nullptr;
}

Value *LLVMGenerator::on_pointer(Pointer *ast) {
  Value *result;
  char opr = '>';
  if (ast->opr.type == OperatorType::MoveRight) {
    result = builder.CreateAdd(getPtr(), builder.getInt32(ast->opr.repeat));
  } else {
    opr = '<';
    result = builder.CreateSub(getPtr(), builder.getInt32(ast->opr.repeat));
  }
  if (debugMode) {
    auto *dbg_msg = builder.CreateGlobalStringPtr(std::to_string(ast->opr.repeat) + opr);
    builder.CreateCall(dmsg, dbg_msg);
    builder.CreateCall(dst, {getCurrentUnit(), getPtr()});
  }
  builder.CreateStore(result, ptr);
  pointerUpdated();
  return nullptr;
}

Value *LLVMGenerator::on_read(Read *ast) {
  if (debugMode) {
    auto *r = builder.CreateGlobalStringPtr("READ");
    builder.CreateCall(dmsg, r);
  }
  for (auto i = 1; i < ast->opr.repeat; ++i) {
    builder.CreateCall(read, None);
  }
  auto *result_i32 = builder.CreateCall(read, None);
  auto *result_i8 = builder.CreateIntCast(result_i32, i8Ty, true);
  builder.CreateStore(result_i8, getCurrentUnitPtr());
  unitUpdated();
  if (debugMode) {
    builder.CreateCall(dst, {result_i8, getPtr()});
  }
  return nullptr;
}

Value *LLVMGenerator::on_print(Print *ast) {
  if (debugMode) {
    auto *r = builder.CreateGlobalStringPtr("PRINT");
    builder.CreateCall(dmsg, r);
  }
  auto *uval_i32 = builder.CreateIntCast(getCurrentUnit(), i32Ty, true);
  for (auto i = 0; i < ast->opr.repeat; ++i) {
    builder.CreateCall(print, uval_i32);
  }
  return nullptr;
}

Value *LLVMGenerator::on_loop(Loop *ast) {
  auto *func = builder.GetInsertBlock()->getParent();
  const auto name_suffix = std::to_string(ast->opr.begin.offset) + "_" + std::to_string(ast->end.end.offset);
  auto *guard = BasicBlock::Create(ctx, "guard_" + name_suffix, func);
  auto *loop = BasicBlock::Create(ctx, "loop_" + name_suffix, func);
  auto *after = BasicBlock::Create(ctx, "afterloop_" + name_suffix, func);
  pointerUpdated();
  builder.CreateBr(guard);
  builder.SetInsertPoint(guard);
  if (debugMode) {
    builder.CreateCall(dmsg, builder.CreateGlobalStringPtr(": guard_" + name_suffix));
  }
  auto *cond = builder.CreateICmpNE(getCurrentUnit(), builder.getInt8(0));
  if (debugMode) {
    builder.CreateCall(dst, {getCurrentUnit(), getPtr()});
  }
  pointerUpdated();
  builder.CreateCondBr(cond, loop, after);
  builder.SetInsertPoint(loop);
  if (debugMode) {
    builder.CreateCall(dmsg, builder.CreateGlobalStringPtr(": loop_" + name_suffix));
  }
  for (const auto *sub : ast->block) {
    on_ast(sub);
  }
  pointerUpdated();
  builder.CreateBr(guard);
  builder.SetInsertPoint(after);
  if (debugMode) {
    builder.CreateCall(dmsg, builder.CreateGlobalStringPtr(": afterloop_" + name_suffix));
  }
  return nullptr;
}

Module *LLVMGenerator::codegen(const std::vector<AST *> &asts) {
  // get units
  units = &(*builder.GetInsertBlock()->getParent()->arg_begin());
  // alloc pointer
  ptr = builder.CreateAlloca(i32Ty, nullptr, "p");
  builder.CreateStore(builder.getInt32(0), ptr);
  // generate codes
  for (auto const *ast : asts) {
    std::clog << on_ast(ast) << std::endl;
  }
  builder.CreateRet(builder.getInt32(0));
  return module.get();
}

void LLVMGenerator::collect(value_t value) { }

void LLVMGenerator::unitUpdated() {
  uval_cache = nullptr;
}

void LLVMGenerator::pointerUpdated() {
  pval_cache = nullptr;
  uval_cache = nullptr;
  unit_cache = nullptr;
}

llvm::Value *LLVMGenerator::getPtr() {
  if (pval_cache == nullptr) {
    pval_cache = builder.CreateLoad(i32Ty, ptr);
  }
  return pval_cache;
}

llvm::Value *LLVMGenerator::getCurrentUnit() {
  if (uval_cache == nullptr) {
    uval_cache = builder.CreateLoad(i8Ty, getCurrentUnitPtr());
  }
  return uval_cache;
}

llvm::Value *LLVMGenerator::getCurrentUnitPtr() {
  if (unit_cache == nullptr) {
    unit_cache = builder.CreateInBoundsGEP(units, getPtr());
  }
  return unit_cache;
}

}
}