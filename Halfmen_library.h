//Halfmen(library).h- Основной заголовочный файл программы сжатия Хаффмана


#ifndef HALFWOMEN_MAIN_C
#define HALFWOMEN_MAIN_C

// Включение необходимых библиотек

#include <stdio.h>    
#include <stdlib.h>   
#include <string.h>   // Для строковых операций
#include <stdint.h>   // Для стандартных целочисленных типов

//!!!!Определение структур данных!!!! 

/*
Структура: Представляет узел в двоичном дереве Хаффмана.
  - character: Символ, который представляет этот узел (для листьев)
  - weight:    Вес узла (частота появления символа)
  - child_left:  Указатель на левого потомка
  - child_right: Указатель на правого потомка
*/
typedef struct TreeNode {
    unsigned char character;      // Символ (8 бит)
    unsigned int weight;          // частота символа
    struct TreeNode *child_left;  // Левый потомок (бит 0)
    struct TreeNode *child_right; // Правый (бит 1)
} TreeNode;

/*
Структура: Используется для построения упорядоченного списка узлов при
            создании дерева Хаффмана. Это упрощенная альтернатива min-heap.
  - huffman_node: Указатель на узел дерева
  - next_item:    Указатель на следующий элемент списка
*/
typedef struct ListItem {
    TreeNode *huffman_node;      // Ссылка на узел дерева Хаффмана
    struct ListItem *next_item;  // Следующий элемент в списке
} ListItem;

/*
Структура: Хранит соответствие между символом и его кодом Хаффмана.
Компоненты:
  - character:       Исходный символ
  - binary_sequence: Строка с двоичным кодом (из '0' и '1')
  - sequence_length: Длина двоичного кода в битах
*/
typedef struct EncodingRecord {
    unsigned char character;     // Исходный символ
    char *binary_sequence;       // Двоичный код Хаффмана
    int sequence_length;         // Длина кода
} EncodingRecord;

//!!!!Объявления функций!!!! 

//????Операции с деревом Хаффмана????

/*
Функция: Создает новый узел дерева Хаффмана
  - character: Символ для узла
  - weight:    Частота символа
Возвращает: Указатель на созданный узел или NULL при ошибке
*/
TreeNode* make_tree_node(unsigned char character, unsigned int weight);

/*
Функция: Создает элемент списка для хранения узла дерева
  - huffman_node: Узел дерева для хранения в списке
Возвращает: Указатель на созданный элемент списка или NULL при ошибке
*/
ListItem* make_list_item(TreeNode *huffman_node);

/*
Функция: Вставляет элемент в упорядоченный список по возрастанию веса
  - list_head: Указатель на указатель головы списка (может измениться)
  - new_item:  Новый элемент для вставки
*/
void place_in_order(ListItem **list_head, ListItem *new_item);

/*
Функция: Строит дерево Хаффмана на основе таблицы частот
  - frequency_array: Массив частот символов (256 элементов)
Возвращает: Указатель на корень дерева или NULL при ошибке
*/
TreeNode* construct_huffman_tree(unsigned int *frequency_array);

/*
Функция: Рекурсивно освобождает память, занятую деревом
  - root_node: Корень дерева для удаления
*/
void destroy_tree(TreeNode *root_node);

//????Операции с кодировками????

/*
Функция: Рекурсивно генерирует коды Хаффмана для всех символов в дереве
  - root_node:       Текущий узел дерева
  - temp_buffer:     Буфер для накопления текущего кода
  - current_depth:   Текущая глубина в дереве (длина кода)
  - encoding_table:  Таблица для сохранения кодов
*/
void create_encodings(TreeNode *root_node, char *temp_buffer, 
                      int current_depth, EncodingRecord *encoding_table);

/*
Функция: Освобождает память, занятую таблицей кодирования
  - encoding_table: Таблица кодирования для очистки
  - table_size:     Размер таблицы (обычно 256)
*/
void clear_encoding_table(EncodingRecord *encoding_table, int table_size);

//????Основные операции сжатия/восстановления????

/*
Функция: Сжимает файл с использованием алгоритма Хаффмана
  - source_filename:  Путь к исходному файлу
  - result_filename:  Путь для сохранения сжатого файла
*/
void perform_compression(const char *source_filename, const char *result_filename);

/*
Функция: Восстанавливает файл из сжатого формата
  - compressed_filename: Путь к сжатому файлу
  - restored_filename:   Путь для сохранения восстановленного файла
*/
void perform_decompression(const char *compressed_filename, const char *restored_filename);

//????Вспомогательные операции????

/*
Функция: Подсчитывает частоту появления каждого символа в файле
  - file_stream:     Указатель на открытый файл
  - frequency_array: Массив для сохранения частот (256 элементов)
*/
void compute_frequencies(FILE *file_stream, unsigned int *frequency_array);

/*
Функция: Записывает один бит в буфер, при заполнении записывает в файл
  - file_stream:   Указатель на выходной файл
  - bit_value:     Бит для записи (0 или 1)
  - bit_collector: Буфер для накопления битов
  - bit_counter:   Счетчик битов в буфере
*/
void output_bit(FILE *file_stream, unsigned char bit_value, 
                unsigned char *bit_collector, int *bit_counter);

/*
Функция: Записывает оставшиеся биты из буфера в файл
  - file_stream:   Указатель на выходной файл
  - bit_collector: Буфер с битами
  - bit_counter:   Количество битов в буфере
*/
void empty_bit_buffer(FILE *file_stream, unsigned char *bit_collector, int *bit_counter);

/*
Функция: Читает один бит из файла, используя байтовый буфер
  - file_stream:   Указатель на входной файл
  - bit_collector: Буфер для чтения байтов
  - bit_counter:   Счетчик оставшихся битов в буфере
  - end_of_file:   Флаг конца файла (устанавливается при EOF)
Возвращает: Прочитанный бит (0 или 1)
*/
unsigned char input_bit(FILE *file_stream, unsigned char *bit_collector, 
                        int *bit_counter, int *end_of_file);

#endif /* HALFWOMEN_MAIN_C */
