// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class doint3 {
public:
	double x, y, z;
	doint3(double x, double y, double z) : x(x), y(y), z(z) {};

	doint3 operator-(const doint3& other) {
		x -= other.x;
		y -= other.y;
		z -= other.z;
		return *this;
	}

	doint3 operator=(const doint3& other) {
		x = other.x;
		y = other.y;
		z = other.z;
		return *this;
	}

	doint3 to_doint3(const FVector& other) {
		x = other.X;
		y = other.Y;
		z = other.Y;
		return *this;
	}
};

class doint2 {
public:
	double x, y;
	doint2() : x(0.0), y(0.0) {};
	doint2(double x, double y) : x(x), y(y) {};
};

class recd {
public:
	double left, top, right, bottom;
	recd(double left, double top, double right, double bottom) : left(left), top(top), right(right), bottom(bottom) {};
	recd() : left(0.0), top(0.0), right(0.0), bottom(0.0) {};
};

class plane {
public:
	double a, b, c;
	plane(double a, double b, double c) : a(a), b(b), c(c) {};
	plane() : a(0.0), b(0.0), c(0.0) {};
};

class matrix {
public:
	double** mat = 0;
	int row = 0;
	int col = 0;
	matrix(int row, int col) : row(row), col(col) {
		mat = new double* [row];
		for (int i = 0; i < row; i++) {
			mat[i] = new double[col];
		}
	}
	//matrix(double** mat) : mat(mat) {};
	//matrix(const matrix& other) {
	//	mat = other.mat;
	//};
	~matrix() { 
		for (int i = 0; i < row; i++) {
			delete[] mat[i];
		}
		delete[] mat;
	}

	template<typename... Args>
	void set(Args... args) {
		set_at(0, 0, args...);
	}

	template<typename T>
	void set_at(int set_row, int set_col, T val) {
		mat[set_row][set_col] = val;
		set_col++;
		if (set_col >= col) {
			set_row++;
			set_col = 0;
		}
		if (set_row >= row)
			return;
		return;
	}

	template<typename T, typename... Args>
	void set_at(int set_row, int set_col, T val, Args... args) {
		mat[set_row][set_col] = val;
		set_col++;
		if (set_col >= col) {
			set_row++;
			set_col = 0;
		}
		if (set_row >= row)
			return;
		set_at(set_row, set_col, args...);
		return;
	}


	void transpose() {
		matrix out(col, row);
		for (int i = 0; i < row; i++) {
			for (int j = 0; j < col; j++) {
				out.mat[j][i] = mat[i][j];
			}
		}
		for (int i = 0; i < row; i++) {
			delete[] mat[i];
		}
		delete[] mat;
		row = out.row;
		col = out.col;

		mat = new double*[row];
		for (int i = 0; i < row; i++) {
			mat[i] = new double[col];
		}

		for (int i = 0; i < row; i++) {
			for (int j = 0; j < col; j++) {
				mat[i][j] = out.mat[i][j];
			}
		}
		
		return;
	}

	void mult(const matrix& other) {
		matrix out(row, other.col);
		double buffer = 0.0;
		for (int i = 0; i < row; i++) {
			for (int j = 0; j < other.col; j++) {
				for (int k = 0; k < other.row; k++) {
					buffer += mat[i][k] * other.mat[k][j];
				}
				out.mat[i][j] = buffer;
				buffer = 0.0;
			}
		}
		for (int i = 0; i < row; i++) {
			delete[] mat[i];
		}
		delete[] mat;
		row = out.row;
		col = out.col;

		mat = new double*[row];
		for (int i = 0; i < row; i++) {
			mat[i] = new double[col];
		}

		for (int i = 0; i < row; i++) {
			for (int j = 0; j < col; j++) {
				mat[i][j] = out.mat[i][j];
			}
		}

		return;
	}

	double determinant() {
		double det = 0.0;
		for (int i = 0; i < 3; i++)
			det = det + (mat[0][i] * (mat[1][(i + 1) % 3] * mat[2][(i + 2) % 3] - mat[1][(i + 2) % 3] * mat[2][(i + 1) % 3]));
		return det;
	}

	void inverse() {
		matrix out(row, col);
		double det = determinant();
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				out.mat[i][j] = (((mat[(j + 1) % 3][(i + 1) % 3] * mat[(j + 2) % 3][(i + 2) % 3]) - (mat[(j + 1) % 3][(i + 2) % 3] * mat[(j + 2) % 3][(i + 1) % 3])) / det);
			}
		}
		for (int i = 0; i < row; i++) {
			for (int j = 0; j < col; j++) {
				mat[i][j] = out.mat[i][j];
			}
		}

		return;
	}

	matrix& operator=(const matrix& other) {
		if (mat != other.mat) {
			if (row != other.row || col != other.col) {
				for (int i = 0; i < row; i++) {
					delete[] mat[i];
				}
				delete[] mat;
				row = other.row;
				col = other.col;

				mat = new double*[row];
				for (int i = 0; i < row; i++) {
					mat[i] = new double[col];
				}
			}
			for (int i = 0; i < row; i++) {
				for (int j = 0; j < col; j++) {
					mat[i][j] = other.mat[i][j];
				}
			}
		}
		return *this;
	}

	/*matrix operator*(const matrix& other) {
		matrix out(row, other.col);
		for (int i = 0; i < row; i++) {
			for (int j = 0; j < other.col; j++) {
				for (int k = 0; k < other.row; k++) {
					out.mat[i][j] += mat[i][k] * other.mat[k][j];
				}
			}
		}
		return out;
	}*/
};


class ELEMENTSVR_API gc_doint {
public:
	gc_doint();
	~gc_doint();
};
