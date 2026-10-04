#include "ExpenseManager.h"
#include "Split.h"
#include <iostream>
#include <vector>

int main()
{
    // --- OOPS: CONSTRUCTOR INVOCATION ---
    // Instantiates three User objects. The compiler automatically resolves this to the User constructor.
    User u1("u1", "Rahul"), u2("u2", "Priya"), u3("u3", "Amit");
    double total = 1000.0;

    // --- SCENARIO 1: EQUAL SPLIT ---
    // --- OOPS: RAII (Resource Acquisition Is Initialization) & SCOPING ---
    // Enclosing the code in `{}` defines a local scope.
    // The `ExpenseManager manager` object is allocated on the stack. When execution leaves this scope,
    // the manager's destructor is automatically invoked, freeing its resources.
    {
        std::cout << "\n--- Scenario 1: Equal Split ---" << std::endl;
        ExpenseManager manager;
        manager.addUser(&u1);
        manager.addUser(&u2);
        manager.addUser(&u3);

        std::vector<std::pair<std::string, double>> equalData = {
            {"u1", 0}, {"u2", 0}, {"u3", 0}};

        // --- OOPS: FACTORY DESIGN PATTERN ---
        // Instead of calling `new EqualSplit(...)` directly, we delegate instantiation
        // to the static factory method `ExpenseManager::createSplits`.
        std::vector<Split *> s1 = ExpenseManager::createSplits(total, equalData, EQUAL); // explore

        manager.addExpense("u1", total, s1);

        // --- OOPS: ABSTRACTION ---
        // Client does not know how debts are simplified inside `simplifyDebts()` (it uses priority queues).
        // The implementation details are abstract.
        manager.simplifyDebts();

        // --- OOPS: MEMORY MANAGEMENT & POLYMORPHIC DELETION ---
        // Since `createSplits` allocated memory dynamically (on the heap), we must delete it.
        // Because of the virtual destructor in `Split`, calling `delete s` correctly invokes
        // the derived class `EqualSplit` destructor.
        for (auto *s : s1)
            delete s; // Cleanup
    }

    // --- SCENARIO 2: PERCENT SPLIT (Rahul 50%, Priya 30%, Amit 20%) ---
    {
        std::cout << "\n--- Scenario 2: Percent Split ---" << std::endl;
        ExpenseManager manager;
        manager.addUser(&u1);
        manager.addUser(&u2);
        manager.addUser(&u3);

        std::vector<std::pair<std::string, double>> percentData = {
            {"u1", 50.0}, {"u2", 30.0}, {"u3", 20.0}};

        // Factory Method instantiates PercentSplit objects behind a base class Split* pointer.
        std::vector<Split *> s2 = ExpenseManager::createSplits(total, percentData, PERCENT);

        manager.addExpense("u1", total, s2);
        manager.simplifyDebts();

        for (auto *s : s2)
            delete s; // Cleanup
    }

    // --- SCENARIO 3: EXACT SPLIT ---
    {
        std::cout << "\n--- Scenario 3: Exact Split ---" << std::endl;
        ExpenseManager manager;
        manager.addUser(&u1);
        manager.addUser(&u2);
        manager.addUser(&u3);

        std::vector<std::pair<std::string, double>> exactData = {
            {"u1", 1500.0}, {"u2", 1000.0}, {"u3", 500.0}};

        // Factory Method instantiates ExactSplit objects.
        std::vector<Split *> s3 = ExpenseManager::createSplits(total, exactData, EXACT);

        manager.addExpense("u1", total, s3);
        manager.simplifyDebts();

        for (auto *s : s3)
            delete s; // Cleanup
    }

    // --- SCENARIO 4: MULTI-TRANSACTION SIMPLIFICATION ---
    {
        std::cout << "\n--- Scenario 4: Multi-transaction Simplification ---" << std::endl;
        ExpenseManager manager;
        manager.addUser(&u1);
        manager.addUser(&u2);
        manager.addUser(&u3);

        // Expense 1: Rahul pays 3000 split equally
        std::vector<std::pair<std::string, double>> equalData1 = {
            {"u1", 0}, {"u2", 0}, {"u3", 0}};
        std::vector<Split *> s4_1 = ExpenseManager::createSplits(3000.0, equalData1, EQUAL);
        manager.addExpense("u1", 3000.0, s4_1);

        // Expense 2: Priya pays 1500 split equally
        std::vector<std::pair<std::string, double>> equalData2 = {
            {"u1", 0}, {"u2", 0}, {"u3", 0}};
        std::vector<Split *> s4_2 = ExpenseManager::createSplits(1500.0, equalData2, EQUAL);
        manager.addExpense("u2", 1500.0, s4_2);

        manager.simplifyDebts();

        for (auto *s : s4_1)
            delete s;
        for (auto *s : s4_2)
            delete s;
    }

    // --- SCENARIO 5: USER EXCLUDED FROM SPLIT ---// explore this
    {
        std::cout << "\n--- Scenario 5: User Excluded from Split ---" << std::endl;
        ExpenseManager manager;
        manager.addUser(&u1);
        manager.addUser(&u2);
        manager.addUser(&u3);

        // Rahul pays 1000, split equally only between Priya and Amit (Rahul is not in the split)
        std::vector<std::pair<std::string, double>> splitData = {// explore
                                                                 {"u2", 0},
                                                                 {"u3", 0}};
        std::vector<Split *> s5 = ExpenseManager::createSplits(1000.0, splitData, EQUAL);
        manager.addExpense("u1", 1000.0, s5);
        manager.simplifyDebts();

        for (auto *s : s5)
            delete s;
    }

    return 0;
}
