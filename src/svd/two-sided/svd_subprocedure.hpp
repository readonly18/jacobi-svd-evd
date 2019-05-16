#pragma once
#include <cstdlib>

size_t svd_subprocedure(struct matrix_t Bmat, struct matrix_t Umat, struct matrix_t Vmat);
size_t svd_subprocedure_vectorized(struct matrix_t Bmat, struct matrix_t Umat, struct matrix_t Vmat);
size_t svd_subprocedure_vectorized_with_transpose(struct matrix_t Bmat, struct matrix_t Umat, struct matrix_t Vmat);
