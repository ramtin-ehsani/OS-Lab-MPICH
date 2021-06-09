#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
  // Initialize the MPI environment
  MPI_Init(NULL, NULL);
  // Find out rank, size
  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  // We are assuming at least 3 processes for this task
  if (world_size < 3) {
    fprintf(stderr, "World size must be greater than 2 for %s\n", argv[0]);
    MPI_Abort(MPI_COMM_WORLD, 1);
  }

  int input[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  if (world_rank == 0) {
    //provide data for world1
    int a[5];
    for(int i=0; i< 5; i++) {
    	a[i] = input[i];
    }
    //send the first half to world1
    MPI_Send(a, 5, MPI_INT, 1, 0, MPI_COMM_WORLD);

    //provide data for world2
    int b[5];
    for(int i=0; i<5; i++) {
    	b[i] = input[i+5];
    }
    //send the second half to world2
    MPI_Send(b, 5, MPI_INT, 2, 0, MPI_COMM_WORLD);

    //recieve world1 result
    int world1_result;
    MPI_Recv(&world1_result, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    //recieve world2 result
    int world2_result;
    MPI_Recv(&world2_result, 1, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    int result;
    result = world1_result + world2_result;

    printf("result is: %d\n", result);
  } else {
    //recieve my_data from world0
    int my_data[5];
    MPI_Recv(my_data, 5, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    //calculations
    int my_result = 0;
    for(int i=0; i<5; i++) {
    	my_result += my_data[i];
    }

    printf("I\'m world: %d, my_result is: %d, sending it back to world0\n", world_rank, my_result);

    //sending data back to world0
    MPI_Send(&my_result, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

  }
  MPI_Finalize();
}