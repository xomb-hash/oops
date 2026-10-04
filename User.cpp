#include "User.h"

// --- OOPS: CONSTRUCTOR & MEMBER INITIALIZATION LIST ---
// The constructor uses a Member Initializer List `: id(id), name(name)` to initialize
// private attributes directly. This is more efficient in C++ than using assignment
// statements inside the `{}` body, as it avoids temporary copy construction.
User::User(std::string id, std::string name) : id(id), name(name) {}
// function defination :: -> scope which telll that it bvelongs to this classs
