##### Week 1: Data Types and parsing
+ ~~Fixnums, Chars, Bools, Strings~~
+ ~~Pairs~~
+ ~~Consing of pairs into lists~~
+ ~~Parser (handwritten)~~
+ ~~symbol table (linked list or hash table)~~ (TODO allow for duplicates rep as
separate symbols)
+ ~~quote operator (makes scum a real interpreter)~~

##### Week 2: Env and vars
+ ~~Global environment and state~~
+ ~~local variables (not garbage collected yet)~~
+ ~~define and set operators for above~~

##### Week 3: Control
+ If and cond
+ procedures, named and anonymous
+ apply and eval (builds heavily on procedures)

##### Week 4: Finishing touches
+ Standard library (trivial after implementing procedures)
+ I/O (easy bc of C backend)
+ mark and sweep (a crappy version, literally store references to non let bound malloced objects in a list, free when full)
