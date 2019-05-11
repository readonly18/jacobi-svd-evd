#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>
#include "perf_test.hpp"
#include "svd.hpp"
#include "types.hpp"

int main() {
    std::ios_base::sync_with_stdio(false);  // disable synchronization between C and C++ standard streams
    std::cin.tie(NULL);                     // untie cin from cout

    size_t n;
    std::cin >> n >> n;
    std::cout << "Performance benchmark on array of size " << n << " by " << n << std::endl;

    std::vector<double> A(n * n);
    std::vector<double> B(n * n);
    std::vector<double> U(n * n, 0);
    std::vector<double> V(n * n, 0);
    matrix_t Data_matr = {&A[0], n, n};
    matrix_t B_mat = {&B[0], n, n};
    matrix_t U_mat = {&U[0], n, n};
    matrix_t V_mat = {&V[0], n, n};

    for (size_t i = 0; i < n * n; ++i) {
        std::cin >> A[i];
    }

    // size_t cost = svd(Data_matr, B_mat, U_mat, V_mat);
    // bench_func(svd, "svd_two_sided", cost, Data_matr, B_mat, U_mat, V_mat);

    // size_t block_size = 32;
    // size_t cost = svd_blocked(Data_matr, B_mat, U_mat, V_mat, block_size);
    // bench_func(svd_blocked, "svd_two_sided_blocked", cost, Data_matr, B_mat, U_mat, V_mat, block_size);

    size_t block_size = 32;
    size_t cost = svd_blocked_less_copy(Data_matr, B_mat, U_mat, V_mat, block_size);
    bench_func(svd_blocked_less_copy, "svd_two_sided_blocked_less_copy", cost, Data_matr, B_mat, U_mat, V_mat,
               block_size);
}
