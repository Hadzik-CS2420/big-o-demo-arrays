// ============================================================================
// Big O Demo: Arrays -- Time Complexity
// ============================================================================
// Times array operations at doubling sizes so you can SEE the growth patterns
// behind CT 06, CT 07 and Lab 3:
//
//   index access     O(1)   -- arr[i] is one address calculation
//   linear search    O(n)   -- may look at every element
//   push_back        O(1) amortized when the array DOUBLES,
//                    O(n) per push when it grows by ONE
//   insert at front  O(n)   -- everything shifts right
//   remove front     O(n)   -- everything shifts left
//
// Sizes double each step, so read the 'growth' column like this:
//   ~1x  -> O(1)   doubling n did not change the cost per operation
//   ~2x  -> O(n)   doubling n doubled the cost per operation
//
// Not graded -- run it, read the output, and open charts.html.
// ============================================================================

#include "DynamicArray.h"
#include "bench.h"

#include <algorithm>

namespace {

const std::vector<int> SIZES = {1000, 2000, 4000, 8000, 16000};
constexpr int MAX_N = 16000;
constexpr int RUNS = 3;   // each measurement is repeated; the fastest run counts

int fixed_array[MAX_N];   // a static array: size fixed when the program is compiled

// Best of RUNS -- the fastest run is the one least disturbed by the OS.
template <typename Func>
double best_us(Func&& func) {
    double best = 1e300;
    for (int r = 0; r < RUNS; ++r) best = std::min(best, func());
    return best;
}

// A cheap repeatable "random" index sequence, so every run reads the same spots.
inline std::size_t next_index(std::size_t& state, std::size_t n) {
    state = state * 1103515245u + 12345u;
    return (state >> 8) % n;
}

void run(std::vector<BenchResult>& results, const std::string& title,
         const std::string& op, const std::string& structure,
         const std::string& complexity, const std::string& note,
         double (*measure)(int)) {
    print_header(title);
    double prev = 0;
    for (int n : SIZES) {
        double us = best_us([&] { return measure(n); });
        print_row(n, us, prev);
        results.push_back({op, structure, complexity, n, us, note});
        prev = us;
    }
}

// ── Each measurement returns microseconds PER OPERATION at size n ──────────

double static_index(int n) {
    for (int i = 0; i < n; ++i) fixed_array[i] = i;
    std::size_t s = 1;
    long long sum = 0;
    double t = time_us([&] {
        for (int k = 0; k < n; ++k) sum += fixed_array[next_index(s, n)];
    });
    sink = sum;
    return t / n;
}

double dynamic_index(int n) {
    DynamicArray a(Growth::Double);
    for (int i = 0; i < n; ++i) a.push_back(i);
    std::size_t s = 1;
    long long sum = 0;
    double t = time_us([&] {
        for (int k = 0; k < n; ++k) sum += a.at(next_index(s, n));
    });
    sink = sum;
    return t / n;
}

double search_missing(int n) {
    DynamicArray a(Growth::Double);
    for (int i = 0; i < n; ++i) a.push_back(i);
    constexpr int SEARCHES = 200;
    int found = 0;
    double t = time_us([&] {
        for (int k = 0; k < SEARCHES; ++k) found += a.contains(-1);   // never there
    });
    sink = found;
    return t / SEARCHES;
}

double push_doubling(int n) {
    double t = 0;
    {
        DynamicArray a(Growth::Double);
        t = time_us([&] { for (int i = 0; i < n; ++i) a.push_back(i); });
        sink = static_cast<long long>(a.size());
    }
    return t / n;
}

double push_by_one(int n) {
    double t = 0;
    {
        DynamicArray a(Growth::ByOne);
        t = time_us([&] { for (int i = 0; i < n; ++i) a.push_back(i); });
        sink = static_cast<long long>(a.size());
    }
    return t / n;
}

double insert_front(int n) {
    DynamicArray a(Growth::Double);
    double t = time_us([&] { for (int i = 0; i < n; ++i) a.insert_front(i); });
    sink = static_cast<long long>(a.size());
    return t / n;
}

double remove_front(int n) {
    DynamicArray a(Growth::Double);
    for (int i = 0; i < n; ++i) a.push_back(i);
    double t = time_us([&] { for (int i = 0; i < n; ++i) a.remove_front(); });
    sink = static_cast<long long>(a.size());
    return t / n;
}

}  // namespace

int main() {
    std::cout << "============================================================\n";
    std::cout << "  Big O Demo: Arrays -- Time\n";
    std::cout << "============================================================\n";
    std::cout << "\nEach size is double the one before it. Watch 'growth':\n";
    std::cout << "  ~1x  the cost per operation did not change   -> O(1)\n";
    std::cout << "  ~2x  the cost per operation doubled with n   -> O(n)\n";
    std::cout << "Small sizes are noisy; trust the pattern across the bigger rows.\n";

    std::vector<BenchResult> results;

    run(results, "Index access, static array  arr[i] - O(1)", "index access arr[i]",
        "static array", "O(1)", "One address calculation: base + i * sizeof(int)", static_index);
    run(results, "Index access, dynamic array - O(1)", "index access arr[i]",
        "dynamic array", "O(1)", "One address calculation: base + i * sizeof(int)", dynamic_index);

    run(results, "Linear search for a value that is not there - O(n)", "linear search (not found)",
        "dynamic array", "O(n)", "A missing value means checking every element", search_missing);

    run(results, "push_back, growing by DOUBLING - O(1) amortized", "push_back",
        "doubling", "O(1) amortized", "Doubling copies rarely; growing by one copies every time",
        push_doubling);
    run(results, "push_back, growing by ONE - O(n)  [THE SLOW ONE]", "push_back",
        "grow by one", "O(n)", "Doubling copies rarely; growing by one copies every time",
        push_by_one);

    run(results, "insert at front - O(n)", "insert at front",
        "dynamic array", "O(n)", "Every element shifts one slot right", insert_front);
    run(results, "remove from front - O(n)", "remove from front",
        "dynamic array", "O(n)", "Every remaining element shifts one slot left", remove_front);

    std::cout << "\nWhat to notice:\n";
    std::cout << "  - Index access is flat no matter how big the array gets. That is\n";
    std::cout << "    what arrays are best at, and what linked lists cannot do.\n";
    std::cout << "  - push_back is the same code both times. Only the growth rule\n";
    std::cout << "    changed -- and it turned O(1) into O(n). That is why vector doubles.\n";
    std::cout << "  - The front of an array is expensive: every insert or remove there\n";
    std::cout << "    shifts everything else. Module 4's linked lists fix exactly this.\n";

    std::string repo_dir = REPO_DIR;
    write_time_csv(repo_dir + "/results.csv", results);
    std::cout << "\n  Results written to results.csv -- generating charts...\n";
    make_charts(repo_dir, "");
    return 0;
}
