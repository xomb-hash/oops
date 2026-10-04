#ifndef SPLIT_H
#define SPLIT_H
#include <string>

// --- OOPS: INHERITANCE & POLYMORPHISM ---
// Base Class: Split is the parent (base) class containing attributes common to all types of splits.
class Split
{
public:
    std::string userId;
    double amount;
    
    // Base class constructor: Initializes attributes common to all splits.
    Split(std::string userId, double amt = 0.0) : userId(userId), amount(amt) {}

    // Polymorphism: A virtual destructor is crucial in base classes. 
    // It ensures that deleting a derived class instance via a base class pointer (e.g., Split*)
    // calls the derived class's destructor, preventing memory leaks and resource leaks.
    virtual ~Split() = default;
};

// Derived Class (Inheritance): EqualSplit inherits all public/protected features of Split.
class EqualSplit : public Split
{
public:
    // Constructor Delegation: Calls the base constructor Split(...) to initialize parent attributes.
    EqualSplit(std::string id, double amt) : Split(id, amt) {}
};

// Derived Class (Inheritance): ExactSplit inherits Split.
class ExactSplit : public Split
{
public:
    // Constructor Delegation: Passes parameters to base class constructor.
    ExactSplit(std::string id, double amt) : Split(id, amt) {}
};

// Derived Class (Inheritance): PercentSplit inherits Split and extends it with extra attributes.
class PercentSplit : public Split
{
public:
    double percent; // Subclass-specific attribute

    // Constructor Delegation: Initializes parent Split with the calculated amount, and its own percent attribute.
    PercentSplit(std::string id, double p, double amt = 0.0) : Split(id, amt), percent(p) {}
};
#endif