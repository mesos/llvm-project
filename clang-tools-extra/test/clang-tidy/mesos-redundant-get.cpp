// RUN: %check_clang_tidy %s mesos-redundant-get %t

struct Foo {
  void bar();
  int baz;
  void quz() const;
};

namespace process {
template <typename T>
struct Future {
  T *operator->();
  T &get();

  const T *operator->() const;
  const T &get() const;
        };
}

template <typename T>
using Option = process::Future<T>;

template <typename T>
using Try = process::Future<T>;

// Positives.
void p1(process::Future<Foo> p) {

  // CHECK-MESSAGES: :[[@LINE+2]]:5: warning: use of redundant 'get' [mesos-redundant-get]
  // CHECK-FIXES: p->bar();
  p.get().bar();

  // CHECK-MESSAGES: :[[@LINE+2]]:11: warning: use of redundant 'get' [mesos-redundant-get]
  // CHECK-FIXES: p->quz();
  (void)p.get().quz();

  // CHECK-MESSAGES: :[[@LINE+2]]:11: warning: use of redundant 'get' [mesos-redundant-get]
  // CHECK-FIXES: p->baz;
  (void)p.get().baz;
}

void p2(const process::Future<Foo> p) {
  // CHECK-MESSAGES: :[[@LINE+2]]:11: warning: use of redundant 'get' [mesos-redundant-get]
  // CHECK-FIXES: p->quz();
  (void)p.get().quz();

  // CHECK-MESSAGES: :[[@LINE+2]]:11: warning: use of redundant 'get' [mesos-redundant-get]
  // CHECK-FIXES: p->baz;
  (void)p.get().baz;
}

#define EXPECT_NONE(option) option.baz;

void p3(process::Future<Option<Foo>> p) {
  // CHECK-MESSAGES: :[[@LINE+2]]:17: warning: use of redundant 'get' [mesos-redundant-get]
  // CHECK-FIXES: EXPECT_NONE(p->get());
  EXPECT_NONE(p.get().get());
}

using TryFoo = Try<Foo>;
void p4(process::Future<TryFoo> p) {
  // CHECK-MESSAGES: :[[@LINE+2]]:17: warning: use of redundant 'get' [mesos-redundant-get]
  // CHECK-FIXES: EXPECT_NONE(p->get());
  EXPECT_NONE(p.get().get());
}

// Negatives.
void n1(process::Future<Foo> n) {
  n.get();
  n->bar();
  n->quz();
  (void)n->baz;
}

void n2(const process::Future<Foo> n) {
  n.get();
  n->quz();
  (void)n->baz;
}

void n3(process::Future<Foo *> n) {
  n.get()->bar();
  n.get()->quz();
  (void)n.get()->baz;
}

template <typename T>
struct Other {
  T get();
};

void n4(Other<Foo> n) {
  (void)n.get().bar();
  (void)n.get().quz();
  (void)n.get().baz;
}

struct Conv {
  operator int() const { return {}; }
};

void n5(process::Future<Conv> n) {
  int ni = n.get();
}

#define EXPECT_NONE(option) option.get();

void n6(process::Future<Option<int>> n) {
  EXPECT_NONE(n.get());
}

void n7() {
  // A future not defined in the `process` namespace.
  struct Future {
    struct Int {
      int value() const { return {}; }
    };
    const Int &get() const { return {}; }
  } n;

  int ni = n.get().value();
}
