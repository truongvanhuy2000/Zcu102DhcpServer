#ifndef LINK_LIST
#define LINK_LIST
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct node {
    void *data;
    struct node *next;
} Node;

struct node *head = NULL;
struct node *current = NULL;

void initList(Node **node);
#endif