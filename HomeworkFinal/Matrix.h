#pragma once

#include <iostream>

class Matrix {
public:
    Matrix(int, int);
    Matrix(double **, int, int);
    Matrix();
    ~Matrix();
    Matrix(const Matrix &);
    Matrix &operator=(const Matrix &);

    inline double& operator()(int x, int y) { return p[x][y]; }

    Matrix &operator+=(const Matrix &);
    Matrix &operator-=(const Matrix &);
    Matrix &operator*=(const Matrix &);
    Matrix &operator*=(double);
    Matrix &operator/=(double);
    Matrix operator^(int);

    friend std::ostream &operator<<(std::ostream &, const Matrix &);
    friend std::istream &operator>>(std::istream &, Matrix &);

    void swap_rows(int, int);
    Matrix transpose();

    static Matrix create_identity(int);
    static Matrix solve(Matrix, Matrix);
    static Matrix band_solve(Matrix, Matrix, int);

    // functions on vectors
    static double dot_product(Matrix, Matrix);

    // functions on augmented matrices
    static Matrix augment(Matrix, Matrix);
    Matrix gaussian_eliminate();
    Matrix row_reduce_from_gaussian();
    void read_solutions_from_rref(std::ostream &os);
    Matrix inverse();

private:
    int rows_, cols_;
    double **p;

    void alloc_space();
    Matrix exp_helper(const Matrix &, int);
};

Matrix operator+(const Matrix &, const Matrix &);
Matrix operator-(const Matrix &, const Matrix &);
Matrix operator*(const Matrix &, const Matrix &);
Matrix operator*(const Matrix &, double);
Matrix operator*(double, const Matrix &);
Matrix operator/(const Matrix &, double);
