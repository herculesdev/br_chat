 /**
  * Autor: Hércules M.
  * Data: 08/06/2018
  **/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h> // struct sockaddr_in
#include <arpa/inet.h> // inet_ntoa()
#include <pthread.h>
#include "cli_list.h"

#define PORT 8080
#define MAX_CLIENTS 100
#define BUF_SIZE 1024
#define EXIT_STRING "#quit"
// Globais
clist_t *clist;
int count = 0;

void send_message(char *str, int id_sender)
{
    int i;
    for(i = 0; i < clist->length; i++)
    {
        if(clist->clients[i].id != id_sender)
        {
            send(clist->clients[i].connfd, str, BUF_SIZE, 0);
        }
    }
}

/*
* Processa as mensagens recebidas do client através da função recv()
* É executado em uma thread.
* Para cada client conectado, um fork da thread é criado
*/
void* process_message(void* arg)
{
    client_t *cli = (client_t*) arg;
    char buffer[BUF_SIZE];
    char message[BUF_SIZE];

    while(true)
    {
        int size = recv(cli->connfd, buffer, sizeof(buffer), 0);

        if(size <= 0)
        {
            printf("Deconectado: %s\n", inet_ntoa(cli->address.sin_addr));
            close(cli->connfd);
            cli_list_remove(clist, cli->id);
            return NULL;
        }

        if(strcmp(buffer, EXIT_STRING) == 0)
        {
            printf("Deconectado: %s\n", inet_ntoa(cli->address.sin_addr));
            close(cli->connfd);
            cli_list_remove(clist, cli->id);
            return NULL;
        }

        if(strlen(buffer) <= 0)
            continue;

        sprintf(message, "%s diz: %s", cli->nickname, buffer);
        printf("%s diz: %s\n", cli->nickname, buffer);
        send_message(message, cli->id);
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    // Informações do servidor
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(PORT);

    // Socket
    int sock = -1;
    
    printf("# Inicializando socket...\n");
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

    if(sock < 0)
    {
        perror("Falha ao inicializar o socket");
        return -1;
    }

    printf("# Associando informações ao socket\n");
    int bind_res = bind(sock, (struct sockaddr*)&server_addr, sizeof(server_addr));

    if(bind_res < 0)
    {
        perror("Falha ao associar informações ao socket");
        return -1;
    }

    printf("# Iniciando escuta na porta %d ...\n", PORT);
    int listenRes = listen(sock, MAX_CLIENTS);
    
    if(listenRes < 0)
    {
        perror("Falha ao iniciar escuta");
        return -1;
    }

    // aceita conexões
    printf("SERVIDOR INICIALIZADO\n");
    clist = cli_list_create();// TAD do tipo Lista (armazena client_t)
    pthread_t th_proc_msg;
    while(true)
    {
        int connfd; // descritor de conexão do cleinte (connetion file descriptor)
        struct sockaddr_in client_addr; // informações do cliente (IP, porta etc..)
        int c_addr_len = sizeof(client_addr);

        // Aceita a conexão
        connfd = accept(sock, (struct sockaddr*)&client_addr, &c_addr_len);

        // Conexão aceitada com sucesso?
        if(connfd < 0)
        {
            printf("Falha.\n");
            continue;
        }

        // Verifica se o número de clients conectados atingiu o limite
        if(cli_list_count(clist) > MAX_CLIENTS)
        {
            printf("Conexão rejeitada: capacidade máxima atingida.\n");
            continue;
        }

        // Objeto do tipo "client" recebe as informações do client
        client_t cli;
        cli.address = client_addr;
        cli.connfd = connfd;
        cli.id = count;
        sprintf(cli.nickname, "%d", cli.id);

        // Adiciona o objeto client em uma lista
        cli_list_add(clist, cli);

        // Cria thread para processar mensagens do client
        pthread_create(&th_proc_msg, NULL, process_message, (void*)cli_list_get(clist, count));

        printf("Nova conexão: %s, ID: %d\n", inet_ntoa(client_addr.sin_addr), count);

        count++;
    }

    return 0;
}