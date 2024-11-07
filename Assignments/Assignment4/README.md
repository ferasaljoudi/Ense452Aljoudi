<div align="center">

# Assignment 4 LUT

</div>

- <b>Processor:</b> 11th Gen Intel(R) Core(TM) i5-1135G7 @ 2.40 GHz
- <b>Date:</b> Nov. 06, 2024
- <b>Compiler:</b> gcc (Ubuntu 9.4.0-1ubuntu1~20.04.2) 9.4.0
- <b>Measurement:</b> using clock_gettime() and `-O0` Switches with 1e9 loops:

```sh
gcc -o main main.c -lm
```

<div align="center">

| Function       | Speed         |
| -------------- | ------------- |
| Table lookup   | 2539572577 ns |
| Built in sin() | 2256732633 ns |
| Table Speedup  | 0.8886x       |

</div>

- <b>Measurement:</b> using clock_gettime() and `-O3` Switches with 1e9 loops:

```sh
gcc -o main main.c -lm -O3
```

<div align="center">

| Function       | Speed   |
| -------------- | ------- |
| Table lookup   | 2216 ns |
| Built in sin() | 523 ns  |
| Table Speedup  | 0.2360x |

</div>
