#include "xos.h"

namespace xos {
Compiler::Compiler() {
  TheContext = std::make_unique<LLVMContext>();
  TheModule = std::make_unique<Module>("my cool jit", *TheContext);
  Builder = std::make_unique<IRBuilder<>>(*TheContext);
}

Value *Compiler::LogErrorV(const char *Str) {
  printf("Error: %s\n", Str);
  return nullptr;
}

Value *Compiler::StringExprAST(const ast::Str &str) {
  return Builder->CreateGlobalStringPtr(str.getStr());
}

Value *Compiler::OutExprAST(const ast::Out &out) {
  /* Dispatch call to printf. We can do this by creating another function called
   * printf, similar to how we created other functions in here. Then call it
   * with out string arg. This involves two things:
   *    1. Create another function.
   *    2. Explicit call to printf.
   *
   * */
  FunctionType *pfFT =
      FunctionType::get(Type::getInt32Ty(*TheContext),
                        {Type::getInt8Ty(*TheContext)->getPointerTo()}, false);
  Function *pf = Function::Create(pfFT, Function::ExternalLinkage, "printf",
                                  TheModule.get());
  return Builder->CreateCall(pf, {StringExprAST(out.getExpr())});
}

Function *Compiler::PrototypeAST(const ast::Prototype &proto) {
  // Make the function type:  double(double,double) etc.
  FunctionType *FT = FunctionType::get(Type::getVoidTy(*TheContext), {}, false);

  Function *F = Function::Create(FT, Function::ExternalLinkage, proto.getName(),
                                 TheModule.get());

  return F;
}

Function *Compiler::FunctionAST(const ast::Func &func) {
  // First, check for an existing function from a previous 'extern' declaration.
  Function *TheFunction = TheModule->getFunction(func.getProto().getName());

  if (!TheFunction) TheFunction = PrototypeAST(func.getProto());

  if (!TheFunction) return nullptr;

  // Create a new basic block to start insertion into.
  BasicBlock *BB = BasicBlock::Create(*TheContext, "entry", TheFunction);
  Builder->SetInsertPoint(BB);

  // Record the function arguments in the NamedValues map.
  NamedValues.clear();
  for (auto &Arg : TheFunction->args())
    NamedValues[std::string(Arg.getName())] = &Arg;

  if ([[maybe_unused]] Value *RetVal = OutExprAST(func.getBody())) {
    // Finish off the function.
    Builder->CreateRetVoid();

    // Validate the generated code, checking for consistency.
    verifyFunction(*TheFunction);

    return TheFunction;
  }

  // Error reading body, remove function.
  TheFunction->eraseFromParent();
  return nullptr;
}

void Compiler::compile(const ast::Func &funcAST) { FunctionAST(funcAST); }

}  // end namespace xos
