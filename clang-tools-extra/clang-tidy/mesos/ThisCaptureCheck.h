//===--- ThisCaptureCheck.h - clang-tidy-------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_MESOS_THIS_CAPTURE_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_MESOS_THIS_CAPTURE_H

#include "../ClangTidy.h"

namespace clang {
namespace tidy {
namespace mesos {

/// FIXME: Write a short description.
///
/// For the user-facing documentation see:
/// http://clang.llvm.org/extra/clang-tidy/checks/mesos-this-capture.html
class ThisCaptureCheck : public ClangTidyCheck {
public:
  ThisCaptureCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}
  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;
};

} // namespace mesos
} // namespace tidy
} // namespace clang

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_MESOS_THIS_CAPTURE_H
