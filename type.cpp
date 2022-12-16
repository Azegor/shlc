#include "type.h"

#include <llvm/IR/DerivedTypes.h>
#include <map>
#include <unordered_map>

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/DIBuilder.h>

#include "ast_base.h"
#include "ast_types.h"
#include "context.h"

const std::string &getTypeName(BuiltinTypeKind t)
{
  static std::map<BuiltinTypeKind, std::string> names = {{BuiltinTypeKind::none, "none"}, // *
                                              //{BuiltinTypeKind::inferred, "inferred"},
                                              {BuiltinTypeKind::int_t, "int_t"},
                                              {BuiltinTypeKind::flt_t, "flt_t"},
                                              {BuiltinTypeKind::chr_t, "chr_t"},
                                              {BuiltinTypeKind::boo_t, "boo_t"},
                                              {BuiltinTypeKind::str_t, "str_t"},
                                              {BuiltinTypeKind::vac_t, "vac_t"}};
  return names[t];
}

const std::string &getMangleName(BuiltinTypeKind t)
{
  static std::map<BuiltinTypeKind, std::string> names = {{BuiltinTypeKind::int_t, "i"},
                                       {BuiltinTypeKind::flt_t, "f"},
                                       {BuiltinTypeKind::chr_t, "c"},
                                       {BuiltinTypeKind::boo_t, "b"},
                                       {BuiltinTypeKind::str_t, "s"},
                                       {BuiltinTypeKind::vac_t, "v"}};
  auto pos = names.find(t);
  if (pos == names.end())
    throw CodeGenError("invalid mangle name for type '" + getTypeName(t) +
                       '\'');
  return pos->second;
}

std::string getMangleName(Type *t)
{
  if (auto *structType = dynamic_cast<StructureType*>(t)) {
    const auto &name = structType->getName();
    return std::to_string(name.length()) + name;
  } else {
      return getMangleName(t->getKind());
  }
}

std::string getMangleDestructorName(ClassType *t)
{
  return "_ZN" + getMangleName(t) + "D1Ev";
}

LLVMTypeRegistry::LLVMTypeRegistry(GlobalContext &gl_ctx)
  : gl_ctx(gl_ctx)
{
    voidPointerType = llvm::Type::getInt8PtrTy(gl_ctx.llvm_context); // is void in llvm
    refCounterPtrType = getRefCounterType()->llvm::Type::getPointerTo();
    destrType = llvm::FunctionType::get(getBuiltinType(BuiltinTypeKind::vac_t), {voidPointerType}, false);
    destrPtrType = llvm::PointerType::get(destrType, 0);
}

llvm::Type *LLVMTypeRegistry::getType(Type *t)
{
    switch(t->getKind()) {
      case BuiltinTypeKind::cls_t:
        return getClassType(static_cast<ClassType*>(t));
        break;
      case BuiltinTypeKind::opq_t:
        return getOpaqueType(static_cast<OpaqueType*>(t));
        break;
      default:
        return getBuiltinType(t->getKind());
        break;
    }
}

llvm::StructType *LLVMTypeRegistry::getClassType(ClassType *ct)
{
  if (!ct->llvmType) {
    createLLVMClassTypeAndDestructor(ct);
  }
  return ct->llvmType;
}

llvm::PointerType *LLVMTypeRegistry::getClassPtrType(ClassType *ct)
{
  if (!ct->llvmType) {
    createLLVMClassTypeAndDestructor(ct);
  }
  return ct->llvmPtrType;
}

llvm::Function *LLVMTypeRegistry::getClassDestructor(ClassType *ct)
{
  if (!ct->llvmType) {
    createLLVMClassTypeAndDestructor(ct);
  }
  return ct->destructorFn;
}

llvm::PointerType *LLVMTypeRegistry::getOpaqueType(OpaqueType *ot)
{
    auto pos = opaqueTypeMap.find(ot);
    if (pos != opaqueTypeMap.end()) {
        return pos->second;
    }
    auto type = createLLVMOpaqueType(ot);
    opaqueTypeMap.emplace(ot, type);
    return type;
}

