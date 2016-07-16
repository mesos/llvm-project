// RUN: %check_clang_tidy %s mesos-this-capture %t

namespace process {
template <typename>
struct Future {
  template <typename F> Future onAny(F) { return {}; };
  template <typename F> Future then(F) { return {}; };
};

template <typename F>
Future<void> defer(int pid, F) { return {}; }

} // namespace process  {

using process::Future;
using process::defer;

struct S {
  Future<void> future() const { return {}; }

  // TODO(bbannier): Rework matcher to check all branches of a chained Future.

  void f() {
    future()
        // CHECK-MESSAGES: :[[@LINE+1]]:15: warning: callback capturing this should be dispatched/deferred to a specific PID [mesos-this-capture]
        .then([this]() { (void)this; });
    future()
        // CHECK-MESSAGES: :[[@LINE+1]]:16: warning: callback capturing this should be dispatched/deferred to a specific PID [mesos-this-capture]
        .onAny([this]() { (void)this; });
    }

    void g() {
      future()
          // CHECK-MESSAGES: :[[@LINE+1]]:17: warning: callback capturing this should be dispatched/deferred to a specific PID [mesos-this-capture]
          .then([=]() { (void)this; });
      future()
          // CHECK-MESSAGES: :[[@LINE+1]]:18: warning: callback capturing this should be dispatched/deferred to a specific PID [mesos-this-capture]
          .onAny([=]() { (void)this->i; });
    }

    // TODO(bbannier): Check referenced lambda.
    // void h() {
    //   auto l = [=]() { (void)this; };
    //   // DISABLEDCHECK-MESSAGES: :[[@LINE+1]]:21: warning: callback capturing this should be dispatched/deferred to a specific PID [mesos-this-capture]
    //   future().then(l);
    // }

    int i = 0;
};

// Negatives.
void f() {
  Future<void>().onAny([]() {});
  Future<void>().then([]() {});
};

struct K {
    void f() {
      Future<void>()
          .then(defer(0, [=]() { (void)this; }));
    }
};
