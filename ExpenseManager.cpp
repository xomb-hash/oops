#include "ExpenseManager.h"
#include <iostream>
#include <queue>
#include <algorithm>

/*std::unordered_map<std::string, User *> userMap;
    std::unordered_map<std::string, std::unordered_map<std::string, double>> balanceSheet;*/
void ExpenseManager::addUser(User *user) { userMap[user->getId()] = user; }

// --- OOPS: SUBTYPE POLYMORPHISM ---
// This method accepts a collection of base pointers `Split*`. At runtime, the loop
// processes various subclass instances (EqualSplit, ExactSplit, PercentSplit) uniformly
// through their shared base class interface (the `amount` and `userId` fields).

void ExpenseManager::addExpense(std::string payerId, double totalAmount, const std::vector<Split *> &splits)
{
    for (auto *split : splits)
    {
        if (payerId != split->userId)
        {
            balanceSheet[payerId][split->userId] += split->amount;
            balanceSheet[split->userId][payerId] -= split->amount;
        }
    }
}

void ExpenseManager::showBalances()
{
    std::cout << "\n--- Current Balances ---" << std::endl;

    bool hasBalances = false;
    for (auto const &[fromUserId, balances] : balanceSheet)
    {
        for (auto const &[toUserId, amount] : balances)
        {
            if (amount > 0.01)
            {
                hasBalances = true;
                std::cout << userMap[toUserId]->getName() << " owes "
                          << userMap[fromUserId]->getName() << " : "
                          << amount << std::endl;
            }
        }
    }

    if (!hasBalances)
        std::cout << "No pending balances." << std::endl;
}

// --- DESIGN PATTERNS: FACTORY METHOD IMPLEMENTATION ---
// This is the concrete implementation of the factory. Clients call this method with a total amount,
// user data, and a type. The factory handles the creation of the specific derived classes (EqualSplit,
// ExactSplit, PercentSplit) depending on the enum type.
// This decouples client code (main.cpp) from the instantiation details of subclass types.
std::vector<Split *> ExpenseManager::createSplits(double totalAmount,
                                                  const std::vector<std::pair<std::string, double>> &data,
                                                  ExpenseType type)
{
    std::vector<Split *> splits;
    for (auto const &[id, value] : data)
    {
        if (type == EQUAL)
        {
            // Instantiates EqualSplit and returns it upcast to Split*
            splits.push_back(new EqualSplit(id, totalAmount / data.size()));
        }
        else if (type == EXACT)
        {
            // Instantiates ExactSplit and returns it upcast to Split*
            splits.push_back(new ExactSplit(id, value));
        }
        else if (type == PERCENT)
        {
            // Instantiates PercentSplit (passing percent & calculated amount) and returns it upcast to Split*
            splits.push_back(new PercentSplit(id, value, (value * totalAmount) / 100.0));
        }
    }
    return splits;
}

// --- OOPS: ALGORITHM & CLASS BEHAVIOR ---
// simplifyDebts encapsulates the greedy algorithm for debt simplification.
// It utilizes private member structures (`balanceSheet`, `userMap`) to compute net balances
// and print the simplified transactions, hiding complexity from the caller (Abstraction).
void ExpenseManager::simplifyDebts()
{
    std::unordered_map<std::string, double> netBalances;
    for (auto const &p : balanceSheet)
        for (auto const &s : p.second)
            netBalances[p.first] += s.second;

    std::priority_queue<std::pair<double, std::string>> creditors, debtors;
    for (auto const &[id, bal] : netBalances)
    {
        if (bal > 0.01)
            creditors.push({bal, id});
        else if (bal < -0.01)
            debtors.push({-bal, id});
    }

    std::cout << "\n--- Settlement Plan ---" << std::endl;
    while (!creditors.empty() && !debtors.empty())
    {
        auto c = creditors.top();
        creditors.pop();
        auto d = debtors.top();
        debtors.pop();
        double amt = std::min(c.first, d.first);

        std::cout << userMap[d.second]->getName() << " pays "
                  << userMap[c.second]->getName() << " : " << amt << std::endl;

        if (c.first - amt > 0.01)
            creditors.push({c.first - amt, c.second});
        if (d.first - amt > 0.01)
            debtors.push({d.first - amt, d.second});
    }
}
