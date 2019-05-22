#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>
#include "evd_cost.hpp"
#include "evd_cyclic.hpp"
#include "perf_test.hpp"
#include "types.hpp"

int main() {
    std::ios_base::sync_with_stdio(false);  // disable synchronization between C and C++ standard streams
    std::cin.tie(NULL);                     // untie cin from cout

    size_t n, n_iter = 10;
    std::cin >> n >> n;
    std::cerr << "Performance benchmark on array of size " << n << " by " << n << std::endl;

    aligned_vector<double> A(n * n);
    aligned_vector<double> A_copy(n * n);
    aligned_vector<double> e(n);
    aligned_vector<double> V(n * n, 0);
    matrix_t Data_matr = {&A[0], n, n};
    matrix_t Data_matr_copy = {&A_copy[0], n, n};
    vector_t E_vals = {&e[0], n};
    matrix_t E_vecs = {&V[0], n, n};
    size_t b = 8, individual_block_iter = 5;

    for (size_t i = 0; i < n * n; ++i) {
        std::cin >> A[i];
    }

    size_t cost = blocked_cost_without_subprocedure_evd(n, b, n_iter, individual_block_iter);
    bench_func(evd_cyclic_blocked, "evd_cyclic_blocked_version", cost, Data_matr, Data_matr_copy, E_vecs, E_vals,
               n_iter);
}
