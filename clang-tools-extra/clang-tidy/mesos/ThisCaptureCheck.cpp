//===--- ThisCaptureCheck.cpp - clang-tidy---------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "ThisCaptureCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

#include "llvm/ADT/STLExtras.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace mesos {

AST_MATCHER(LambdaExpr, capturesThis) {
  return llvm::any_of(Node.captures(),
                      [](const LambdaCapture &c) { return c.capturesThis(); });
}

void ThisCaptureCheck::registerMatchers(MatchFinder *Finder) {
  const auto dispatcher = callExpr(
      hasDeclaration(namedDecl(anyOf(hasName("defer"), hasName("dispatch")))));

  const auto undeferredLambda =
      lambdaExpr(capturesThis(), unless(hasAncestor(dispatcher)));

  const auto futureCallbackName = anyOf(
      hasName("after"),
      hasName("onAny"),
      hasName("onDiscard"),
      hasName("onDiscarded"),
      hasName("onFailed"),
      hasName("onReady"),
      hasName("repair"),
      hasName("then"));

  Finder->addMatcher(
      cxxMemberCallExpr(hasDeclaration(namedDecl(futureCallbackName)),
                        on(hasType(cxxRecordDecl(hasName("Future")))),
                        hasDescendant(undeferredLambda.bind("lambda"))),
      this);
}

void ThisCaptureCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *lambda = Result.Nodes.getNodeAs<LambdaExpr>("lambda");

  if (not lambda)
    return;

  diag(lambda->getLocStart(), "callback capturing this should be "
                              "dispatched/deferred to a specific PID");
}

} // namespace mesos
} // namespace tidy
} // namespace clang
