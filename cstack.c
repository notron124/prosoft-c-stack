#include "cstack.h"
#include <stddef.h>
#include <stdlib.h>

#define UNUSED(VAR) (void)(VAR)

struct node {
    const struct node* prev;
    size_t data_size;
    char *data;
};

typedef struct node* stack_t;

struct stack_entry {
    int reserved;
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

hstack_t stack_new(void)
{
    // Проверить, достаточно ли места в базовом массиве указателей
    if (g_table.size >= g_table.capacity) {
        size_t new_cap = (g_table.capacity == 0) ? 4 : g_table.capacity * 2;
        
        // Перевыделяем память под массив структур stack_entry_t
        stack_entry_t* new_entries = (stack_entry_t*)realloc(
                g_table.entry,
                new_cap * sizeof(stack_entry_t)
        );

        // В случае, если realloc вернул NULL, выходим с ошибкой
        if (new_entries == NULL) {
            return -1;
        }

        g_table.entry = new_entries;
        g_table.capacity = new_cap;
    }

    size_t new_index = g_table.size;

    // Инициализируем данные
    g_table.entry[new_index].reserved = 0;
    g_table.entry[new_index].stack = NULL; // Возвращаем пустой стэк

    g_table.size++;

    return (hstack_t)new_index;
}

void stack_free(const hstack_t hstack)
{
    UNUSED(hstack);
}

int stack_valid_handler(const hstack_t hstack)
{
    UNUSED(hstack);
    return 1;
}

unsigned int stack_size(const hstack_t hstack)
{
    UNUSED(hstack);
    return 0;
}

void stack_push(const hstack_t hstack, const void* data_in, const unsigned int size)
{
    UNUSED(hstack);
    UNUSED(data_in);
    UNUSED(size);
}

unsigned int stack_pop(const hstack_t hstack, void* data_out, const unsigned int size)
{
    UNUSED(hstack);
    UNUSED(data_out);
    UNUSED(size);
    return 0;
}

