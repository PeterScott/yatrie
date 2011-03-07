Yet Another Trie
==========

This is a project to create a cache-fill-efficient and memory-efficient trie
data structure, mapping machine words to other machine words. It aims to produce
a search data structure that is reliably efficient in every way people care
about, across a wide variety of workloads.

Design notes
----------

The four low bits of every pointer returned by `malloc()` are all zero. We can
take advantage of this fact to use them as a type tag.
