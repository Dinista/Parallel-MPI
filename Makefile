CC=mpicc

default: mpi_sum

par_sum: mpi_sum.c
	mpicc mpi_sum.c -o mpi_sum 

clean:
	rm -f mpi_sum
