#ifndef EXPENSEMANAGER_H
#define EXPENSEMANAGER_H

#include "User.h"
#include "Split.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <utility> // Required for std::pair

// Enum to specify types of split behavior.
enum ExpenseType
{
    EQUAL,
    EXACT,
    PERCENT
};

// --- OOPS: DESIGN PATTERNS & ENCAPSULATION ---
class ExpenseManager
{
private:
    // Encapsulation: Internal data structures (user database and balance registry) are private 
    // to prevent direct modification from outside.
    std::unordered_map<std::string, User *> userMap;
    std::unordered_map<std::string, std::unordered_map<std::string, double>> balanceSheet;

public:
    void addUser(User *user);
    
    // Polymorphic Collection: The splits vector contains pointers to the base class `Split*`.
    // However, at runtime, it contains instances of subclass objects (EqualSplit, ExactSplit, PercentSplit).
    void addExpense(std::string payerId, double totalAmount, const std::vector<Split *> &splits);
    void simplifyDebts();
    void showBalances();

    // --- DESIGN PATTERN: FACTORY METHOD ---
    // Declared as `static` so it belongs to the class itself rather than instances.
    // It encapsulates the complex object creation logic for subclasses (Equal/Exact/Percent Split),
    // returning them as base pointers (Split*). This is a textbook implementation of the Factory Pattern.
    static std::vector<Split *> createSplits(double totalAmount,
                                             const std::vector<std::pair<std::string, double>> &data,
                                             ExpenseType type);
};

#endif