llvm::Type *LLVMTypeRegistry::getBuiltinType(BuiltinTypeKind tk) const
{
  switch (tk)
  {
    // int_t, flt_t, chr_t, boo_t, str_t, vac_t
    case BuiltinTypeKind::vac_t:
      return llvm::Type::getVoidTy(gl_ctx.llvm_context);
    case BuiltinTypeKind::int_t:
      return llvm::Type::getInt64Ty(gl_ctx.llvm_context);
    case BuiltinTypeKind::flt_t:
      return llvm::Type::getDoubleTy(gl_ctx.llvm_context);
    case BuiltinTypeKind::boo_t:
      return llvm::Type::getInt1Ty(gl_ctx.llvm_context);
    case BuiltinTypeKind::chr_t:
      return llvm::Type::getInt8Ty(gl_ctx.llvm_context); // no unicode
    case BuiltinTypeKind::str_t:
      return llvm::Type::getInt8PtrTy(gl_ctx.llvm_context);
    default:
      throw CodeGenError("Unknown type id " + getTypeName(tk));
  }
}

llvm::StructType *LLVMTypeRegistry::createLLVMClassType(ClassType *ct)
{
    std::vector<llvm::Type*> members;
    members.reserve(ct->fields.size() + 1);
    members.push_back(getRefCounterType());
    for (auto &field : ct->fields) {
        members.push_back(getType(field.type));
    }
    return llvm::StructType::create(gl_ctx.llvm_context, members, ct->getName());
}

llvm::Function *LLVMTypeRegistry::createLLVMClassDestructor(ClassType *ct)
{
  std::vector<ClassField*> classMembers;
  for (auto &f : ct->fields) {
    if (f.type->getKind() == BuiltinTypeKind::cls_t) {
      classMembers.push_back(&f);
    }
  }
  if (classMembers.empty()) {
    return nullptr; // no destructor needed
  }

  auto &builder = gl_ctx.builder;
  auto fn = llvm::Function::Create(
    llvm::FunctionType::get(
        getBuiltinType(BuiltinTypeKind::vac_t),
        {ct->llvmType}, // has to already be avaliable!
        false),
    llvm::Function::LinkOnceODRLinkage,
    getMangleDestructorName(ct),
    gl_ctx.module.get()
  );
  auto oldInsertBlock = builder.GetInsertBlock();
  auto oldInsertPoint = builder.GetInsertPoint();


  Context ctx(gl_ctx);
  ctx.currentFn = fn;

  // create body
  llvm::BasicBlock *entryBB =
      llvm::BasicBlock::Create(gl_ctx.llvm_context, "entry", fn);
  builder.SetInsertPoint(entryBB);

  auto zeroIdx = llvm::ConstantInt::get(gl_ctx.llvm_context, llvm::APInt(32, 0, true));
  auto arg1 = &*fn->arg_begin();
  for (auto *f : classMembers) {
    auto fieldIdx = llvm::ConstantInt::get(gl_ctx.llvm_context, llvm::APInt(32, f->index, true));
    auto classField = gl_ctx.builder.CreateInBoundsGEP(f->llvmType, arg1, {zeroIdx, fieldIdx}, f->name + "_fieldptr");
    makeXDecRefCall(ctx, classField, gl_ctx.llvmTypeRegistry.getClassDestructor(static_cast<ClassType*>(f->type)));
  }

  builder.CreateRetVoid();

  // reset insert point to current function (if it exists)
  if (oldInsertBlock) {
    builder.SetInsertPoint(oldInsertBlock, oldInsertPoint);
  }
  return fn;
}

void LLVMTypeRegistry::createLLVMClassTypeAndDestructor(ClassType *ct)
{
  ct->llvmType = createLLVMClassType(ct);
  ct->llvmPtrType = llvm::PointerType::get(ct->llvmType, 0);
  ct->destructorFn = createLLVMClassDestructor(ct);
}

llvm::PointerType *LLVMTypeRegistry::createLLVMOpaqueType(OpaqueType *ot) {
  auto structType = llvm::StructType::create(gl_ctx.llvm_context, ot->getName());
  return llvm::PointerType::get(structType, 0);
}

