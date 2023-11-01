
/* Felipe Diniz tomás - RA 110752 */
/* Para compilar: mpicc mpi_sum.c -o mpi_sum ou make */
/* Para executar: mpirun -n <num trabalhadores> ./mpi_sum  */

#include<limits.h>
#include<stdbool.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<errno.h>
#include <mpi.h>


/* Estrutura da lista ligada */

struct Node {
  int value;
  char action;
  struct Node * next;
};

typedef struct Node List;

List * volatile first = NULL, * volatile last = NULL, * volatile aux = NULL;

/* Prototipos de funcao */

void add_to_list(int val, char act);
struct Node * remove_from_list();
void read_file();
void worker(int rank);
void print_result(int num_workers, int rank);
void master(int num_workers);

/* Adiciona a lista */

void add_to_list(int val, char act) {

  aux = (struct Node * ) malloc(sizeof(struct Node));
  aux -> value = val;
  aux -> action = act;
  aux -> next = NULL;

  if (first == NULL) {
    first = aux;
    last = aux;
  } else {
    last -> next = aux;
    last = aux;
  }
}

/* Remove da lista */

struct Node * remove_from_list() {
  aux = first;
  first = first -> next;

  if (first == NULL) {
    last = NULL;
  }

  int val = aux -> value;

  return aux;
}


/* Le arquivo de caso de teste (sera escolhido pelo usuario) */

void read_file() {
  char * fn, action;
  long num;
  char filename[20];
  printf("\nDigite o nome do arquivo:\n");
  scanf("%s", filename);
  FILE * fin = fopen(filename, "r");

  if (fin == NULL) {
    printf("\nERRO: Arquivo nao encontrado ou vazio.\nO arquivo deve estar na mesma pasta que o codigo fonte!\n\n");
    exit(EXIT_FAILURE);
  }

  /* Carrega os numeros e adiciona-os a lista */

  while (fscanf(fin, "%c %ld\n", & action, & num) == 2) {
    if (action == 'p' || action == 'e') {
      add_to_list(num, action);

    } else {
      printf("Acao nao conhecida : '%c'\n", action);
      exit(EXIT_FAILURE);
    }
  }
}

/* Funcao executada pelos trabalhadores*/

void worker(int rank) {

  int number = 1;

  long result[4] = {
    0,
    0,
    LONG_MAX,
    LONG_MIN
  }; //{sum, odd, min, max}

  while (number != 0) {
	
	/* Avisa a master que esta ociosa,
	em seguida recebe numero a ser computado*/
	
    MPI_Send( & rank, 1, MPI_INT, 0, 1, MPI_COMM_WORLD); // MPI_Send: Operação básica de bloqueio de envio.
    
	// MPI_Recev: Recebe uma msg e bloqueia ate que o dado recebido esteja no buffer
	MPI_Recv( & number, 1, MPI_LONG, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    if (number != 0) {
      //update(number);

      /* Simula a computacao */
      sleep(number);

      result[0] += number;

      /* impares */
      if (number % 2 == 1) {
        result[1]++;
      }
      /* Minimo */
      if (number < result[2]) {
        result[2] = number;
      }
      /* Maximo */
      if (number > result[3]) {
        result[3] = number;
      }
    }
  }
  
  /* Envia a master o vetor contendo as
  variaveis atualizadas sum odd min max
  de cada trabalhadora por vez*/

  MPI_Request request = MPI_REQUEST_NULL;
  MPI_Isend( & result, 4, MPI_LONG, 0, 0, MPI_COMM_WORLD, & request);
  // MPI_Send é a versão sem bloqueio de MPI_Send.
  MPI_Wait( & request, MPI_STATUS_IGNORE);
  // MPI_Wait boqueia até que um específico envio não bloquente ou operação de recebimento tenha completado.
  
}

/* Coleta resultados de cada trabalhador
e junta exibindo o resultado*/

void print_result(int num_workers, int rank) {
  
  long final[4] = {0, 0, LONG_MAX, LONG_MIN}; //{sum, odd, min, max}
  
  #define i_max 3
  #define i_min 2
  #define i_odd 1
  #define i_sum 0

  long done = 0;
  long result[4];

  while (num_workers > 0) {

    MPI_Recv( & rank, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Send( & done, 1, MPI_LONG, rank, 0, MPI_COMM_WORLD); // sinaliza que o trabalhador terminou
    MPI_Recv( & result, 4, MPI_LONG, rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // coleta resultado
	
	// junta resultado de cada trabalahdora
	
    final[i_sum] = final[i_sum] + result[i_sum];
    final[i_odd] = final[i_odd] + result[i_odd];

    if (final[i_max] < result[i_max]) {
      final[i_max] = result[i_max];
    }

    if (final[i_min] > result[i_min]) {
      final[i_min] = result[i_min];
    }

    num_workers--;
  }

  printf("%ld %ld %ld %ld\n", final[i_sum], final[i_odd], final[i_min], final[i_max]);
}

/* Funcao executada pela mestre*/

void master(int num_workers) {
  struct Node * aux2 = NULL;
  char action;
  int value;
  int worker_id;

  read_file();
  
  /* Retira da fila e envia para as trabalahdoras*/
  
  while (last != NULL && first != NULL) {
    aux2 = remove_from_list();
    action = aux2 -> action;
    value = aux2 -> value;

    if (action == 'p') {
    /* Recebe o id do trabalhador ocioso
	e envia o valor a ser processado*/
	  MPI_Recv( & worker_id, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      MPI_Send( & value, 1, MPI_LONG, worker_id, 0, MPI_COMM_WORLD);
    } else {
      sleep(value);
    }
  }

  print_result(num_workers, worker_id);

}

/* Funcao Main*/

int main(int argc, char ** argv) {

  int worker_id;
  int size;
  
  /* Inicializa a mestre e trabalhador
  e designa as funcoes para cada uma*/
  
  MPI_Init( & argc, & argv); // Inicializa o ambiente de execução de MPI
  MPI_Comm_size(MPI_COMM_WORLD, & size); // Numero de processos em um grupo associado a um comunicado
  MPI_Comm_rank(MPI_COMM_WORLD, & worker_id); // Associa um rank do processo requisitante ao comunicador

  if (worker_id != 0) {
    worker(worker_id);
  } else {
    master(size - 1);
  }

  MPI_Finalize(); // Termina a execução do ambiente MPI

  return 0;
}
