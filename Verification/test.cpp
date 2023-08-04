#include <iostream>
#include <cmath>
#include <fstream>
#include <cstdlib>  // Include the required header for rand() and srand()
#include <ctime>    // Include the required header for time()

using namespace std;

int myabs(int test)
{
    if (test >= 0)
        return test;
    else
    {
        test = test * (-1);
        return test;
    }
}

int main()
{
    int correct = 0, test = 0;

    // Create a txt file to store the test data
    ofstream outfile("test_data.txt");

    // Seed the random number generator
    srand(time(0));

    for (int i = 0; i < 100; i++) // Generate 100 test cases
    {
        // Generate random correct and test values
        correct = rand() % 1000 - 500; // Random value between -500 and 499
        test = rand() % 1000 - 500;

        // Write the test data to the txt file
        outfile << correct << " " << test << "\n";

        test = myabs(test);
        correct = abs(correct);

        if (test != correct)
        {
            cout << "Test case failed: " << test << " " << correct << "\n";
            break;
        }
    }

    // Close the txt file
    outfile.close();

    return 0;
}
