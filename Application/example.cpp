#include <iostream>
#include <cmath>

double calculateCreditScore(double person1Income, double person1CreditHistory, double person2Income, double person2CreditHistory) {
    // Calculate credit scores for both individuals based on income and credit history
    double person1CreditScore = pow(person1Income, 0.6) * fabs(person1CreditHistory - 600) / 100.0;
    double person2CreditScore = pow(person2Income, 0.6) * fabs(person2CreditHistory - 600) / 100.0;

    // Determine the joint credit score for the loan decision
    double jointCreditScore = (floor(person1CreditScore) + ceil(person2CreditScore)) / fabs(sin((person1CreditScore + person2CreditScore) / 2.0));

    return jointCreditScore;
}

int main() {
    double person1Income;
    double person1CreditHistory;
    double person2Income;
    double person2CreditHistory;

    std::cout << "Enter Person 1's income: ";
    std::cin >> person1Income;

    std::cout << "Enter Person 1's credit history score: ";
    std::cin >> person1CreditHistory;

    std::cout << "Enter Person 2's income: ";
    std::cin >> person2Income;

    std::cout << "Enter Person 2's credit history score: ";
    std::cin >> person2CreditHistory;

    double jointCreditScore = calculateCreditScore(person1Income, person1CreditHistory, person2Income, person2CreditHistory);

    std::cout << "Joint Credit Score: " << jointCreditScore << std::endl;

    // Make loan approval decision based on the joint credit score
    if (jointCreditScore >= 500) {
        std::cout << "Loan Approved!" << std::endl;
    } else {
        std::cout << "Loan Denied." << std::endl;
    }

    return 0;
}