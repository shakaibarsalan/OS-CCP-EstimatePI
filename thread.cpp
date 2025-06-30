#include <iostream>
#include <cmath>
#include <vector>
#include <thread>
#include <mutex>
#include <fstream>
using namespace std;

long double PI = 0.0;
std::mutex mutex;
std::mutex file_mutex;

double maclaurin_polynomial_for_PI(unsigned int n)
{
    return pow(-1, n) / (2 * n + 1);
}

class ThreadTaskRange
{
public:
    unsigned int start;
    unsigned int end;
};

void thread_task(const ThreadTaskRange &range, ofstream &file)
{
    long double partialPI = 0.0;

    for (unsigned int i = range.start; i <= range.end; ++i)
    {
        partialPI += maclaurin_polynomial_for_PI(i);
    }

    {
        lock_guard<std::mutex> lock(std::mutex);
        PI += partialPI;
    }

    {
        lock_guard<std::mutex> file_lock(file_mutex);
        file << "Thread Range: " << range.start << " to " << range.end << ", Partial PI: " << partialPI << "\n";
    }
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cout << "Invalid argument count\n";
        return 1;
    }

    unsigned int no_of_threads = stoul(argv[1]);
    unsigned int no_of_terms = stoul(argv[2]);

    cout << "Thread count: " << no_of_threads << "\n";
    cout << "# of terms: " << no_of_terms << "\n";

    if (no_of_threads > no_of_terms || no_of_terms < 1 || no_of_threads < 1)
    {
        cout << "Invalid inputs. Ensure no_of_terms >= no_of_threads and both are > 0.\n";
        return 1;
    }

    unsigned int div_each = no_of_terms / no_of_threads;
    unsigned int mod_remain = no_of_terms % no_of_threads;

    unsigned int start = 0;

    vector<thread> threads;
    ofstream file("thread_results.txt");

    if (!file.is_open())
    {
        cout << "Error opening file\n";
        return 1;
    }

    {
        lock_guard<std::mutex> file_lock(file_mutex);
        file << "Calculating PI using threads:\n";
    }

    for (unsigned int i = 0; i < no_of_threads; ++i)
    {
        ThreadTaskRange range;
        range.start = start;
        range.end = range.start + div_each - 1;

        if (i < mod_remain)
        {
            range.end++;
        }

        threads.emplace_back([=, &file]()
                             { thread_task(range, file); });

        start = range.end + 1;
    }

    for (auto &thread : threads)
    {
        thread.join();
    }

    {
        lock_guard<std::mutex> file_lock(file_mutex);
        file << "\nValue of PI before *4: " << PI;
    }

    PI *= 4;

    {
        lock_guard<std::mutex> file_lock(file_mutex);
        file << "\nFinal PI value: " << PI << "\n";
    }

    file.close();
    cout << "\nPI is: " << PI << "\n";

    return 0;
}
