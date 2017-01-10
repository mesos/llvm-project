//===--- MesosTidyModule.cpp - clang-tidy ---------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "../ClangTidy.h"
#include "../ClangTidyModule.h"
#include "../ClangTidyModuleRegistry.h"

#include "FlagsInheritanceCheck.h"
#include "NamespaceCommentCheck.h"
#include "ThisCaptureCheck.h"

namespace clang {
namespace tidy {
namespace mesos {

class MesosModule : public ClangTidyModule {
public:
  void addCheckFactories(ClangTidyCheckFactories &CheckFactories) override {
    CheckFactories.registerCheck<FlagsInheritanceCheck>(
        "mesos-flags-inheritance");
    CheckFactories.registerCheck<NamespaceCommentCheck>(
        "mesos-namespace-comments");
    CheckFactories.registerCheck<ThisCaptureCheck>("mesos-this-capture");
  }
};

// Register the MesosModule using this statically initialized variable.
static ClangTidyModuleRegistry::Add<MesosModule> X("mesos-module",
                                                   "Adds Mesos lint checks.");

} // namespace mesos

// This anchor is used to force the linker to link in the generated object file
// and thus register the MesosModule.
volatile int MesosModuleAnchorSource = 0;

} // namespace tidy
} // namespace clang
