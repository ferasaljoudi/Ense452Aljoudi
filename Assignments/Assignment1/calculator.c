#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>

// I defined a small tolerance for floating point comparisons to
// eliminate the warning: "conversion to 'char' from 'int' may alter its value [-Wconversion]"
#define EPSILON 1e-6

// Defining the Complex with real and imag
typedef struct {
    double real;
    double imag;
} Complex;

// Prototypes for add, subtract, multiply, divide, print and error
void add(Complex z1, Complex z2, Complex* result);
void subtract(Complex z1, Complex z2, Complex* result);
void multiply(Complex z1, Complex z2, Complex* result);
void divide(Complex z1, Complex z2, Complex* result);
void printComplex(Complex z);
void handleError(int errorCode);

int main() {
    char command[100];
    Complex z1, z2, result;
    char op;
    
    fprintf(stderr, "---------------------------------------------------------------------\n");
    fprintf(stderr, "Complex calculator\n");
    fprintf(stderr, "Type a letter to specify the arithmetic operator: (A, S, M, D)\n");
    fprintf(stderr, "A for Add, S for Subtract, M for Multiply, and D for Divide)\n");
    fprintf(stderr, "followed by two complex numbers expressed as pairs of doubles.\n");
    fprintf(stderr, "Type Q to quit.\n");
    fprintf(stderr, "---------------------------------------------------------------------\n");
    
    while (1) {
        fprintf(stderr, "Enter exp: ");
        // Checking if fgets reads input successfully or not
        if (fgets(command, 100, stdin) == NULL) continue;
        
        // Ignoring empty or whitespace lines
        if (sscanf(command, " %c", &op) != 1) continue;

        // Converting to uppercase to make it case insensitive
        op = (char)toupper((unsigned char)op);
        
        // Quitting if it was Q
        if (op == 'Q') {
            fprintf(stderr, "---------------------------------------------------------------------\n");
            break;
        }

        // Storing the inputs values (excluding the operator) and checking for extra arguments
        char extraArgs[100];
        int argsRead = sscanf(command, " %*c %lf %lf %lf %lf %s", &z1.real, &z1.imag, &z2.real, &z2.imag, extraArgs);
        
        // Missing arguments if less than 4
        if (argsRead < 4) {
            handleError(2);
            continue;
        // Extra arguments if more than 4
        } else if (argsRead > 4) {
            handleError(3);
            continue;
        }

        // Checking which operation is to be done
        switch (op) {
            case 'A':
                add(z1, z2, &result);
                printComplex(result);
                break;
            case 'S':
                subtract(z1, z2, &result);
                printComplex(result);
                break;
            case 'M':
                multiply(z1, z2, &result);
                printComplex(result);
                break;
            case 'D':
                // Checking the division by zero
                if (fabs(z2.real) < EPSILON && fabs(z2.imag) < EPSILON) {
                    handleError(4);
                } else {
                    divide(z1, z2, &result);
                    printComplex(result);
                }
                break;
            default:
                // Illegal command
                handleError(1);
                break;
        }
    }
    
    return 0;
}

// Adding
void add(Complex z1, Complex z2, Complex* result) {
    result->real = z1.real + z2.real;
    result->imag = z1.imag + z2.imag;
}

// Subtracting
void subtract(Complex z1, Complex z2, Complex* result) {
    result->real = z1.real - z2.real;
    result->imag = z1.imag - z2.imag;
}

// Multiplying
void multiply(Complex z1, Complex z2, Complex* result) {
    result->real = z1.real * z2.real - z1.imag * z2.imag;
    result->imag = z1.real * z2.imag + z1.imag * z2.real;
}

// Dividing
void divide(Complex z1, Complex z2, Complex* result) {
    double denominator = z2.real * z2.real + z2.imag * z2.imag;
    result->real = (z1.real * z2.real + z1.imag * z2.imag) / denominator;
    result->imag = (z1.imag * z2.real - z1.real * z2.imag) / denominator;
}

void printComplex(Complex z) {
    // I replaced the below line:
    // printf("%.3f + j %.3f\n", z.real, z.imag);
    // with the below if statements to print the decimals only if the result is not whole number

    if (fabs(z.real - round(z.real)) < EPSILON && fabs(z.imag - round(z.imag)) < EPSILON) {
        // Both real and imag are whole numbers
        printf("%.0f + j %.0f\n", z.real, z.imag);
    } else if (fabs(z.real - round(z.real)) < EPSILON) {
        // Only real part is a whole number
        printf("%.0f + j %.6f\n", z.real, z.imag);
    } else if (fabs(z.imag - round(z.imag)) < EPSILON) {
        // Only imag part is a whole number
        printf("%.6f + j %.0f\n", z.real, z.imag);
    } else {
        // Neither of them are whole numbers
        printf("%.6f + j %.6f\n", z.real, z.imag);
    }
}

void handleError(int errorCode) {
    switch (errorCode) {
        case 1:
            printf("error code: 1: Illegal command\n");
            break;
        case 2:
            printf("error code: 2: Missing arguments\n");
            break;
        case 3:
            printf("error code: 3: Extra arguments\n");
            break;
        case 4:
            printf("error code: 4: Divide by zero\n");
            break;
        default:
            printf("Unknown error\n");
    }
}