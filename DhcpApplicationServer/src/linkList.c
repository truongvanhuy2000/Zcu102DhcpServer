#include "linkList.h"
void initList(Node **node)
{
    head = *node;
    
}
void add_node(Node **head, void *data) {
    Node *new_node = (Node *)malloc(sizeof(Node));
    new_node->data = data;
    new_node->next = NULL;

    if (*head == NULL) {
        *head = new_node;
    } else {
        Node *current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_node;
    }
}

void delete_node(Node **head, void *data) {
    Node *current = *head;
    Node *previous = NULL;

    while (current != NULL) {
        if (current->data == data) {
            if (previous == NULL) {
                *head = current->next;
            } else {
                previous->next = current->next;
            }
            free(current);
            break;
        } else {
            previous = current;
            current = current->next;
        }
    }
}