#include "svd.hpp"
#include <assert.h>
#include <math.h>
#include <algorithm>
#include "matrix.hpp"
#include "nsvd.hpp"
#include "types.hpp"
#include "util.hpp"

static inline size_t svd_block(struct matrix_t Bmat, struct matrix_t Umat, struct matrix_t Vmat);
static inline void mult(struct matrix_t Amat, size_t blockA_row, size_t blockA_col,
                        struct matrix_t Bmat, size_t blockB_row, size_t blockB_col,
                        struct matrix_t Cmat, size_t blockC_row, size_t blockC_col, size_t block_size);
static inline void add(struct matrix_t Amat, struct matrix_t Bmat, struct matrix_t Cmat);
static inline void copy(struct matrix_t S, size_t blockS_row, size_t blockS_col,
                        struct matrix_t D, size_t blockD_row, size_t blockD_col, size_t block_size);

#define IDX(i, j, block_i, block_j) ((block_i * block_size * n) + (i * n) + (block_j * block_size) + (j))

size_t svd(struct matrix_t Amat, struct matrix_t Bmat, struct matrix_t Umat, struct matrix_t Vmat) {
    assert(Amat.rows == Amat.cols);  // Matrix A should be square
    assert(Amat.rows == Bmat.rows && Amat.cols == Bmat.cols);
    assert(Amat.rows == Umat.rows && Amat.cols == Umat.cols);
    assert(Amat.rows == Vmat.rows && Amat.cols == Vmat.cols);

    size_t iter = 0;           // count main loop iterations performed till convergence
    const double tol = 1e-15;  // convergence tolerance
    const size_t n = Amat.rows;
    double* B = Bmat.ptr;
    double norm = 0.0;      // frobenius norm of matrix B
    double off_norm = 0.0;  // frobenius norm of the off-diagonal elements of matrix B

    const size_t block_size = 8;
    const size_t n_blocks = n / block_size;
    assert(n_blocks * block_size == n);
    double* memory = (double*)malloc((4 + 4 + 4 + 1 + 1)*block_size*block_size*sizeof(double));
    double* Bblock = memory;
    double* Ublock = Bblock + 4*block_size*block_size;
    double* Vblock = Ublock + 4*block_size*block_size;
    double* M1 = Vblock + 4*block_size*block_size;
    double* M2 = M1 + block_size*block_size;

    matrix_t Bblockmat = {Bblock, 2*block_size, 2*block_size};
    matrix_t Ublockmat = {Ublock, 2*block_size, 2*block_size};
    matrix_t Vblockmat = {Vblock, 2*block_size, 2*block_size};
    matrix_t M1mat = {M1, block_size, block_size};
    matrix_t M2mat = {M2, block_size, block_size};

    matrix_copy(Bmat, Amat);
    matrix_identity(Umat);
    matrix_identity(Vmat);
    matrix_frobenius(Bmat, &norm, &off_norm);

    while (off_norm >= tol * tol * norm) {
        for (size_t i_block = 0; i_block < n_blocks - 1; ++i_block) {
            for (size_t j_block = i_block + 1; j_block < n_blocks; ++j_block) {
                // block 00
                for (size_t i = 0; i < block_size; ++i) {
                    for (size_t j = 0; j < block_size; ++j) {
                        Bblock[2 * block_size * i + j] = B[IDX(i, j, i_block, i_block)];
                    }
                }
                // block 01
                for (size_t i = 0; i < block_size; ++i) {
                    for (size_t j = 0; j < block_size; ++j) {
                        Bblock[2 * block_size * i + j + block_size] = B[IDX(i, j, i_block, j_block)];
                    }
                }
                // block 10
                for (size_t i = 0; i < block_size; ++i) {
                    for (size_t j = 0; j < block_size; ++j) {
                        Bblock[2 * block_size * (block_size + i) + j] = B[IDX(i, j, j_block, i_block)];
                    }
                }
                // block 11
                for (size_t i = 0; i < block_size; ++i) {
                    for (size_t j = 0; j < block_size; ++j) {
                        Bblock[2 * block_size * (block_size + i) + j + block_size] = B[IDX(i, j, j_block, j_block)];
                    }
                }

                svd_block(Bblockmat, Ublockmat, Vblockmat);

                for (size_t k_block = 0; k_block < n_blocks; ++k_block) {
                    mult(Ublockmat, 0, 0, Bmat, i_block, k_block, M1mat, 0, 0, block_size);
                    mult(Ublockmat, 1, 0, Bmat, j_block, k_block, M2mat, 0, 0, block_size);
                    add(M1mat, M2mat, M2mat);
                    mult(Ublockmat, 0, 1, Bmat, i_block, k_block, M1mat, 0, 0, block_size);
                    copy(M2mat, 0, 0, Bmat, i_block, k_block, block_size);
                    mult(Ublockmat, 1, 1, Bmat, j_block, k_block, M2mat, 0, 0, block_size);
                    add(M1mat, M2mat, M2mat);
                    copy(M2mat, 0, 0, Bmat, j_block, k_block, block_size);
                }

                for (size_t k_block = 0; k_block < n_blocks; ++k_block) {
                    mult(Bmat, k_block, i_block, Vblockmat, 0, 0, M1mat, 0, 0, block_size);
                    mult(Bmat, k_block, j_block, Vblockmat, 1, 0, M2mat, 0, 0, block_size);
                    add(M1mat, M2mat, M2mat);
                    mult(Bmat, k_block, i_block, Vblockmat, 0, 1, M1mat, 0, 0, block_size);
                    copy(M2mat, 0, 0, Bmat, k_block, i_block, block_size);
                    mult(Bmat, k_block, j_block, Vblockmat, 1, 1, M2mat, 0, 0, block_size);
                    add(M1mat, M2mat, M2mat);
                    copy(M2mat, 0, 0, Bmat, k_block, j_block, block_size);
                }

                for (size_t k_block = 0; k_block < n_blocks; ++k_block) {
                    mult(Umat, k_block, i_block, Ublockmat, 0, 0, M1mat, 0, 0, block_size);
                    mult(Umat, k_block, j_block, Ublockmat, 1, 0, M2mat, 0, 0, block_size);
                    add(M1mat, M2mat, M2mat);
                    mult(Umat, k_block, i_block, Ublockmat, 0, 1, M1mat, 0, 0, block_size);
                    copy(M2mat, 0, 0, Umat, k_block, i_block, block_size);
                    mult(Umat, k_block, j_block, Vblockmat, 1, 1, M2mat, 0, 0, block_size);
                    add(M1mat, M2mat, M2mat);
                    copy(M2mat, 0, 0, Umat, k_block, j_block, block_size);
                }

                for (size_t k_block = 0; k_block < n_blocks; ++k_block) {
                    mult(Vmat, k_block, i_block, Vblockmat, 0, 0, M1mat, 0, 0, block_size);
                    mult(Vmat, k_block, j_block, Vblockmat, 1, 0, M2mat, 0, 0, block_size);
                    add(M1mat, M2mat, M2mat);
                    mult(Vmat, k_block, i_block, Vblockmat, 0, 1, M1mat, 0, 0, block_size);
                    copy(M2mat, 0, 0, Vmat, k_block, i_block, block_size);
                    mult(Vmat, k_block, j_block, Vblockmat, 1, 1, M2mat, 0, 0, block_size);
                    add(M1mat, M2mat, M2mat);
                    copy(M2mat, 0, 0, Vmat, k_block, j_block, block_size);
                }
            }
        }

        matrix_off_frobenius(Bmat, &off_norm);
        iter += 1;
    }

    free(memory);

    return iter;
}

