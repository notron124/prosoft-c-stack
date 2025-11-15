#include "cstack.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define UNUSED(VAR) (void)(VAR)

#define BASE_STACK_CAPACITY (4)
#define NEW_CAPACITY_MULTIPLIER (2)

#define STACK_CREATION_ERR  ((hstack_t)-1)

struct node {
    struct node* prev;
    size_t data_size;
    char *data;
};

typedef struct node* stack_t;

struct stack_entry {
    int is_reserved;
    stack_t stack;  
};

typedef struct stack_entry stack_entry_t;

// Добавлен capacity для использования realloc
struct stack_entries_table {
    size_t size;
    size_t capacity;
    stack_entry_t* entry; 
};

struct stack_entries_table g_table = { 0, 0, NULL };

hstack_t stack_new(void) {
    // Проверить на пустые ячейки для стеков в таблице
    for (size_t i = 0; i < g_table.size; ++i) {
        if (!g_table.entry[i].is_reserved) {
            g_table.entry[i].is_reserved = 1;
            g_table.entry[i].stack = NULL;
            return (hstack_t)i;
        }
    }

    // Проверить, достаточно ли места в базовом массиве указателей
    if (g_table.size >= g_table.capacity) {
        size_t new_cap = (g_table.capacity == 0) ? BASE_STACK_CAPACITY : g_table.capacity * NEW_CAPACITY_MULTIPLIER;
        
        // Перевыделяем память под массив структур stack_entry_t
        stack_entry_t* new_entries = (stack_entry_t*)realloc(
                g_table.entry,
                new_cap * sizeof(stack_entry_t)
        );

        // В случае, если realloc вернул NULL, выходим с ошибкой
        if (new_entries == NULL) {
            return STACK_CREATION_ERR;
        }

        g_table.entry = new_entries;
        g_table.capacity = new_cap;
    }

    size_t new_index = g_table.size;

    // Инициализируем данные
    g_table.entry[new_index].is_reserved = 1;
    g_table.entry[new_index].stack = NULL; // Возвращаем пустой стэк

    g_table.size++;

    return (hstack_t)new_index;
}

void stack_free(const hstack_t hstack)
{
    if (stack_valid_handler(hstack) != 0) {
        return;
    }

    stack_t current = g_table.entry[(size_t)hstack].stack;

    while (current != NULL) {
        stack_t prev = current->prev;
            
        if (current->data != NULL) {
            free(current->data);
            current->data = NULL;
        }

        free(current);

        current = prev;
    }
    
    g_table.entry[(size_t)hstack].is_reserved = 0;
    g_table.entry[(size_t)hstack].stack = NULL;
}

int stack_valid_handler(const hstack_t hstack) {
    if ((size_t)hstack > g_table.size) {
        return 1;
    }

    return !g_table.entry[(size_t)hstack].is_reserved;
}

unsigned int stack_size(const hstack_t hstack) {
    if (stack_valid_handler(hstack) != 0) {
        return 0;
    }

    unsigned int size = 0;
    stack_t current = g_table.entry[(size_t)hstack].stack;

    while (current != NULL) {
        size++;
        current = current->prev;
    }

    return size;
}

void stack_push(const hstack_t hstack, const void* data_in, const unsigned int size) {
    // Выход на не валидный хендлер/данные
    if (stack_valid_handler(hstack) != 0 || data_in == NULL || size == 0) {
        return;
    }
        
    stack_t node = (stack_t)malloc(sizeof(struct node));
    node->prev = g_table.entry[(size_t)hstack].stack;
    node->data = (char*)malloc(size);
        
    // Выход, если выделение памяти не удалось
    if (node->data == NULL) {
       free(node);
       return;
    }

    memcpy(node->data, data_in, size);
    node->data_size = size;

    g_table.entry[(size_t)hstack].stack = node;
}

unsigned int stack_pop(const hstack_t hstack, void* data_out, const unsigned int size) {
    UNUSED(hstack);
    UNUSED(data_out);
    UNUSED(size);
    return 0;
}

