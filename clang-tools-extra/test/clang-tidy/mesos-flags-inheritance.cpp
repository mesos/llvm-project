// RUN: %check_clang_tidy %s mesos-flags-inheritance %t

namespace flags {
struct FlagsBase {};
} // namespace flags

struct A : flags::FlagsBase {};
// CHECK-MESSAGES: :[[@LINE-1]]:12: warning: 'A' does not inherit virtually from 'FlagsBase' [mesos-flags-inheritance]
// CHECK-FIXES: {{^}}struct A : virtual flags::FlagsBase {};{{$}}

struct B : A {};
// CHECK-MESSAGES: :[[@LINE-1]]:12: warning: 'B' does not inherit virtually from 'A' which inherits from 'FlagsBase' [mesos-flags-inheritance]
// CHECK-FIXES: {{^}}struct B : virtual A {};{{$}}

class C : public flags::FlagsBase {};
// CHECK-MESSAGES: :[[@LINE-1]]:18: warning: 'C' does not inherit virtually from 'FlagsBase' [mesos-flags-inheritance]
// CHECK-FIXES: {{^}}class C : public virtual flags::FlagsBase {};{{$}}

struct D : virtual flags::FlagsBase {};
struct E : virtual C {};

struct FlagsBase {};
struct F : FlagsBase {};

namespace ns {
struct FlagsBase {};
} // namespace ns

struct G : ns::FlagsBase {};