static size_t svd_block(struct matrix_t Bmat, struct matrix_t Umat, struct matrix_t Vmat) {
    size_t iter = 0;           // count main loop iterations performed till convergence
    size_t n = Bmat.rows;
    const double tol = 1e-15;  // convergence tolerance
    double* B = Bmat.ptr;
    double* U = Umat.ptr;
    double* V = Vmat.ptr;
    double norm = 0.0;      // frobenius norm of matrix B
    double off_norm = 0.0;  // frobenius norm of the off-diagonal elements of matrix B

    matrix_identity(Umat);
    matrix_identity(Vmat);
    matrix_frobenius(Bmat, &norm, &off_norm);

    // Repeat while the frobenius norm of the off-diagonal elements of matrix B, which is updated in every
    // iteration, is smaller than the forbenius norm of the original matrix B (or A) times the tolerance
    while (off_norm >= tol * tol * norm) {
        for (size_t i = 0; i < n - 1; ++i) {
            for (size_t j = i + 1; j < n; ++j) {
                const double bii = B[n * i + i];  // B[i][i]
                const double bij = B[n * i + j];  // B[i][j]
                const double bji = B[n * j + i];  // B[j][i]
                const double bjj = B[n * j + j];  // B[j][j]

                // Compute the 2x2 svd of B[i][i], B[i][j], B[j][i] and B[j][j]
                struct svd_2x2_params cf = nsvd(bii, bij, bji, bjj);

                // R_ij(c,s) * B where R_ij(c,s) is the Givens rotation matrix that acts on
                // rows i and j during left multiplication with B
                for (size_t k = 0; k < n; ++k) {
                    double left = cf.c1 * B[n * i + k] - cf.s1 * B[n * j + k];
                    double right = cf.s1 * cf.k * B[n * i + k] + cf.c1 * cf.k * B[n * j + k];
                    B[n * i + k] = left;
                    B[n * j + k] = right;
                }

                // B * R_ij(c',-s') where R_ij(c',-s') is the Givens rotation matrix that acts on
                // columns i and j during right multiplication with B
                for (size_t k = 0; k < n; ++k) {
                    double left = cf.c2 * B[n * k + i] - cf.s2 * B[n * k + j];
                    double right = cf.s2 * B[n * k + i] + cf.c2 * B[n * k + j];
                    B[n * k + i] = left;
                    B[n * k + j] = right;
                }

                // U * R_ij(c,s) where R_ij(c,s) is the Givens rotation matrix that acts on
                // columns i and j during right multiplication with U
                for (size_t k = 0; k < n; ++k) {
                    double left = cf.c1 * U[n * k + i] - cf.s1 * U[n * k + j];
                    double right = cf.s1 * cf.k * U[n * k + i] + cf.c1 * cf.k * U[n * k + j];
                    U[n * k + i] = left;
                    U[n * k + j] = right;
                }

                // V * R_ij(c',-s') where R_ij(c',-s') is the Givens rotation matrix that acts on
                // columns i and j during right multiplication with V
                for (size_t k = 0; k < n; ++k) {
                    double left = cf.c2 * V[n * k + i] - cf.s2 * V[n * k + j];
                    double right = cf.s2 * V[n * k + i] + cf.c2 * V[n * k + j];
                    V[n * k + i] = left;
                    V[n * k + j] = right;
                }
            }
        }

        matrix_off_frobenius(Bmat, &off_norm);
        iter += 1;
    }

    return iter;
}

