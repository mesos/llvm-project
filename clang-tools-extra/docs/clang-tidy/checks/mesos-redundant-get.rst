.. title:: clang-tidy - mesos-redundant-get

mesos-redundant-get
===================

Find and remove redundant calls to ``.get()`` on stout and libprocess wrapper
types. This check currently handles ``Future``, ``Option``, ``Try``, and
``Result``.

Examples:

.. code-block:: c++

  Option<vector<int>> option;

  option.get().empty() ==> option->empty()
