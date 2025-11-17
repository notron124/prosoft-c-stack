#include "cstack.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define UNUSED(VAR) (void)(VAR)

#define BASE_STACK_CAPACITY (4)             // Базовая емкость таблицы стеков
#define NEW_CAPACITY_MULTIPLIER (2)         // Множитель новой емкости

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

/**
 * @brief Создает новый стек.
 * @param none
 * @return -1 в случае ошибки, хендлер нового стека иначе.
 */
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
        // При первом выделении работает как malloc
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

/**
 * @brief Удалить стек, если hstack существует.
 * @param stack Хендлер стека.
 * @return none
 */
void stack_free(const hstack_t hstack)
{
    // Проверка валидности хендлера
    if (stack_valid_handler(hstack) != 0) {
        return;
    }

    stack_t current = g_table.entry[(size_t)hstack].stack;

    // Проходимся по всему стеку, освобождая памтять, выделенную под данные
    while (current != NULL) {
        stack_t prev = current->prev;
            
        if (current->data != NULL) {
            free(current->data);
            current->data = NULL;
        }

        free(current);

        current = prev;
    }
    
    // Отмечаем вхождение в таблице как свободное
    g_table.entry[(size_t)hstack].is_reserved = 0;
    g_table.entry[(size_t)hstack].stack = NULL;
}

/**
 * @brief Проверить валидность хендлера.
 * @param stack Хендлер стека.
 * @return 0, если хендлер существует, 1 иначе.
 */
int stack_valid_handler(const hstack_t hstack) {
    // Провека относительно размера таблицы стеков
    if ((size_t)hstack >= g_table.size || hstack < 0) {
        return 1;
    }

    // Если индекс стека зарезервирован, значит он валидный
    return !g_table.entry[(size_t)hstack].is_reserved;
}

/**
 * @brief Проучить количество элементов стека.
 * @param stack Хендлер стека.
 * @return Количество элементов в стеке, если hstack валиден, иначе 0.
 */
unsigned int stack_size(const hstack_t hstack) {
    // Проверка валидности хендлера
    if (stack_valid_handler(hstack) != 0) {
        return 0;
    }

    unsigned int size = 0;
    stack_t current = g_table.entry[(size_t)hstack].stack;

    // Подсчет кол-ва элементов
    while (current != NULL) {
        size++;
        current = current->prev;
    }

    return size;
}

/**
 * @brief Добавить элемент данных из буфера в стек.
 * @param stack Хендлер стека.
 * @param data_in Указатель на массив с данными.
 * @param size Размер данных для записи.
 * @note Добавление данных в стек происходит by copy.
 */
void stack_push(const hstack_t hstack, const void* data_in, const unsigned int size) {
    // Выход на не валидный хендлер/данные
    if (stack_valid_handler(hstack) != 0 || data_in == NULL || size == 0) {
        return;
    }
    
    // Выделяем память под ноду    
    stack_t node = (stack_t)malloc(sizeof(struct node));
    // Предыдущее для новой ноды, это прошлая нода
    node->prev = g_table.entry[(size_t)hstack].stack;
    // Выделяем память под данные, которые хотят поместить в стек
    node->data = (char*)malloc(size);
        
    // Выход, если выделение памяти не удалось
    if (node->data == NULL) {
       free(node);
       return;
    }

    // Копируем данные
    memcpy(node->data, data_in, size);
    node->data_size = size;

    // Обновляем указатель на head
    g_table.entry[(size_t)hstack].stack = node;
}

/**
 * @brief Извлекает элемент из стека и записать данные этого элемента в data_out.
 * @param stack Хендлер стека.
 * @param data_out Указатель на массив, куда будут записаны данные.
 * @param size Размер массива, на который передан указатель.
 * @return Размер записанных данных, если соответствующий хэндлеру стек
 * существует, 0 иначе.
 * @note Если стек пустой, возвращаемое значение будет равно 0.
 */
unsigned int stack_pop(const hstack_t hstack, void* data_out, const unsigned int size) {
    // Провека на валидность хендела и указателя на буфер 
    if (stack_valid_handler(hstack) != 0 || data_out == NULL) {
        return 0;
    }

    stack_t current = g_table.entry[(size_t)hstack].stack;
    
    // Провека на пустой стек
    if (current == NULL) {
        return 0;
    }
    
    unsigned int data_size = current->data_size;

    // Провека на достаточный размер буфера 
    if (data_size > size) {
        return 0;
    }
    
    // Обновляем укзатель на head стека
    g_table.entry[(size_t)hstack].stack = current->prev;

    // Копируем данные
    memcpy(data_out, current->data, current->data_size);
    
    // Освобождаем память, выделенную под данные и ноду 
    free(current->data);
    free(current);
    
    return data_size;
}

