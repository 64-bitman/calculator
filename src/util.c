#include "util.h"
#include <stdio.h>
#include <stdlib.h>

void node_print(Node *target, int newline) {
    if (target == NULL) {
        printf("cannot print NULL node\n");
        return;
    }
    char nl = (newline) ? '\n' : ' ';
    switch (target->value_type) {
        case TYPE_NUMBER:
            printf("%Lf%c", target->number, nl);
            break;
        case TYPE_OPERATOR:
            printf("%c%c", target->operator, nl);
            break;
        case TYPE_PAREN:
            printf("%c%c", target->paren, nl);
            break;
        default:
            printf("node has no declared value type\n");
    }
}

Node *node_create(long double value, enum NodeType type) {
    Node *new = malloc(sizeof(*new));

    if (new == NULL) {
        perror("error:");
        abort();
    }

    new->next = NULL;
    new->previous = NULL;

    switch (type) {
        case TYPE_NUMBER:
            new->number = value;
            break;
        case TYPE_OPERATOR:
            new->operator=(char) value;
            break;
        case TYPE_PAREN:
            new->paren = (char)value;
            break;
        default:
            printf("error: no matching NodeType\n");
            return NULL;
    }
    new->value_type = type;

    return new;
}

Node *node_insert(Node *target, Node *previous, Node *next) {
    if (previous != NULL) {
        previous->next = target;
    }
    if (next != NULL) {
        next->previous = target;
    }
    target->previous = previous;
    target->next = next;

    return target;
}

Node *node_remove(Node *target) {
    if (target == NULL) {
        printf("error: cannot remove NULL node\n");
        return NULL;
    }
    Node *previous = target->previous, *next = target->next;

    if (previous != NULL) {
        previous->next = next;
    }
    if (next != NULL) {
        next->previous = previous;
    }
    target->next = NULL;
    target->previous = NULL;

    return target;
}

void node_free(Node *target) {
    if (target != NULL) {
        Node *prev_target = target->previous;

        while (target != NULL) {
            Node *next = target->next;
            free(target);
            target = next;
        }

        target = prev_target;
        while (target != NULL) {
            Node *previous = target->previous;
            free(target);
            target = previous;
        }
    }
}

Stack *stack_create(Node *head) {
    Stack *new = malloc(sizeof(*new));

    if (new == NULL) {
        perror("error:");
        abort();
    }
    new->top = head;

    return new;
}

void stack_free(Stack *target) {
    if (target != NULL) {
        node_free(target->top);
    }
    free(target);
}

Node *stack_push(Stack *target, Node *node) {
    if (target != NULL) {
        if (node == NULL) {
            printf("error: cannot push NULL node to stack\n");
            return NULL;
        }
        Node *prev_top = target->top;

        target->top = node;
        node_insert(node, NULL, prev_top);

        return node;
    } else {
        printf("error: cannot push on undeclared Stack\n");
        return NULL;
    }
}

Node *stack_pop(Stack *target) {
    if (target != NULL) {
        Node *top = target->top;

        if (top == NULL) {
            printf("error: cannot pop NULL node from stack\n");
            return NULL;
        }

        Node *new_top = top->next;
        Node *popped = node_remove(top);

        target->top = new_top;
        return popped;

    } else {
        printf("error: cannot pop on undeclared Stack\n");
        return NULL;
    }
}

int is_operator(char target) {
    switch (target) {
        case '+':
            break;
        case '-':
            break;
        case '/':
            break;
        case '*':
            break;
        case '^':
            break;
        default:
            return 0;
    }
    return 1;
}

int get_precedence(char operator) {
    switch (operator) {
        case '+':
            return 1;
        case '-':
            return 1;
        case '/':
            return 2;
        case '*':
            return 2;
        case '^':
            return 3;
        default:
            printf("error: cannot get precendence of unknown operator '%c'\n", operator);
            return 0;
    }
}

int is_left_assoc(char operator) {
    switch (operator) {
        case '+':
            return 1;
        case '-':
            return 1;
        case '/':
            return 1;
        case '*':
            return 1;
        case '^':
            return 0;
        default:
            printf("error: cannot get associativity of unknown operator '%c'\n", operator);
            return 0;
    }
}

int node_check_valuetype(Node *target, enum NodeType type) {
    return (target == NULL || target->value_type != type) ? 0 : 1;
}
