#include "util.h"
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Node *parse_to_postfix(char *expr);
long double evaluate_postfix(Node *expr, int *err);

int main(void) {
    char expr[200] = {0};

    while (1) {
        fgets(expr, 200, stdin);

        if (strncmp(expr, "quit", 4) == 0) {
            break;
        }

        Node *parsed = parse_to_postfix(expr);

        if (parsed != NULL) {
            int err;
            long double result = evaluate_postfix(parsed, &err);

            if (err == 0) {
                printf("= %Lf\n", result);
            }
        }
        memset(expr, 0, strlen(expr));
    }

    return 0;
}

Node *parse_to_postfix(char *expr) {
    Stack *output = stack_create(NULL);
    Stack *op_stack = stack_create(NULL);
    Node *bottom = NULL;
    int first_node_created = 0, previous_node_is_op = 1;
    char *num_buf = calloc(strlen(expr) + 1, sizeof(char));

    if (num_buf == NULL) {
        perror("error:");
        abort();
    }

    for (char c = *expr; c != 0; c = *++expr) {
        if (isspace(c)) {
            continue;
        }

        int c_is_op = is_operator(c);
        int converted_num = 0;

        // convert num_buf to long double and push to output
        if ((c_is_op || c == '(' || c == ')') && strlen(num_buf) > 0) {
            stack_push(output, node_create(strtold(num_buf, NULL), TYPE_NUMBER));
            memset(num_buf, 0, strlen(num_buf));
            converted_num = 1;
        }

        if (c_is_op) {
            // convert - into -1 *
            if (c == '-' &&
                ((op_stack->top != NULL &&
                  (previous_node_is_op || (op_stack->top->value_type == TYPE_PAREN && !converted_num))) ||
                 output->top == NULL)) {
                c = '*';
                stack_push(output, node_create(-1, TYPE_NUMBER));

                // simulate having -1 * take precedence over right associative operators
                if (node_check_valuetype(op_stack->top, TYPE_OPERATOR) &&
                    !is_left_assoc(op_stack->top->operator)) {

                    stack_push(op_stack, node_create(c, TYPE_OPERATOR)); // push * to op_stack
                    goto exit_ifs;
                }
            }

            while (node_check_valuetype(op_stack->top, TYPE_OPERATOR) &&
                   ((is_left_assoc(c) && get_precedence(c) <= get_precedence(op_stack->top->operator)) ||
                    (!is_left_assoc(c) && get_precedence(c) < get_precedence(op_stack->top->operator)))) {

                stack_push(output, stack_pop(op_stack)); // pop from stack to output
            }
            stack_push(op_stack, node_create(c, TYPE_OPERATOR)); // push c to op_stack
        } else if (c == '(') {
            stack_push(op_stack, node_create(c, TYPE_PAREN));
        } else if (c == ')') {
            while (!node_check_valuetype(op_stack->top, TYPE_PAREN) && op_stack->top->paren != '(') {
                stack_push(output, stack_pop(op_stack)); // pop from stack to output
            }
            free(stack_pop(op_stack)); // free left parenthesis
        } else {
            num_buf[strlen(num_buf)] = c;
        }

    exit_ifs:
        // set reference to first node added to output
        if (!first_node_created && output->top != NULL) {
            bottom = output->top;
            first_node_created = 1;
        }

        previous_node_is_op = c_is_op;
    }

    // convert any left over digits in num_buf
    if (strlen(num_buf) > 0) {
        stack_push(output, node_create(strtold(num_buf, NULL), TYPE_NUMBER));
    }

    // pop remaining operators in op_stack to output
    while (op_stack->top != NULL) {
        stack_push(output, stack_pop(op_stack));
    }

    if (!first_node_created && output->top != NULL) {
        bottom = output->top;
    }

    stack_free(op_stack);
    free(output);
    free(num_buf);

    // Node *head = bottom;

    // while (head != NULL) {
    //     node_print(head, 0);
    //     head = head->previous;
    // }
    // printf("\n");

    return bottom;
}

long double evaluate_postfix(Node *expr, int *err) {
    Stack *stack = stack_create(NULL);
    Node *previous = expr->previous;

    *err = 0;

    while (expr != NULL) {
        previous = expr->previous;

        if (node_check_valuetype(expr, TYPE_NUMBER)) {
            stack_push(stack, node_remove(expr));
        } else if (node_check_valuetype(expr, TYPE_OPERATOR)) {
            if (stack->top == NULL || stack->top->next == NULL) {
                printf("error: syntax error\n");
                *err = 1;
                break;
            }

            Node *a = stack_pop(stack), *b = stack_pop(stack);
            long double result;

            switch (expr->operator) {
                case '+':
                    result = b->number + a->number;
                    break;
                case '-':
                    result = b->number - a->number;
                    break;
                case '/':
                    result = b->number / a->number;
                    break;
                case '*':
                    result = b->number * a->number;
                    break;
                case '^':
                    result = pow(b->number, a->number);
                    break;
            }
            free(a);
            free(b);
            free(node_remove(expr)); // free operator from expression
            stack_push(stack, node_create(result, TYPE_NUMBER));
        }
        expr = previous;
    }
    long double whole_result = (node_check_valuetype(stack->top, TYPE_NUMBER)) ? stack->top->number : 0;

    stack_free(stack);
    node_free(expr);

    return whole_result;
}
