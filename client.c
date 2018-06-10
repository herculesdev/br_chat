 /**
  * Autor: Hércules M.
  * Data: 08/06/2018
  **/
 
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h> // struct sockaddr_in
#include <arpa/inet.h> // inet_ntoa()
#include <pthread.h>

#define PORT 8080
#define BUF_SIZE 1024
#define EXIT_STRING "#quit"

/**
 * Recebe e processa mensagens do servidor
 * É executado numa thread até que não seja
 * mais possível receber mensagens do servidor
 **/
void* process_message(void* arg)
{
    char buf[BUF_SIZE];
    int *sock = (int *) arg;
    int size = 0;
    while(true)
    {
        size = recv(*sock, buf, sizeof(buf), 0);  
        if(size <= 0)
            continue; 
        printf("%s\n", buf);
    }
    return NULL;
}

void clear_buffer()
{
    char c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int main(int argc, char* argv[])
{
    // Entrada
    char IP[32];
    char nickname[32];

    printf("Entre com o IP do servidor: ");
    scanf("%s", IP);

    printf("Digite um nome: ");
    scanf("%s", nickname);

    // Informações para conexão com o servidor
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(IP);
    server_address.sin_port = htons(PORT);

    // Socket
    printf("# Inicializando socket...\n");
    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if(sock < 0)
    {
        perror("Falha ao inicializar socket");
        return -1;
    }

    // Conexão
    printf("# Conectando-se a %s\n", inet_ntoa(server_address.sin_addr));
    int connRes = connect(sock, (struct sockaddr*)&server_address, sizeof(server_address));
    if(connRes < 0)
    {
        perror("Falha ao conectar-se");
        return -1;
    }

    // Thread que processará as mensagens do servidor
    pthread_t th_proc_msg;
    pthread_create(&th_proc_msg, NULL, process_message, (void*)&sock);

    // Loop de envio de mensagens
    char message[BUF_SIZE];
    while(true)
    {
        clear_buffer();
        scanf("%[^\n]", message);
        int sendRes = send(sock, message, sizeof(message), 0);

        if(strcmp(message, EXIT_STRING) == 0)
            return 0;

        if(sendRes < 0)
        {
            perror("Falha ao enviar mensagem");
            return -1;
        }
    }

    return 0;
}