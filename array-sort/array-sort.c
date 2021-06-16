#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

void swap(int *xp, int *yp)
{
    int temp = *xp;
    *xp = *yp;
    *yp = temp;
}

void sort(int arr[], int n)
{
   int i, j;
   for (i = 0; i < n-1; i++)     
 
       for (j = 0; j < n-i-1; j++)
           if (arr[j] > arr[j+1])
              swap(&arr[j], &arr[j+1]);
}
void merge(int arr1[], int arr2[], int n1, int n2, int arr3[])
{
  int i = 0, j = 0, k = 0;
 
    while (i<n1 && j <n2)
    {
        if (arr1[i] < arr2[j])
            arr3[k++] = arr1[i++];
        else
            arr3[k++] = arr2[j++];
    }
 
    // Store remaining elements of first array
    while (i < n1)
        arr3[k++] = arr1[i++];
 
    // Store remaining elements of second array
    while (j < n2)
        arr3[k++] = arr2[j++];
}

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

  int input[10] = {5, 8, 6, 9, 7, 0, 2, 1, 3, 4};
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
    int world1_result[5];
    MPI_Recv(world1_result, 5, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    //recieve world2 result
    int world2_result[5];
    MPI_Recv(world2_result, 5, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    int result[10];
    merge(world1_result, world2_result , 5, 5, result);

    printf("world0, final result is:\n");
    for (int i = 0; i < 10; i++)
      printf("%d ", result[i]);
    printf("\n");

  } else {
    //recieve my_data from world0
    int my_data[5];
    MPI_Recv(my_data, 5, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    //calculations
    sort(my_data,5);

    printf("I\'m world: %d, my_result is:\n",world_rank);
    for (int i = 0; i < 5; i++){
      printf("%d ", my_data[i]); 
    }
    printf("\n");
    printf("sending it back to world0\n");

    //sending data back to world0
    MPI_Send(my_data, 5, MPI_INT, 0, 0, MPI_COMM_WORLD);

  }
  MPI_Finalize();
}