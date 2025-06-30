#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>
#include <cmath>

using namespace std;

long double maclaurin_polynomial_for_PI(unsigned int n) {
    return ((n % 2) ? -1.0L : 1.0L) / (2 * n + 1);
}

long double child_func(int start, int end) {
    long double local_sum = 0.0L;
    for (unsigned int i = start; i <= end; i++) {
        local_sum += maclaurin_polynomial_for_PI(i);
    }
    return local_sum;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        cout << "Usage: " << argv[0] << " <num_processes> <num_terms>" << endl;
        return 1;
    }

    int no_of_processes = atoi(argv[1]);
    int no_of_terms = atoi(argv[2]);

    if (no_of_processes < 1 || no_of_terms < 1) {
        cout << "Number of processes and terms must be at least 1." << endl;
        return 1;
    }

    int pipes[no_of_processes][2];
    int div_each = no_of_terms / no_of_processes;
    int mod_remain = no_of_terms % no_of_processes;
    int start = 0;

    for (int i = 0; i < no_of_processes; i++) {
        if (pipe(pipes[i]) == -1) {
            cout << "Pipe error";
            return 1;
        }

        int end = start + div_each - 1;
        if (i < mod_remain) end++; // Distribute remainder

        pid_t pid = fork();

        if (pid == -1) {
            cout << "Fork error";
            return 1;
        } 
        else if (pid == 0) { // Child process
            close(pipes[i][0]); // Close read end
            long double partial_sum = child_func(start, end);
            write(pipes[i][1], &partial_sum, sizeof(long double));
            close(pipes[i][1]);
            exit(0);
        }

        start = end + 1; // Update for next process
    }

    long double PI = 0.0L;

    // Parent process collects results
    for (int i = 0; i < no_of_processes; i++) {
        close(pipes[i][1]); // Close write end
        long double partial_sum;
        read(pipes[i][0], &partial_sum, sizeof(long double));
        close(pipes[i][0]);
        wait(NULL); // Wait for child
        PI += partial_sum;
    }

    PI *= 4;
    cout.precision(20);
    cout << "\nPI is: " << PI << endl;

    return 0;
}
