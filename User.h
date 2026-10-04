#ifndef USER_H
#define USER_H
#include <string>

// --- OOPS: ENCAPSULATION & ABSTRACTION ---
// Encapsulation: Grouping data variables (attributes) and methods (behaviors) together inside a class.
// We restrict direct access to data members by making them 'private' and exposing them via 'public' getters.
class User
{
private:
    // Encapsulation: Private member variables, hidden from external classes/functions to prevent unauthorized tampering.
    std::string id, name;

public:
    // Constructor: Initializes the object's state at creation time.
    User(std::string id, std::string name);

    // Abstraction: Public getters expose essential interface details to the outer world
    // while hiding the internal implementation details of how the data is stored or fetched.
    std::string getId()
    {
        return id;
    }
    std::string getName()
    {
        return name;
    }
};
#endif
