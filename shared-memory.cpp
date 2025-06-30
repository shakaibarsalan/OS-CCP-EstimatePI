#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <cmath>
#include <atomic>
using namespace std;

typedef atomic_flag lock_t;

// #define LOCK_INIT ATOMIC_FLAG_INIT

static inline void acquire(lock_t *lock)
{
    while (lock->test_and_set(memory_order_acquire));
}

static inline void release(lock_t *lock)
{
    lock->clear(memory_order_release);
}

long double maclaurin_polynomial_for_PI(unsigned int n)
{
    return ((n % 2) ? -1.0L : 1.0L) / (2 * n + 1);
}

void child_func(int start, int end, long double *PI, lock_t *mutex)
{
    long double local_sum = 0.0L;
    for (int i = start; i <= end; i++)
    {
        local_sum += maclaurin_polynomial_for_PI(i);
    }

    acquire(mutex);
    *PI += local_sum; // Write to shared memory
    release(mutex);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cout << "Invalid argument count" << endl;
        return 1;
    }

    int no_of_child = atoi(argv[1]);
    int no_of_terms = atoi(argv[2]);

    if (no_of_child < 1 || no_of_terms < 1)
    {
        cout << "Number of processes and terms should be at least 1." << endl;
        return 1;
    }

    // Allocate shared memory
    long double *PI = (long double *)mmap(NULL, sizeof(long double), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    lock_t *mutex = (lock_t *)mmap(NULL, sizeof(lock_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    if (PI == MAP_FAILED || mutex == MAP_FAILED)
    {
        cout << "mmap error";
        return 1;
    }

    *PI = 0.0L;
    // *mutex = LOCK_INIT;
    atomic_flag_clear(mutex);


    int div_each = no_of_terms / no_of_child;
    int mod_remain = no_of_terms % no_of_child;
    int start = 0;

    for (int i = 0; i < no_of_child; i++)
    {
        int end = start + div_each - 1;
        if (i < mod_remain)
            end++;

        pid_t pid = fork();
        if (pid == -1)
        {
            cout << "fork error";
            return 1;
        }
        else if (pid == 0)
        { // Child process
            child_func(start, end, PI, mutex);
            exit(0);
        }
        start = end + 1;
    }

    // Parent waits for all children to finish
    for (int i = 0; i < no_of_child; i++)
    {
        wait(NULL);
    }

    *PI *= 4;
    long double error = 100 - ((*PI / 3.1415926535) * 100);
    cout.precision(20);
    cout << "\n\nPI is: " << fixed << *PI << endl;
    cout << "error: " << error << endl;

    munmap(PI, sizeof(long double));
    munmap(mutex, sizeof(lock_t));

    return 0;
}
