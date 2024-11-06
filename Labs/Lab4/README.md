# Lab 4

## Timing tests:

### Optimization level: None (-O0):

- 32-bit Addition: 599 ticks
- 64-bit Addition: 585 ticks
- 32-bit Multiplication: 598 ticks
- 64-bit Multiplication: 576 ticks
- 32-bit Division: 575 ticks
- 64-bit Division: 436 ticks
- 8-byte Struct Copy: 608 ticks
- 128-byte Struct Copy: 100 ticks
- 1024-byte Struct Copy: 93 ticks


### Optimization level: Optimize for Debug (-Og):

- 32-bit Addition: 614 ticks
- 64-bit Addition: 605 ticks
- 32-bit Multiplication: 612 ticks
- 64-bit Multiplication: 593 ticks
- 32-bit Division: 613 ticks
- 64-bit Division: 461 ticks
- 8-byte Struct Copy: 633 ticks
- 128-byte Struct Copy: 654 ticks
- 1024-byte Struct Copy: 408 ticks

### Optimization level: Optimize (-O1):

- 32-bit Addition: 616 ticks
- 64-bit Addition: 604 ticks
- 32-bit Multiplication: 613 ticks
- 64-bit Multiplication: 597 ticks
- 32-bit Division: 613 ticks
- 64-bit Division: 461 ticks
- 8-byte Struct Copy: 634 ticks
- 128-byte Struct Copy: 634 ticks
- 1024-byte Struct Copy: 636 ticks

### Optimization level: Optimize more (-O2):

- 32-bit Addition: 641 ticks
- 64-bit Addition: 637 ticks
- 32-bit Multiplication: 643 ticks
- 64-bit Multiplication: 633 ticks
- 32-bit Division: 638 ticks
- 64-bit Division: 509 ticks
- 8-byte Struct Copy: 655 ticks
- 128-byte Struct Copy: 655 ticks
- 1024-byte Struct Copy: 655 ticks

### Optimization level: Optimize most (-O3):

- 32-bit Addition: 641 ticks
- 64-bit Addition: 636 ticks
- 32-bit Multiplication: 643 ticks
- 64-bit Multiplication: 632 ticks
- 32-bit Division: 634 ticks
- 64-bit Division: 509 ticks
- 8-byte Struct Copy: 655 ticks
- 128-byte Struct Copy: 655 ticks
- 1024-byte Struct Copy: 655 ticks

### Optimization level: Optimize for size (-Os):

- 32-bit Addition: 617 ticks
- 64-bit Addition: 602 ticks
- 32-bit Multiplication: 617 ticks
- 64-bit Multiplication: 596 ticks
- 32-bit Division: 611 ticks
- 64-bit Division: 463 ticks
- 8-byte Struct Copy: 634 ticks
- 128-byte Struct Copy: 636 ticks
- 1024-byte Struct Copy: 634 ticks

### Optimization level: Optimize for speed (-Ofast):

- 32-bit Addition: 640 ticks
- 64-bit Addition: 636 ticks
- 32-bit Multiplication: 643 ticks
- 64-bit Multiplication: 632 ticks
- 32-bit Division: 634 ticks
- 64-bit Division: 509 ticks
- 8-byte Struct Copy: 655 ticks
- 128-byte Struct Copy: 655 ticks
- 1024-byte Struct Copy: 655 ticks

### Optimization level: Optimize more for size (-Oz):

- 32-bit Addition: 617 ticks
- 64-bit Addition: 602 ticks
- 32-bit Multiplication: 617 ticks
- 64-bit Multiplication: 595 ticks
- 32-bit Division: 611 ticks
- 64-bit Division: 463 ticks
- 8-byte Struct Copy: 634 ticks
- 128-byte Struct Copy: 636 ticks
- 1024-byte Struct Copy: 634 ticks
