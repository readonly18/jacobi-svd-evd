#include "evd_cyclic.hpp"
#include <math.h>
#include <iostream>
#include <random>
#include <vector>
#include "../../test_utils.hpp"
#include "gtest/gtest.h"
#include "types.hpp"

TEST(evd_cyclic, identity_matrix) {
    size_t n = 10;
    aligned_vector<double> A(n * n, 0);
    aligned_vector<double> A_copy(n * n, 0);
    for (size_t i = 0; i < n; ++i) {
        A[i * n + i] = 1.0;
    }
    aligned_vector<double> e(n);
    aligned_vector<double> V(n * n, 0);
    matrix_t Data_matr = {&A[0], n, n};
    matrix_t Data_matr_copy = {&A_copy[0], n, n};
    vector_t E_vals = {&e[0], n};
    matrix_t E_vecs = {&V[0], n, n};

    evd_cyclic(Data_matr, Data_matr_copy, E_vecs, E_vals, 10);
    for (size_t i = 0; i < n; ++i) {
        ASSERT_DOUBLE_EQ(e[i], 1.0);
    }
}

TEST(evd_cyclic, random_square_matrix) {
    size_t n = 4;
    aligned_vector<double> A = {7.0, 3.0, 2.0, 1.0, 3.0, 9.0, -2.0, 4.0, 2.0, -2.0, -4.0, 2.0, 1.0, 4.0, 2.0, 3.0};
    aligned_vector<double> A_copy(n * n, 0);
    aligned_vector<double> e(n);
    aligned_vector<double> V(n * n, 0);

    matrix_t Data_matr = {&A[0], n, n};
    matrix_t Data_matr_copy = {&A_copy[0], n, n};
    vector_t E_vals = {&e[0], n};
    matrix_t E_vecs = {&V[0], n, n};

    evd_cyclic(Data_matr, Data_matr_copy, E_vecs, E_vals, 10);

    aligned_vector<double> e_expect = {12.71986, 5.78305, 2.09733, -5.60024};

    for (size_t i = 0; i < n; ++i) {
        ASSERT_NEAR(e[i], e_expect[i], 1e-5);
    }
}

TEST(evd_cyclic, evd_crosscheck) {
    size_t n = 5;
    aligned_vector<double> A = {
        2.000000000000000000e+00, 6.000000000000000000e+00, 4.000000000000000000e+00, 6.000000000000000000e+00,
        4.500000000000000000e+00, 6.000000000000000000e+00, 5.000000000000000000e+00, 8.000000000000000000e+00,
        5.500000000000000000e+00, 4.500000000000000000e+00, 4.000000000000000000e+00, 8.000000000000000000e+00,
        3.000000000000000000e+00, 6.000000000000000000e+00, 4.000000000000000000e+00, 6.000000000000000000e+00,
        5.500000000000000000e+00, 6.000000000000000000e+00, 1.000000000000000000e+00, 2.000000000000000000e+00,
        4.500000000000000000e+00, 4.500000000000000000e+00, 4.000000000000000000e+00, 2.000000000000000000e+00,
        7.000000000000000000e+00};
    aligned_vector<double> A_copy(n * n, 0);
    aligned_vector<double> e(n);
    aligned_vector<double> V(n * n, 0);

    matrix_t Data_matr = {&A[0], n, n};
    matrix_t Data_matr_copy = {&A_copy[0], n, n};
    vector_t E_vals = {&e[0], n};
    matrix_t E_vecs = {&V[0], n, n};

    evd_cyclic(Data_matr, Data_matr_copy, E_vecs, E_vals, 10);

    aligned_vector<double> e_expect = {2.415032147975995969e+01, 4.001355036163166012e+00, -1.007738346679503572e+00,
                                       -3.262428878677021693e+00, -5.881509290566617310e+00};

    for (size_t i = 0; i < n; ++i) {
        ASSERT_NEAR(e[i], e_expect[i], 1e-7);
    }
}

