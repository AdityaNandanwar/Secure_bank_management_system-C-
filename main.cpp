#include <iostream>
#include <string>
#include <unordered_map>
#include <iomanip>
#include <limits>

// Function to hide passwords in the terminal (platform-specific)
#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>

char getch() {
    struct termios oldt, newt;
    char ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}
#endif

class Account {
private:
    std::string accountNumber;
    std::string password;
    double balance;

public:
    Account(const std::string& accNumber, const std::string& pwd)
        : accountNumber(accNumber), password(pwd), balance(0.0) {}

    bool authenticate(const std::string& pwd) const {
        return password == pwd;
    }

    void deposit(double amount) {
        if (amount > 0) {
            balance += amount;
            std::cout << "Deposited: $" << amount << std::endl;
        } else {
            std::cout << "Invalid amount. Deposit failed." << std::endl;
        }
    }

    void withdraw(double amount) {
        if (amount > 0 && amount <= balance) {
            balance -= amount;
            std::cout << "Withdrew: $" << amount << std::endl;
        } else {
            std::cout << "Invalid amount or insufficient balance. Withdrawal failed." << std::endl;
        }
    }

    void displayBalance() const {
        std::cout << "Current balance: $" << std::fixed << std::setprecision(2) << balance << std::endl;
    }
};

class Bank {
private:
    std::unordered_map<std::string, Account> accounts;

public:
    void createAccount(const std::string& accNumber, const std::string& password) {
        if (accounts.find(accNumber) == accounts.end()) {
            accounts.emplace(accNumber, Account(accNumber, password));
            std::cout << "Account created successfully." << std::endl;
        } else {
            std::cout << "Account number already exists." << std::endl;
        }
    }

    Account* login(const std::string& accNumber, const std::string& password) {
        auto it = accounts.find(accNumber);
        if (it != accounts.end() && it->second.authenticate(password)) {
            std::cout << "Login successful." << std::endl;
            return &it->second;
        } else {
            std::cout << "Invalid account number or password." << std::endl;
            return nullptr;
        }
    }
};

std::string inputPassword() {
    std::string pwd;
    char ch;
    std::cout << "Enter password: ";
    while ((ch = getch()) != '\n') {
        if (ch == 127 || ch == 8) { // Handle backspace
            if (!pwd.empty()) {
                std::cout << "\b \b";
                pwd.pop_back();
            }
        } else {
            std::cout << '*';
            pwd.push_back(ch);
        }
    }
    std::cout << std::endl;
    return pwd;
}

int main() {
    Bank bank;
    std::string accountNumber, password;
    int choice;
    Account* currentAccount = nullptr;

    do {
        std::cout << "\n1. Create Account\n2. Login\n3. Deposit\n4. Withdraw\n5. Display Balance\n6. Exit\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignore any remaining input

        switch (choice) {
        case 1:
            std::cout << "Enter account number: ";
            std::getline(std::cin, accountNumber);
            password = inputPassword();
            bank.createAccount(accountNumber, password);
            break;
        case 2:
            std::cout << "Enter account number: ";
            std::getline(std::cin, accountNumber);
            password = inputPassword();
            currentAccount = bank.login(accountNumber, password);
            break;
        case 3:
            if (currentAccount) {
                double amount;
                std::cout << "Enter amount to deposit: ";
                std::cin >> amount;
                currentAccount->deposit(amount);
            } else {
                std::cout << "Please login first." << std::endl;
            }
            break;
        case 4:
            if (currentAccount) {
                double amount;
                std::cout << "Enter amount to withdraw: ";
                std::cin >> amount;
                currentAccount->withdraw(amount);
            } else {
                std::cout << "Please login first." << std::endl;
            }
            break;
        case 5:
            if (currentAccount) {
                currentAccount->displayBalance();
            } else {
                std::cout << "Please login first." << std::endl;
            }
            break;
        case 6:
            std::cout << "Exiting..." << std::endl;
            break;
        default:
            std::cout << "Invalid choice. Please try again." << std::endl;
        }
    } while (choice != 6);

    return 0;
}
