/*
 * main.cpp
 * 
 * Author: Dylan Turner and Igor Minevich
 * Based On: NxN matrix by Dylan Turner
 * Created: Tues,Dec 5
 *
 * This program solves the lights out puzzle for any size array
 */

#include <iostream>
#include <cstdlib>

using namespace std;

/* Calculation functions */
void gen_eq_matrix(bool**, int, int); // Generate the matrix for the equations
void add_row(bool**, int, int, int); // Field 2 add one row to another
void swap_row(bool**, int, int, int); // Swap 2 rows in matrix
void eliminate(bool**, int, int); // Gaussian elimination of the array
void reduce(bool**, int, int, int); // Gauss-Jordan elimination on the REF array
bool check_zero_row(bool**, int, int); // Check if a row has zero
int  free_vars(bool**, int, int); // Get the number of solutions, 0, 1, or more for any solution
void copy_solution(bool**, bool**, int, int, int); // Copy solution from first into actual solution array

// Extra functions
void print_arr(int**, int, int);
void print_arr(bool**, int, int);

int main(int argc, char* argv[]) {
    if(argc < 3) {
        cerr << "Input a game size!!" << endl;
        return -1;
    }
    
    int m = atoi(argv[1]); // The number of rows in the puzzle
    int n = atoi(argv[2]); // The number of columns
    
    // Declare rays for use
    bool** solution    = new bool*[m];
    bool** calc_matrix = new bool*[m * n];
    for(int i = 0; i < m; i++)
        solution[i] = new bool[n];
    for(int i =0; i < m * n; i++)
        calc_matrix[i] = new bool[m * n + 1];
    
    gen_eq_matrix(calc_matrix, m, n);
//    print_arr(calc_matrix, m * n, m * n + 1);
//    cout << endl;
    
    eliminate(calc_matrix, m, n);
//    print_arr(calc_matrix, m * n, m * n + 1);
//    cout << endl;
    
    int num_free_vars = free_vars(calc_matrix, m * n, m * n + 1);
//    cout << "There are " << num_free_vars << " free variables." << endl << endl;
    
    reduce(calc_matrix, m, n, num_free_vars);
//    print_arr(calc_matrix, m * n, m * n + 1);
//    cout << endl;
    
    copy_solution(calc_matrix, solution, m, n, num_free_vars);
    print_arr(solution, m, n);
    cout << endl;
    
    free(calc_matrix);
    free(solution);
    
    return 0;
}

// Create a matrix for the system of equations
void gen_eq_matrix(bool** calc_matrix, int m, int n) {
    for(int i = 0; i < m; i++) {
        for(int j = 0; j < n; j++) {
            int row = n * i + j;
            calc_matrix[row][row] = 1;
            
            if(i + 1 < m)
                calc_matrix[row][n * (i + 1) + j] = 1;
            if(i - 1 >= 0)
                calc_matrix[row][n * (i - 1) + j] = 1;
            
            if(j + 1 < n)
                calc_matrix[row][n * i + j + 1] = 1;
            if(j - 1 >= 0)
                calc_matrix[row][n * i + j - 1] = 1;
        
            calc_matrix[row][m * n] = 1;
        }
    }
}

// Perform Gaussian elimination
void eliminate(bool** calc_matrix, int m, int n) {
    for(int row = 0; row < m * n; row++) { // Go through each row
        // If not already empty
        if(!check_zero_row(calc_matrix, row, m * n + 1)) {
            // Get a one in the correct position
            int i = row + 1;
            while(!calc_matrix[row][row]) {
                swap_row(calc_matrix, row, i, m * n + 1);
                i++;
            }

            // Get zeroes below
            for(i = row + 1; i < m * n; i++) {
                if(calc_matrix[i][row]) // If there's a one
                    add_row(calc_matrix, i, row, m * n + 1); // Remove it

                /*print_arr(calc_matrix, m * n, m * n + 1);
                cout << endl;*/
            }
        }
    }
}

void reduce(bool** calc_matrix, int m, int n, int num_free) {
    int row = m * n - num_free - 1;
    while(row >= 0) {
        int i = row - 1;
        while(i >= 0) {
            if(calc_matrix[i][row]) // remove top above non free
                add_row(calc_matrix, i, row, m * n + 1);

            i--;
        }
        
        row--;
    }
}

// Create the solution matrix
void copy_solution(bool** calc_matrix, bool** solution, int m, int n, int num_free) {
    if(num_free == -1) {// no solution
        cerr << "No solution!" << endl;
        exit(1);
    } else { // Solution should already be there from reduce - let free variables be equal to 0
        for(int row = 0; row < m; row++)
            for(int col = 0; col < n; col++) 
                solution[row][col] = calc_matrix[row * n + col][m * n];
    }
}
            
// Switch two rows
void swap_row(bool** calc_matrix, int row1, int row2, int n) {
    int first_val = 0;
    for(int col = 0; col < n; col++) {
        first_val = calc_matrix[row1][col];
        calc_matrix[row1][col] = calc_matrix[row2][col];
        calc_matrix[row2][col] = first_val;
    }
}
            
// Add row2 into row1
void add_row(bool** calc_matrix, int row1, int row2, int n) {
    for(int col = 0; col < n; col++)
        calc_matrix[row1][col] ^= calc_matrix[row2][col];
}

// Check if row is a zero row
bool check_zero_row(bool** calc_matrix, int row, int n) {
    for(int col = 0; col < n; col++)
        if(calc_matrix[row][col])
            return false;
    return true;
}

// Get the solutions
int free_vars(bool** arr, int m, int n) {
    int i = 0;
    // Check for zero rows
    while(i < m && !check_zero_row(arr, i, n))
        i++;
    if(i == m - 1) // no zero rows
        return 0;
    
    int counter = 0;
    // Now count how many solutions there are and check for 1=0's 
    while(i < m) {
        //cout << i << " is less than " << m << endl;
        
        if(!check_zero_row(arr, i, n))
            return -1;
        else
            counter ++;
        
        i++;
    }
    
    return counter;
}

void print_arr(int** arr, int num_rows, int num_cols) {
    for(int i = 0; i < num_rows; i++) {
        for(int j = 0; j < num_cols; j++) {
            cout << arr[i][j] << " ";
        }
        
        cout << endl;
    }
}

void print_arr(bool** arr, int num_rows, int num_cols) {
    for(int i = 0; i < num_rows; i++) {
        for(int j = 0; j < num_cols; j++) {
            cout << arr[i][j] << " ";
        }
        
        cout << endl;
    }
}
