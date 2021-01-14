# Two-Phase Simplex
This project implement two-phase simplex algorithm.

The standard form of simplex is positive RHS and '<' sign.
It can process negative RHS, '>' and '=' sign.

If ths the sign is '>' or '<', then the constraint need to add slack variables, and

if the sign is '>' or '=', then the constraint need to add artificial variables.

# Format
Input file is mps file, and the objective function is minimize.

# Use
Using command line input,
For Example,
./simplex SHIP12L
(didn't contain .mps)

or defalut mode 
Select testbench :
1 : AFIRO
2 : SC105
3 : STOCFOR1
4 : SCAGR7
5 : LOTFI
6 : Largest Testbench MAROS-R7

The other mps file can download from the following URL:

https://github.com/YimingYAN/LP-Test-Problems/tree/master/MPS_Files

NOTE: Do NOT use the testbench with BR index for normal testing