static inline void mult(struct matrix_t Amat, size_t blockA_row, size_t blockA_col,
                 struct matrix_t Bmat, size_t blockB_row, size_t blockB_col,
                 struct matrix_t Cmat, size_t blockC_row, size_t blockC_col, size_t block_size) {
    size_t n = Amat.rows;
    double* A = Amat.ptr;
    double* B = Bmat.ptr;
    double* C = Cmat.ptr;
    size_t Abeg = blockA_row*block_size*n + blockA_col*block_size;
    size_t Bbeg = blockB_row*block_size*n + blockB_col*block_size;
    size_t Cbeg = blockC_row*block_size*n + blockC_col*block_size;
    for (size_t i = 0; i < block_size; ++i) {
        for (size_t j = 0; j < block_size; ++j) {
            C[Cbeg + i*n + j] = 0.0;
            for (size_t k = 0; k < block_size; ++k) {
                C[Cbeg + i*n + j] += A[Abeg + i*n + k] * B[Bbeg + k*n + j];
            }
        }
    }
}

static inline void add(struct matrix_t Amat, struct matrix_t Bmat, struct matrix_t Cmat) {
    size_t n = Amat.rows;
    double* A = Amat.ptr;
    double* B = Bmat.ptr;
    double* C = Cmat.ptr;
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            C[n*i + j] = A[n*i + j] + B[n*i + j];
        }
    }
}

static inline void copy(struct matrix_t Smat, size_t blockS_row, size_t blockS_col,
                        struct matrix_t Dmat, size_t blockD_row, size_t blockD_col, size_t block_size) {
    size_t n = Dmat.rows;
    double* S = Smat.ptr;
    double* D = Dmat.ptr;
    size_t Sbeg = blockS_row*block_size*n + blockS_col*block_size;
    size_t Dbeg = blockD_row*block_size*n + blockD_col*block_size;
    for (size_t i = 0; i < block_size; ++i) {
        for (size_t j = 0; j < block_size; ++j) {
            D[Dbeg + i*n + j] = S[Sbeg + i*n + j];
        }
    }
}
