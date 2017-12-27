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
  const auto futureCallbackName = anyOf(
      hasName("after"),
      hasName("onAny"),
      hasName("onDiscard"),
      hasName("onDiscarded"),
      hasName("onFailed"),
      hasName("onReady"),
      hasName("repair"),
      hasName("then"));

  // A matcher for a this-capturing lambda.
  const auto lambda = has(lambdaExpr(capturesThis()).bind("lambda"));

  // Depending on whether the lambda captures additional variables with
  // non-POD, non-trivial types, an additional `CXXBindTemporaryExpr` might be
  // emitted.
  const auto lambdaCapturingThis =
      anyOf(lambda, has(cxxBindTemporaryExpr(lambda)));

  // If the lambda is passed in and not directly defined as an argument a
  // `DeclRefExpr` is emitted and we need to check to referenced variable.
  //
  // The variable will be constructed from some lambda expr, e.g.,
  //
  //     auto l = []() {};
  //
  // This sort of construction of class-type objects always emits an
  // `ExprWithCleanups` with a `CXXConstructExpr` from a
  // `MaterializeTemporaryExpr`.
  const auto lambdaCapturingThisRef =
      declRefExpr(
          hasDeclaration(varDecl(has(exprWithCleanups(has(cxxConstructExpr(
              has(materializeTemporaryExpr(lambdaCapturingThis)))))))))
          .bind("ref");

  // Register a matcher for this-capturing lambdas used directly to the `Future`
  // callbacks. This matcher requires such a lambda or reference to such a
  // lambda as arguments and e.g., does not match lambdas wrapped in `defer`
  // invocations.
  Finder->addMatcher(
      cxxMemberCallExpr(
          hasDeclaration(namedDecl(futureCallbackName)),
          on(hasType(cxxRecordDecl(hasName("Future")))),
          hasAnyArgument(anyOf(lambdaCapturingThis, lambdaCapturingThisRef))),
      this);

  // Matcher for `process::loop`. This function has two overloads, a
  // two-argument version taking just lambdas and a three-argument one also
  // taking a PID to dispatch to. We only check the two-argument version and
  // assume that the version taking a PID is internally sound.
  Finder->addMatcher(
      callExpr(
          argumentCountIs(2), callee(namedDecl(hasName("process::loop"))),
          hasAnyArgument(anyOf(materializeTemporaryExpr(lambdaCapturingThis),
                               lambdaCapturingThisRef))),
      this);
}

void ThisCaptureCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *ref = Result.Nodes.getNodeAs<Expr>("ref");
  const auto *lambda = Result.Nodes.getNodeAs<Expr>("lambda");

  diag(ref ? ref->getBeginLoc() : lambda->getBeginLoc(),
       "callback capturing this should be "
       "dispatched/deferred to a specific PID");

  // If the lambda was not declared at the site of the use add a note
  // at its declaration.
  if (ref && ref->getExprLoc() != lambda->getExprLoc()) {
    diag(lambda->getExprLoc(), "declared here", DiagnosticIDs::Note);
  }
}

} // namespace mesos
} // namespace tidy
} // namespace clang
