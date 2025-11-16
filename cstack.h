#ifndef CSTACK_H
#define CSTACK_H

// Хендлер стека
typedef int hstack_t;

/**
 * @brief Создает новый стек.
 * @param none
 * @return -1 в случае ошибки, хендлер нового стека иначе.
 */
hstack_t stack_new(void);

/**
 * @brief Удалить стек, если hstack существует.
 * @param stack Хендлер стека.
 * @return none
 */
void stack_free(const hstack_t stack);

/**
 * @brief Проверить валидность хендлера.
 * @param stack Хендлер стека.
 * @return 0, если хендлер существует, 1 иначе.
 */
int stack_valid_handler(const hstack_t stack);

/**
 * @brief Проучить количество элементов стека.
 * @param stack Хендлер стека.
 * @return Количество элементов в стеке, если hstack валиден, иначе 0.
 */
unsigned int stack_size(const hstack_t stack);

/**
 * @brief Добавить элемент данных из буфера в стек.
 * @param stack Хендлер стека.
 * @param data_in Указатель на массив с данными.
 * @param size Размер данных для записи.
 * @note Добавление данных в стек происходит by copy.
 */
void stack_push(const hstack_t stack, const void* data_in, const unsigned int size);

/**
 * @brief Извлекает элемент из стека и записать данные этого элемента в data_out.
 * @param stack Хендлер стека.
 * @param data_out Указатель на массив, куда будут записаны данные.
 * @param size Размер массива, на который передан указатель.
 * @return Размер записанных данных, если соответствующий хэндлеру стек
 * существует, 0 иначе.
 * @note Если стек пустой, возвращаемое значение будет равно 0.
 */
unsigned int stack_pop(const hstack_t stack, void* data_out, const unsigned int size);

#endif /* CSTACK_H */
