#include <netinet/in.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define MAX_CLIENTS 100

 // Estruturas
typedef struct client
{
    int id; // identificador único
    int connfd; // Descritor de conexão (socket)
    struct sockaddr_in address; // informações do client (IP, porta etc..)
    char nickname[32]; // apelido no bate-papo
} client_t;

typedef struct client_list
{
    client_t clients[MAX_CLIENTS];
    int length;  
}clist_t;

// Funções de manipulação da lista estática
clist_t* cli_list_create();
void cli_list_destroy(clist_t *list);
int cli_list_count(clist_t *list);
bool cli_list_add(clist_t *list, client_t client);
bool cli_list_remove(clist_t *list, int id);
client_t * cli_list_get(clist_t *list, int id);

// Outras
void* process_message(void* arg);