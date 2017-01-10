.. title:: clang-tidy - mesos-flags-inheritance

mesos-flags-inheritance
===========

Classes implementing flags-inheritance parsing by inheriting stout's `flags-inheritance::FlagBase`
class should always use `virtual` inheritance on any path leading to
`flags-inheritance::FlagsBase`. This is required since the `FlagsBase` part of the flags-inheritance
class contains a map of flag names to `Flag` instances; `virtual` inheritance
makes sure that only a single instance of the map is created so that all flags-inheritance
from base classes are set once and found correctly.
