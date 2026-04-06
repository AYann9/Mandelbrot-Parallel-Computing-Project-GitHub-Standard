# Mandelbrot Set Parallel Computing
CPU (OpenMP) & GPU (OpenCL) Parallel Computation and Performance Comparison

## Overview
This project implements parallel computation of the Mandelbrot set using:
- Multi-core CPU parallelism with OpenMP
- Massively parallel GPU computing with OpenCL
- Compare execution time, speedup, and output consistency between serial and parallel versions

## Key Results
- CPU Parallel: 0.038s | Serial: 0.388s | Speedup ≈ 10x
- GPU Parallel: 0.009s | Serial: 13.395s | Consistent output, no noise

## Quick Start
### CPU Parallel (OpenMP)
```bash
cd cpu
gcc -fopenmp -o mandelbrot mandelbrot.cpp
./mandelbrot
