#include "cli_list.h"

clist_t* cli_list_create()
{
    clist_t *p = (clist_t*) malloc(sizeof(clist_t));
    return p;
}

void cli_list_destroy(clist_t *list)
{
    free(list);
    list = NULL;
}

int cli_list_count(clist_t *list){
    if(list == NULL)
        return -1;

    return list->length;
}

bool cli_list_add(clist_t *list, client_t client)
{
    if(list == NULL)
        return false;

    if(list->length == MAX_CLIENTS)
        return false;

    if(list->length == 0)
    {
        list->clients[0] = client;
        list->length++;
        return true;
    }

    int len = list->length;
    list->clients[len] = client;
    list->length++;
    return true;
}

bool cli_list_remove(clist_t *list, int id)
{
    if(list == NULL)
        return false;

    if(list->length == 0)
        return false;

    int i;
    for(i = 0; i<list->length; i++)
    {
        if(list->clients[i].id == id)
        {
            // reloca todos clients posteriores em -1 posição
            int j;
            for(j = i; j<list->length-1; j++)
            {
                list->clients[j] = list->clients[j+1];
            }
            list->length--;
            return true;
        }
    }

    return false;
}

client_t * cli_list_get(clist_t *list, int id)
{
    if(list == NULL)
        return NULL;

    if(list->length == 0)
        return NULL;

    int i;
    for(i = 0; i<list->length; i++)
    {
        if(list->clients[i].id == id)
        {
            return &list->clients[i];
        }
    }

    return NULL;
}
