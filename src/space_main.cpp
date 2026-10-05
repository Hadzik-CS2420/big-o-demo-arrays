// ============================================================================
// Big O Demo: Arrays -- Space Complexity
// ============================================================================
// Measures what it COSTS in memory and copying to hold n ints, three ways:
//
//   static array   -- exactly n slots, but the size is fixed at compile time
//   doubling       -- capacity jumps 1, 2, 4, 8 ... so up to half can sit empty
//   grow by one    -- never wastes a slot, but copies everything on every push
//
// All three are O(n) space. The difference is the constant in front of n, and
// the work it takes to get there. Every number below is counted by the
// DynamicArray as it runs, not calculated from a formula.
//
// Not graded -- run it, read the output, and open charts_space.html.
// ============================================================================

#include "DynamicArray.h"
#include "bench.h"

namespace {

// Each size is one past a power of two -- the moment doubling has just grown
// and is at its emptiest. That is the worst case for wasted space, on purpose.
const std::vector<int> SIZES = {1025, 2049, 4097, 8193, 16385};

struct SpaceRow {
    std::string metric, structure;
    int n;
    double value;
    std::string unit, note;
};

}  // namespace

int main() {
    std::cout << "============================================================\n";
    std::cout << "  Big O Demo: Arrays -- Space\n";
    std::cout << "============================================================\n\n";

    std::vector<SpaceRow> rows;
    const std::string RESERVED = "memory reserved";
    const std::string REALLOCS = "reallocations to reach n";
    const std::string COPIES = "elements copied while growing";
    const std::string PEAK = "peak memory during a resize";

    std::cout << std::setw(8) << "n"
              << std::setw(14) << "static B"
              << std::setw(14) << "double B"
              << std::setw(14) << "by-one B"
              << std::setw(10) << "dbl reall"
              << std::setw(11) << "1 reall"
              << std::setw(12) << "dbl copied"
              << std::setw(14) << "1 copied" << "\n";
    std::cout << std::string(97, '-') << "\n";

    for (int n : SIZES) {
        DynamicArray dbl(Growth::Double), one(Growth::ByOne);
        for (int i = 0; i < n; ++i) { dbl.push_back(i); one.push_back(i); }

        double static_bytes = static_cast<double>(n) * sizeof(int);
        double dbl_bytes = static_cast<double>(dbl.capacity()) * sizeof(int);
        double one_bytes = static_cast<double>(one.capacity()) * sizeof(int);

        std::cout << std::setw(8) << n
                  << std::setw(14) << static_bytes
                  << std::setw(14) << dbl_bytes
                  << std::setw(14) << one_bytes
                  << std::setw(10) << dbl.reallocations()
                  << std::setw(11) << one.reallocations()
                  << std::setw(12) << dbl.elements_copied()
                  << std::setw(14) << one.elements_copied() << "\n";

        rows.push_back({RESERVED, "static array", n, static_bytes, "bytes",
                        "Doubling can hold up to twice what it needs"});
        rows.push_back({RESERVED, "doubling", n, dbl_bytes, "bytes", ""});
        rows.push_back({RESERVED, "grow by one", n, one_bytes, "bytes", ""});

        rows.push_back({REALLOCS, "doubling", n, static_cast<double>(dbl.reallocations()), "reallocations",
                        "Doubling reallocates about log2(n) times; growing by one reallocates n times"});
        rows.push_back({REALLOCS, "grow by one", n, static_cast<double>(one.reallocations()), "reallocations", ""});

        rows.push_back({COPIES, "doubling", n, static_cast<double>(dbl.elements_copied()), "elements copied",
                        "Doubling copies fewer than 2n in total; growing by one copies about n squared over 2"});
        rows.push_back({COPIES, "grow by one", n, static_cast<double>(one.elements_copied()), "elements copied", ""});

        rows.push_back({PEAK, "doubling", n, static_cast<double>(dbl.peak_bytes()), "bytes",
                        "Old and new blocks are both alive while copying -- about 3x the old block when doubling"});
        rows.push_back({PEAK, "grow by one", n, static_cast<double>(one.peak_bytes()), "bytes", ""});
    }

    std::cout << "\nWhat to notice:\n";
    std::cout << "  - Every column grows with n: all three are O(n) space.\n";
    std::cout << "  - Doubling reserves up to 2x what it uses -- each size here is one\n";
    std::cout << "    past a power of two, its emptiest moment. That empty space is\n";
    std::cout << "    the price of cheap appends -- a trade, not a bug.\n";
    std::cout << "  - Growing by one never wastes a slot, and copies about n*n/2\n";
    std::cout << "    elements to get there. At 16385 ints that is over 134 million.\n";
    std::cout << "  - The static array is the tightest of all, but its size is fixed\n";
    std::cout << "    before the program runs. It cannot grow.\n";

    std::string repo_dir = REPO_DIR;
    std::ofstream csv(repo_dir + "/results_space.csv");
    csv << "metric,structure,n,value,unit,note\n";
    for (const auto& r : rows) {
        csv << r.metric << "," << r.structure << "," << r.n << "," << std::fixed
            << std::setprecision(0) << r.value << "," << r.unit << "," << r.note << "\n";
    }
    csv.close();
    std::cout << "\n  Results written to results_space.csv -- generating charts...\n";
    make_charts(repo_dir, "--space");
    return 0;
}