TEST(evd_cyclic, eigenvector_check) {
    size_t n = 5;
    aligned_vector<double> A = {
        2.000000000000000000e+00, 6.000000000000000000e+00, 4.000000000000000000e+00, 6.000000000000000000e+00,
        4.500000000000000000e+00, 6.000000000000000000e+00, 5.000000000000000000e+00, 8.000000000000000000e+00,
        5.500000000000000000e+00, 4.500000000000000000e+00, 4.000000000000000000e+00, 8.000000000000000000e+00,
        3.000000000000000000e+00, 6.000000000000000000e+00, 4.000000000000000000e+00, 6.000000000000000000e+00,
        5.500000000000000000e+00, 6.000000000000000000e+00, 1.000000000000000000e+00, 2.000000000000000000e+00,
        4.500000000000000000e+00, 4.500000000000000000e+00, 4.000000000000000000e+00, 2.000000000000000000e+00,
        7.000000000000000000e+00};
    aligned_vector<double> A_copy(n * n, 0);
    aligned_vector<double> e(n);
    aligned_vector<double> V(n * n, 0);

    matrix_t Data_matr = {&A[0], n, n};
    matrix_t Data_matr_copy = {&A_copy[0], n, n};
    vector_t E_vals = {&e[0], n};
    matrix_t E_vecs = {&V[0], n, n};

    evd_cyclic(Data_matr, Data_matr_copy, E_vecs, E_vals, 10);

    aligned_vector<double> V_expect = {
        4.183471639051151714e-01,  4.111245337904025771e-02,  7.089492553079320691e-01,  -2.465472573146875179e-01,
        -5.098046880312467888e-01, 5.351174862492769080e-01,  2.123587416842354081e-01,  -3.608245211597456148e-01,
        -6.773537917555900734e-01, 2.820470642724520749e-01,  4.694747160408517250e-01,  2.236196547254168665e-01,
        -4.844770415395814878e-01, 4.881635720053102978e-01,  -5.065235080496436337e-01, 3.922584192557757032e-01,
        3.227049851987033868e-01,  3.613226388290429747e-01,  4.766867428706260679e-01,  6.198471786592852917e-01,
        4.054155274334266257e-01,  -8.939067476844743121e-01, -4.386986686600086172e-02, 1.219523138088823705e-01,
        1.406131021557419369e-01};

    for (size_t j = 0; j < n; ++j) {
        int sign = (V[j] / V_expect[j] < 0) ? -1 : 1;
        for (size_t i = 0; i < n; ++i) {
            ASSERT_NEAR(sign * V[n * i + j], V_expect[n * i + j], 1e-7);
        }
    }
}

TEST(evd_cyclic, random_matrix_big) {
    size_t n = 128;

    aligned_vector<double> A(n * n, 0);
    aligned_vector<double> A_copy(n * n, 0);
    aligned_vector<double> e(n), e_expect(n);
    aligned_vector<double> V(n * n, 0), V_expect(n * n, 0);

    std::string cmd = "python scripts/evd_testdata.py " + std::to_string(n) + " " + std::to_string(n);
    std::stringstream ss(exec_cmd(cmd.c_str()));
    read_into(ss, &A[0], n * n);
    read_into(ss, &e_expect[0], n);
    read_into(ss, &V_expect[0], n * n);

    matrix_t Data_matr = {&A[0], n, n};
    matrix_t Data_matr_copy = {&A_copy[0], n, n};
    vector_t E_vals = {&e[0], n};
    matrix_t E_vecs = {&V[0], n, n};
    evd_cyclic(Data_matr, Data_matr_copy, E_vecs, E_vals, 10);

    for (size_t i = 0; i < n; ++i) {
        ASSERT_NEAR(e[i], e_expect[i], 1e-7);
    }
    for (size_t j = 0; j < n; ++j) {
        // equal up to sign
        int sign = (V[j] / V_expect[j] < 0) ? -1 : 1;
        for (size_t i = 0; i < n; ++i) {
            ASSERT_NEAR(sign * V[i * n + j], V_expect[i * n + j], 1e-7);
        }
    }
}

TEST(evd_cyclic_unroll_vectorize, random_matrix_big) {
    size_t n = 128;

    aligned_vector<double> A(n * n, 0);
    aligned_vector<double> A_copy(n * n, 0);
    aligned_vector<double> e(n), e_expect(n);
    aligned_vector<double> V(n * n, 0), V_expect(n * n, 0);

    std::string cmd = "python scripts/evd_testdata.py " + std::to_string(n) + " " + std::to_string(n);
    std::stringstream ss(exec_cmd(cmd.c_str()));
    read_into(ss, &A[0], n * n);
    read_into(ss, &e_expect[0], n);
    read_into(ss, &V_expect[0], n * n);

    matrix_t Data_matr = {&A[0], n, n};
    matrix_t Data_matr_copy = {&A_copy[0], n, n};
    vector_t E_vals = {&e[0], n};
    matrix_t E_vecs = {&V[0], n, n};
    evd_cyclic_unroll_outer_vectorize(Data_matr, Data_matr_copy, E_vecs, E_vals, 10);

    for (size_t i = 0; i < n; ++i) {
        ASSERT_NEAR(e[i], e_expect[i], 1e-7);
    }
    for (size_t j = 0; j < n; ++j) {
        // equal up to sign
        int sign = (V[j] / V_expect[j] < 0) ? -1 : 1;
        for (size_t i = 0; i < n; ++i) {
            ASSERT_NEAR(sign * V[i * n + j], V_expect[i * n + j], 1e-7);
        }
    }
}
