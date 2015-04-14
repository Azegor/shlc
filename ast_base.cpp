/*
 * Copyright 2014 Azegor
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "ast_base.h"

#include <typeinfo>

#include "ast_statements.h"

void printIndent(int indent)
{
  static constexpr int width = 2;
  for (int i = 0; i < indent * width; ++i)
    std::cout << ' ';
}

// -----------------------------------------------------------------------------

void BlockStmt::print(int indent)
{
  printIndent(indent);
  std::cout << '{' << std::endl;
  for (auto &stmt : block)
  {
    if (stmt) stmt->print(indent + 1);
  }
  printIndent(indent);
  std::cout << '}' << std::endl;
}

llvm::Value *BlockStmt::codegen(Context &ctx)
{
  for (auto &&stmt : block)
  {
    stmt->codegen(ctx);
  }
  return nullptr;
}

Statement::CodeFlowReturn BlockStmt::codeFlowReturn() const
{
  bool first = true;
  Statement::CodeFlowReturn res = Statement::CodeFlowReturn::Always;
  for (auto &&stmt : block)
  {
    // rest of block is skipped with these statements
    // -> following return meaningless
    // only works if there wasn't a possible return before
    //     if (dynamic_cast<ContinueStmt *>(stmt.get()) ||
    //         dynamic_cast<BreakStmt *>(stmt.get()))
    if (typeid(*stmt) == typeid(ContinueStmt) ||
        typeid(stmt) == typeid(BreakStmt))
    {
      return res; // can never reach a return, but might return before
    }
    //     if (dynamic_cast<ReturnStmt *>(stmt.get())) {
    //       // whatever happens before, after this stmt CF cannot continue
    //       return Statement::CodeFlowReturn::Never;
    //     }

    // check other statements
    auto cfr = stmt->codeFlowReturn();
    if (cfr == Statement::CodeFlowReturn::Never)
      return Statement::CodeFlowReturn::Never;
    res = Statement::combineCFR(res, cfr);
  }
  return res;
}
