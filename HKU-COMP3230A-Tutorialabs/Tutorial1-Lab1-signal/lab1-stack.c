// file : lab1 - stack .c
# include <stdio.h>
# include <stdlib.h>

int* initialize_array ( int n){
    int stack_arr [n];
    for (int i = 0; i < n; i++) {
        stack_arr [i] = 0;
    }
    // Note : The following return is problematic
    // because stack_arr is a local variable and will be deallocated once the function returns.

    // 由于stack_arr是一个局部变量，它将在函数返回后被自动释放。因此，返回指向这个局部变量的指针是错误的，会导致未定义的行为。
    return stack_arr ;
}

int main ( int argc , char * argv []) {
    int *arr; // Pointer for our dynamically allocated array
    // Check if the command line argument is provided
    if ( argc != 2) {
        printf (" Usage : %s <number_of_elements >\n", argv [0]) ;
        return 1; // Exit with an error code
    }
    // Convert the command line argument to an integer
    int n = atoi ( argv [1]) ;
    arr = initialize_array (n);
    for (int i=0; i<n; i++){
        printf ("%d\t", arr[i]);
    }
    return 0;
}