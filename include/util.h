#ifndef INCLUDE_UTIL_H
#define INCLUDE_UTIL_H

enum NodeType { TYPE_OPERATOR, TYPE_NUMBER, TYPE_PAREN};

typedef struct Node {
    struct Node *next;
    struct Node *previous;
    union {
        char operator;
        char paren;
        long double number;
    };
    enum NodeType value_type;
} Node;

typedef struct Stack {
    Node *top;
} Stack;

// print value of node
void node_print(Node *target, int newline);

//  allocate node
Node *node_create(long double value, enum NodeType type);

// insert node between two nodes
Node *node_insert(Node *target, Node *previous, Node *next);

// update surrounding nodes and return popped node
Node *node_remove(Node *target);

// free all nodes that are connected to target
void node_free(Node *target);

// free all nodes and stack
void stack_free(Stack *target);

// allocate new stack
Stack *stack_create(Node *head);

// push node to stack
Node *stack_push(Stack *target, Node *node);

// pop node from stack
Node *stack_pop(Stack *target);

// if char is an operator * / + - ^
int is_operator(char target);

// get precendence value of operator
int get_precedence(char operator);

// return 1 if operator is left associative
int is_left_assoc(char operator);

// check if node matches type, return 0 when NULL
int node_check_valuetype(Node *target, enum NodeType type);

#endif
