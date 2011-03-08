Yet Another Trie
==========

This is a project to create a cache-fill-efficient and memory-efficient trie
data structure, mapping machine words to other machine words. It aims to produce
a search data structure that is reliably efficient in every way people care
about, across a wide variety of workloads.

Operations
---------

This is strictly a fun prototype thing, and not meant for production use. In
support of the just-for-fun goal, there are only four operations:new, insert,
get, and free.

    yatrie_t yatrie_new(void);

Creates a new, empty yatrie.

    yatrie_t yatrie_insert(yatrie_t yatrie, word_t key, word_t value);

Insert a key, value pair into a yatrie, and return its new value.

    word_t *yatrie_get(yatrie_t yatrie, word_t key);

Return a pointer to the value of the given key. You can modify this
pointer. Returns NULL if the key was not found.

    void yatrie_free(yatrie_t yatrie);

Free the yatrie.

Design notes
----------

The four low bits of every pointer returned by `malloc()` are all zero. We can
take advantage of this fact to use them as a type tag. This lets us define
several types of nodes. Some nodes contain one or more key-value pairs. Some
nodes contain one or more byte-pointer pairs. Some nodes contain an array of 256
tagged pointers, implementing an inner node in a 256-ary trie.

This way, we can get efficient performance in the common cases where some
branches of the tree are very sparse, which is important for memory efficiency
and reducing the number of cache line fills.

Compiling
--------

Just use the usual incantation:

    ./configure
    make

There is no `make install` step.
