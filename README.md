# Big O Demo: Arrays

**Not graded** -- this is a runnable demonstration, not an assignment.

## What This Shows

Times array operations at doubling sizes (1,000 to 16,000), and measures the memory and
copying it takes to hold them. It puts numbers behind CT 06, CT 07 and Lab 3.

| Operation | Static array | Dynamic array | Why |
|-----------|--------------|---------------|-----|
| `arr[i]` index access | O(1) | O(1) | One address calculation: base + i &times; sizeof(int) |
| linear search | O(n) | O(n) | A missing value means checking every element |
| `push_back` | -- (fixed size) | O(1) amortized **when doubling**, O(n) **when growing by one** | Doubling copies rarely; growing by one copies everything, every time |
| insert at front | -- | O(n) | Every element shifts one slot right |
| remove from front | -- | O(n) | Every remaining element shifts one slot left |

The space demo shows the other side of the `push_back` trade:

| Growth rule | Memory reserved | Reallocations to reach n | Elements copied to reach n |
|-------------|-----------------|--------------------------|----------------------------|
| static array | exactly n | 0 -- it cannot grow | 0 |
| doubling | up to 2n | about log&#8322;(n) | fewer than 2n |
| grow by one | exactly n | n | about n&sup2; / 2 |

All three are O(n) space. Doubling spends extra memory to buy cheap appends -- which is
exactly the trade `std::vector` makes.

## How to Run

```bash
cmake -B build
cmake --build build
./build/big-o-demo-arrays          # time:  prints tables, writes results.csv, makes charts.html
./build/big-o-demo-arrays-space    # space: prints tables, writes results_space.csv, makes charts_space.html
```

Each program finishes by running `graph.py`, which turns its CSV into charts. Open
`charts.html` or `charts_space.html` in a browser. `graph.py` needs nothing installed --
only Python itself -- so it works in the Dev Container, on Windows and on a Mac.

## Reading the Output

Each size is **double** the one before it, so the `growth` column tells you the complexity:

| growth | means | because |
|--------|-------|---------|
| ~1x | O(1) | doubling n did not change the cost per operation |
| ~2x | O(n) | doubling n doubled the cost per operation |

The smallest sizes are noisy -- timings of a few nanoseconds are at the mercy of whatever
else the computer is doing. Trust the pattern across the larger rows. Every measurement is
run three times and the fastest run is kept, which removes most of the noise.

The space demo uses sizes one **past** a power of two (1,025, 2,049, ...). That is the
moment right after doubling has grown, when it is at its emptiest -- the worst case for
wasted space, on purpose.

## Files

| File | Purpose |
|------|---------|
| `src/DynamicArray.h` | A minimal growable array with a switchable growth rule. It counts its own reallocations, copies and peak memory, so the space numbers are measured, not calculated |
| `src/bench.h` | Timing, table printing, CSV writing and chart generation shared by both programs |
| `src/main.cpp` | The time demo |
| `src/space_main.cpp` | The space demo |
| `graph.py` | Turns `results.csv` / `results_space.csv` into HTML charts with a data table under each one |