llvm::DIType *LLVMTypeRegistry::getDIType(Type *t)
{
  switch(t->getKind()) {
    case BuiltinTypeKind::cls_t:
      return getDIClassType(static_cast<ClassType*>(t));
      break;
    case BuiltinTypeKind::opq_t:
      return getDIOpaqueType(static_cast<OpaqueType*>(t));
      break;
    default:
      return getDIBuiltinType(t->getKind());
      break;
  }
}
llvm::DIType *LLVMTypeRegistry::getDIBuiltinType(BuiltinTypeKind tk)
{
  if (!diTypesInitialized) {
    createDIBuiltinTypes();
  }
  return diBuiltinTypes[(int)tk];
}

llvm::DIType *LLVMTypeRegistry::getDIClassType(ClassType *ct) {
  auto pos = diClassTypes.find(ct);
  if (pos != diClassTypes.end()) {
    return pos->second;
  }
  auto clsType = createDIClassType(ct);
  diClassTypes.insert({ct, clsType});
  return clsType;
}

llvm::DIType *LLVMTypeRegistry::getDIOpaqueType(OpaqueType *ot) {
  auto pos = diOpaqueTypes.find(ot);
  if (pos != diOpaqueTypes.end()) {
    return pos->second;
  }
  auto opaqueType = createDIOpaqueType(ot);
  diOpaqueTypes.insert({ot, opaqueType});
  return opaqueType;
}

void LLVMTypeRegistry::createDIBuiltinTypes()
{
    diBuiltinTypes[(int)BuiltinTypeKind::vac_t] = nullptr;
    diBuiltinTypes[(int)BuiltinTypeKind::int_t] = gl_ctx.diBuilder.createBasicType("int", 64, llvm::dwarf::DW_ATE_signed);
    diBuiltinTypes[(int)BuiltinTypeKind::flt_t] = gl_ctx.diBuilder.createBasicType("flt", 64, llvm::dwarf::DW_ATE_float);
    diBuiltinTypes[(int)BuiltinTypeKind::boo_t] = gl_ctx.diBuilder.createBasicType("boo", 8, llvm::dwarf::DW_ATE_boolean);
    diBuiltinTypes[(int)BuiltinTypeKind::chr_t] = gl_ctx.diBuilder.createBasicType("chr", 8, llvm::dwarf::DW_ATE_signed_char);
    diBuiltinTypes[(int)BuiltinTypeKind::str_t] = gl_ctx.diBuilder.createPointerType(diBuiltinTypes[(int)BuiltinTypeKind::chr_t], 64, 1, {}, "str");
    diTypesInitialized = true;
}

llvm::DIType *LLVMTypeRegistry::createDIClassType(ClassType *ct)
{
  std::vector<llvm::Metadata*> members;
  members.reserve(ct->fields.size()/* + 1*/);
  // members.push_back(getDIBuiltinType(BuiltinTypeKind::int_t)); // refcount
  for (auto &field : ct->fields) {
      members.push_back(getDIType(field.type));
  }
  auto llvmType = getClassType(ct);
  auto size = gl_ctx.module->getDataLayout().getTypeAllocSize(llvmType) - 8;
  auto align = gl_ctx.module->getDataLayout().getABITypeAlignment(llvmType); // TODO
  return gl_ctx.diBuilder.createClassType(gl_ctx.getCurrentDILexicalScope(), ct->name, gl_ctx.allDIFiles[ct->srcLoc.lexerNr], ct->srcLoc.startToken.line,
    size, align, 8, llvm::DINode::FlagZero, nullptr, gl_ctx.diBuilder.getOrCreateArray(members));
}

llvm::DIType *LLVMTypeRegistry::createDIOpaqueType(OpaqueType *ot)
{
  return gl_ctx.diBuilder.createForwardDecl(llvm::dwarf::DW_TAG_structure_type, ot->name, gl_ctx.getCurrentDILexicalScope(),
                                            gl_ctx.allDIFiles[ot->srcLoc.lexerNr], ot->srcLoc.startToken.line);
}
