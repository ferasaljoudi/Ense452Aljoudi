# Complex Number Calculator in C

<div align="center">

This assessment is about implementing a program that performs basic arithmetic operations (addition, subtraction, multiplication, and division) on complex numbers in C.

</div>

## Important notes

### If you are running it on Snoopy:
Make sure you include `-lm` while compiling to explicitly link the math library. Look at the example below:

```sh
gcc calculator.c -o calculator -lm
```

### To check any warning:

Use the below command when combiling:

```sh
gcc calculator.c -o calculator -Wextra -Wall -Wfloat-equal -Wconversion -Wparentheses -pedantic -Wunused-parameter -Wunused-variable -Wreturn-type -Wunused-function -Wredundant-decls -Wunused-value -Wswitch-default -Wuninitialized -Winit-self -Werror

```

Again if you are using Snoopy, you may want to include `-lm` as below:

```sh
gcc calculator.c -o calculator -lm -Wextra -Wall -Wfloat-equal -Wconversion -Wparentheses -pedantic -Wunused-parameter -Wunused-variable -Wreturn-type -Wunused-function -Wredundant-decls -Wunused-value -Wswitch-default -Wuninitialized -Winit-self -Werror

```

### To test the program, run the below command after combiling:

```sh
./runtests.py ./calculator 
```