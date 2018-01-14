//===--- RedundantGetCheck.cpp - clang-tidy--------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "RedundantGetCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace mesos {

void RedundantGetCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(
      memberExpr(
          has(ignoringImpCasts(
              cxxMemberCallExpr(
                  callee(cxxMethodDecl(hasName("get"))),
                  on(expr(hasType(cxxRecordDecl(anyOf(
                              hasName("::process::Future"), hasName("::Option"),
                              hasName("::Try"), hasName("::Result")))))
                         .bind("wrapper")))
                  .bind("get"))),
          unless(anyOf(isArrow(), hasDeclaration(cxxConversionDecl()))))
          .bind("member"),
      this);
}

void RedundantGetCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *get = Result.Nodes.getNodeAs<CXXMemberCallExpr>("get");
  const auto *wrapper = Result.Nodes.getNodeAs<Expr>("wrapper");
  const auto *member = Result.Nodes.getNodeAs<MemberExpr>("member");

  // Do not diagnose cases where the `get` and member accessed via its return
  // value do not have the same `FileID`. This e.g., suppresses cases where the
  // `get` was part of a macro argument, but the member access was in a macro
  // definition like in uses of `EXPECT_NONE` and similar macros.
  if (Result.SourceManager->getDecomposedLoc(get->getRParenLoc()).first !=
      Result.SourceManager->getDecomposedLoc(member->getExprLoc()).first)
    return;

  StringRef replacement = Lexer::getSourceText(
      CharSourceRange::getTokenRange(wrapper->getSourceRange()),
      *Result.SourceManager, getLangOpts());

  diag(get->getExprLoc(), "use of redundant 'get'")
      << FixItHint::CreateReplacement(member->getOperatorLoc(), "->")
      << FixItHint::CreateReplacement(get->getSourceRange(), replacement);
}

} // namespace mesos
} // namespace tidy
} // namespace clang
