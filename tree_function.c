
//tree(functions).c - Реализация функций для работы с деревом Хаффмана



#include "Halfmen_library.h"  // Включаем объявления структур и функций

// Реализация функций для работы с деревом

/*
Функция: Создает новый узел дерева Хаффмана
  - character: Символ, который будет храниться в узле
  - weight:    Частота появления символа (вес узла)
Возвращает:
  - Указатель на созданный узел
  - NULL, если не удалось выделить память
*/
TreeNode* make_tree_node(unsigned char character, unsigned int weight) {
    TreeNode *node_ptr = (TreeNode*)malloc(sizeof(TreeNode));  // Выделение памяти
    if (node_ptr) {  // Проверка успешности выделения памяти
        node_ptr->character = character;  // Установка символа
        node_ptr->weight = weight;        // Установка веса
        node_ptr->child_left = NULL;      // Инициализация левого потомка
        node_ptr->child_right = NULL;     // Инициализация правого потомка
    }
    return node_ptr;  // Возврат указателя на узел
}

/*
Функция: Создает элемент списка для хранения узла дерева
  - huffman_node: Указатель на узел дерева для хранения
Возвращает:
  - Указатель на созданный элемент списка
  - NULL, если не удалось выделить память
*/
ListItem* make_list_item(TreeNode *huffman_node) {
    ListItem *item_ptr = (ListItem*)malloc(sizeof(ListItem));  // Выделение памяти
    if (item_ptr) {  // Проверка успешности выделения
        item_ptr->huffman_node = huffman_node;  // Сохранение ссылки на узел
        item_ptr->next_item = NULL;             // Следующий элемент пока отсутствует
    }
    return item_ptr;  // Возврат указателя на элемент
}

/*
Функция: Вставляет элемент в упорядоченный список по возрастанию веса
  - list_head: Указатель на указатель головы списка (может измениться)
  - new_item:  Новый элемент для вставки в список
*/
void place_in_order(ListItem **list_head, ListItem *new_item) {
    // Случай 1: Список пуст или новый элемент должен быть в начале
    if (*list_head == NULL || new_item->huffman_node->weight < (*list_head)->huffman_node->weight) {
        new_item->next_item = *list_head;  // Новый элемент указывает на старую голову
        *list_head = new_item;             // Новый элемент становится головой
    } 
    // Случай 2: Вставка в середину или конец списка
    else {
        ListItem *current_ptr = *list_head;  // Начинаем с головы списка
        
        // Ищем позицию для вставки: идем до тех пор, пока следующий элемент
        // существует и его вес меньше или равен весу нового элемента
        while (current_ptr->next_item != NULL && 
               current_ptr->next_item->huffman_node->weight <= new_item->huffman_node->weight) {
            current_ptr = current_ptr->next_item;  // Переходим к следующему элементу
        }
        
        // Вставляем новый элемент после current_ptr
        new_item->next_item = current_ptr->next_item;  // Новый элемент указывает на следующий
        current_ptr->next_item = new_item;             // Текущий элемент указывает на новый
    }
}

/*
Функция: Строит дерево Хаффмана на основе таблицы частот
  - frequency_array: Массив частот символов (256 элементов)
Возвращает:
  - Указатель на корень построенного дерева
  - NULL, если файл пуст или произошла ошибка
Особые случаи:
  - Пустой файл: возвращает NULL
  - Один уникальный символ: создает искусственное дерево с одним листом
*/
TreeNode* construct_huffman_tree(unsigned int *frequency_array) {
    ListItem *list_head = NULL;        // Голова упорядоченного списка
    int distinct_count = 0;            // Количество уникальных символов
    
    // Этап 1: Создание начального списка листьев
    for (int index = 0; index < 256; index++) {
        if (frequency_array[index] > 0) {  // Только для символов, которые встречаются
            // Создаем лист дерева для символа
            TreeNode *leaf_node = make_tree_node((unsigned char)index, frequency_array[index]);
            
            // Создаем элемент списка для листа
            ListItem *list_element = make_list_item(leaf_node);
            
            // Вставляем элемент в упорядоченный список
            place_in_order(&list_head, list_element);
            
            distinct_count++;  // Увеличиваем счетчик уникальных символов
        }
    }
    
    // Обработка особых случаев
    
    // Случай 1: Файл пустой
    if (distinct_count == 0) {
        return NULL;  // Нечего сжимать
    }
    
    // Случай 2: В файле только один уникальный символ
    // Алгоритм Хаффмана требует хотя бы двух символов для построения дерева,
    // поэтому создаем искусственное дерево
    if (distinct_count == 1) {
        TreeNode *single_node = list_head->huffman_node;  // Единственный узел
        
        // Создаем искусственный корень с нулевым символом
        TreeNode *artificial_root = make_tree_node(0, single_node->weight);
        artificial_root->child_left = single_node;   // Левый потомок - единственный символ
        artificial_root->child_right = NULL;         // Правый потомок отсутствует
        
        free(list_head);  // Освобождаем элемент списка (но не узел дерева!)
        return artificial_root;  // Возвращаем искусственное дерево
    }
    
    // Этап 2: Основной алгоритм построения дерева
    // Пока в списке больше одного элемента
    while (list_head != NULL && list_head->next_item != NULL) {
        // Шаг 1: Извлечение двух узлов с наименьшими частотами
        ListItem *first_min = list_head;          // Первый минимальный элемент
        ListItem *second_min = list_head->next_item; // Второй минимальный элемент
        
        // Обновляем голову списка: пропускаем два извлеченных элемента
        list_head = second_min->next_item;  // Теперь голова - третий элемент
        
        // Шаг 2: Создание нового внутреннего узла
        // Сумма частот двух извлеченных узлов
        unsigned int combined_weight = first_min->huffman_node->weight + 
                                      second_min->huffman_node->weight;
        
        // Создаем новый внутренний узел (символ не важен, используем 0)
        TreeNode *merged_node = make_tree_node(0, combined_weight);
        
        // Делаем извлеченные узлы потомками нового узла
        // Меньший частотой становится левым потомком (может быть любым)
        merged_node->child_left = first_min->huffman_node;
        merged_node->child_right = second_min->huffman_node;
        
        // Шаг 3: Создаем элемент списка для нового узла
        ListItem *new_list_element = make_list_item(merged_node);
        
        // Шаг 4: Вставляем новый узел обратно в упорядоченный список
        place_in_order(&list_head, new_list_element);
        
        // Шаг 5: Освобождаем элементы списка (но не узлы дерева!)
        // Узлы дерева теперь являются частью дерева и будут освобождены позже
        free(first_min);   // Освобождаем первый элемент списка
        free(second_min);  // Освобождаем второй элемент списка
    }
    
    // В списке остался один элемент - это корень дерева Хаффмана
    TreeNode *final_root = list_head->huffman_node;  // Извлекаем корень дерева
    
    free(list_head);  // Освобождаем последний элемент списка
    
    return final_root;  // Возвращаем построенное дерево
}

/*
Функция: Рекурсивно освобождает память, занятую деревом Хаффмана
  - root_node: Корень дерева для освобождения

*/
void destroy_tree(TreeNode *root_node) {
    // Базовый случай рекурсии: пустое дерево
    if (root_node == NULL) return;
    
    // Рекурсивный вызов для левого поддерева
    destroy_tree(root_node->child_left);
    
    // Рекурсивный вызов для правого поддерева
    destroy_tree(root_node->child_right);
    
    // Освобождение текущего узла (после освобождения потомков)
    free(root_node);
}

//!!!!Реализация функций для работы с кодировками!!!!

/*
Функция:Рекурсивно генерирует коды Хаффмана для всех символов в дереве

  - root_node:      Текущий узел дерева (изначально - корень)
  - temp_buffer:    Буфер для накопления текущего кода
  - current_depth:  Текущая глубина в дереве (длина кода)
  - encoding_table: Таблица для сохранения сгенерированных кодов
Замечание: Максимальная глубина рекурсии ограничена 256 для безопасности
*/
void create_encodings(TreeNode *root_node, char *temp_buffer, 
                      int current_depth, EncodingRecord *encoding_table) {
    // Базовый случай: пустой узел
    if (root_node == NULL) return;
    
    // Случай 1: Достигнут лист дерева (символ)
    if (root_node->child_left == NULL && root_node->child_right == NULL) {
        // Сохраняем символ в соответствующей ячейке таблицы
        encoding_table[root_node->character].character = root_node->character;
        
        // Выделяем память для хранения двоичного кода
        // +1 для нуль-терминатора строки
        encoding_table[root_node->character].binary_sequence = (char*)malloc(current_depth + 1);
        
        // Проверяем успешность выделения памяти
        if (encoding_table[root_node->character].binary_sequence) {
            // Копируем накопленный код из буфера в таблицу
            strncpy(encoding_table[root_node->character].binary_sequence, temp_buffer, current_depth);
            
            // Добавляем нуль-терминатор в конец строки
            encoding_table[root_node->character].binary_sequence[current_depth] = '\0';
            
            // Сохраняем длину кода
            encoding_table[root_node->character].sequence_length = current_depth;
        }
        return;  // Возврат из рекурсивного вызова
    }
    
    // Случай 2: Внутренний узел - обрабатываем левое поддерево
    // Добавляем '0' к текущему коду для левого перехода
    if (current_depth < 256) {  // Защита от переполнения буфера
        temp_buffer[current_depth] = '0';  // Бит '0' соответствует левому переходу
        
        // Рекурсивный вызов для левого поддерева
        create_encodings(root_node->child_left, temp_buffer, 
                        current_depth + 1, encoding_table);
    }
    
    // Случай 3: Внутренний узел - обрабатываем правое поддерево
    // Добавляем '1' к текущему коду для правого перехода
    if (current_depth < 256) {  // Защита от переполнения буфера
        temp_buffer[current_depth] = '1';  // Бит '1' соответствует правому переходу
        
        // Рекурсивный вызов для правого поддерева
        create_encodings(root_node->child_right, temp_buffer, 
                        current_depth + 1, encoding_table);
    }
}

/*
Функция: Освобождает память, занятую таблицей кодирования Хаффмана
  - encoding_table: Таблица кодирования для очистки
  - table_size:     Размер таблицы (обычно 256 для всех возможных байтов)
*/
void clear_encoding_table(EncodingRecord *encoding_table, int table_size) {
    // Проходим по всем элементам таблицы
    for (int i = 0; i < table_size; i++) {
        // Если для этого символа был выделен код
        if (encoding_table[i].binary_sequence != NULL) {
            free(encoding_table[i].binary_sequence);  // Освобождаем память
            encoding_table[i].binary_sequence = NULL; // Хорошая практика: обнуляем указатель
        }
    }
}

//!!!!Реализация вспомогательных функций

/*
Функция:Подсчитывает частоту появления каждого символа в файле
  - file_stream:     Указатель на открытый файл (должен быть открыт для чтения)
  - frequency_array: Массив для сохранения частот (должен быть размером 256)
*/
void compute_frequencies(FILE *file_stream, unsigned int *frequency_array) {
    // Инициализация массива частот нулями
    // sizeof(unsigned int) * 256 - размер всего массива в байтах
    memset(frequency_array, 0, 256 * sizeof(unsigned int));
    
    int character;  // Переменная для чтения символов (используем int для корректной обработки EOF)
    
    // Чтение файла посимвольно
    // fgetc возвращает int, чтобы различать EOF и допустимые символы
    while ((character = fgetc(file_stream)) != EOF) {
        // Увеличиваем частоту для прочитанного символа
        // Приведение к unsigned char гарантирует корректный индекс (0-255)
        frequency_array[(unsigned char)character]++;
    }
    
    // Возвращаем указатель файла в начало для последующего чтения
    // Это необходимо, так как после подсчета частот нужно снова читать файл для кодирования
    rewind(file_stream);
}

/*
Функция: Записывает один бит в буфер, при заполнении буфера записывает байт в файл

  - file_stream:   Указатель на выходной файл (должен быть открыт для записи)
  - bit_value:     Бит для записи (0 или 1)
  - bit_collector: Указатель на буфер для накопления битов
  - bit_counter:   Указатель на счетчик битов в буфере

*/
void output_bit(FILE *file_stream, unsigned char bit_value, 
                unsigned char *bit_collector, int *bit_counter) {
    // Сдвигаем текущий буфер влево на 1 бит
    // и добавляем новый бит в младший разряд
    // &1 гарантирует, что мы работаем только с младшим битом bit_value
    *bit_collector = (*bit_collector << 1) | (bit_value & 1);
    
    (*bit_counter)++;  // Увеличиваем счетчик битов в буфере
    
    // Если буфер заполнен (8 бит)
    if (*bit_counter == 8) {
        fputc(*bit_collector, file_stream);  // Записываем байт в файл
        *bit_collector = 0;                  // Сбрасываем буфер
        *bit_counter = 0;                    // Сбрасываем счетчик
    }
}

/*
Функция: Записывает оставшиеся биты из буфера в файл, дополняя до полного байта

  - file_stream:   Указатель на выходной файл
  - bit_collector: Указатель на буфер с битами
  - bit_counter:   Указатель на количество битов в буфере
Замечание: Эта функция должна вызываться после завершения записи всех битов
*/
void empty_bit_buffer(FILE *file_stream, unsigned char *bit_collector, int *bit_counter) {
    // Если в буфере есть незаписанные биты
    if (*bit_counter > 0) {
        // Сдвигаем биты в старшие разряды, дополняя нулями справа
        // (8 - bit_counter) - количество нулей, которые нужно добавить
        *bit_collector = *bit_collector << (8 - *bit_counter);
        
        fputc(*bit_collector, file_stream);  // Записываем байт в файл
        *bit_collector = 0;                  // Сбрасываем буфер
        *bit_counter = 0;                    // Сбрасываем счетчик
    }
}

/*
Функция: Читает один бит из файла, используя байтовый буфер
  - file_stream:   Указатель на входной файл
  - bit_collector: Указатель на буфер для чтения байтов
  - bit_counter:   Указатель на счетчик оставшихся битов в буфере
  - end_of_file:   Указатель на флаг конца файла
Возвращает:
  - Прочитанный бит (0 или 1)
  - Если достигнут конец файла, устанавливает флаг и возвращает 0
*/
unsigned char input_bit(FILE *file_stream, unsigned char *bit_collector, 
                        int *bit_counter, int *end_of_file) {
    // Если буфер пуст (все биты прочитаны)
    if (*bit_counter == 0) {
        int byte_value = fgetc(file_stream);  // Читаем следующий байт из файла
        
        // Проверяем, не достигли ли конца файла
        if (byte_value == EOF) {
            *end_of_file = 1;  // Устанавливаем флаг конца файла
            return 0;          // Возвращаем 0 (значение не важно, так как eof=1)
        }
        
        // Сохраняем прочитанный байт в буфер
        *bit_collector = (unsigned char)byte_value;
        
        *bit_counter = 8;  // Устанавливаем счетчик битов (только что прочитали 8 бит)
    }
    
    // Извлекаем старший (7-й) бит из буфера
    // >>7 сдвигает старший бит в младшую позицию
    // &1 изолирует младший бит
    unsigned char bit_value = (*bit_collector >> 7) & 1;
    
    // Сдвигаем буфер влево на 1 бит, чтобы следующий бит стал старшим
    *bit_collector = *bit_collector << 1;
    
    (*bit_counter)--;  // Уменьшаем счетчик оставшихся битов
    
    return bit_value;  // Возвращаем прочитанный бит
}

//!!!! Реализация основных функций сжатия и восстановления !!!!

/*
Функция: Сжимает файл с использованием алгоритма Хаффмана
  - source_filename: Путь к исходному файлу для сжатия
  - result_filename: Путь для сохранения сжатого файла
*/
void perform_compression(const char *source_filename, const char *result_filename) {
    // Этап 1: Открытие файлов
    FILE *input_stream = fopen(source_filename, "rb");  // Открываем для чтения в бинарном режиме
    if (!input_stream) {
        printf("ERROR: Unable to open source file %s\n", source_filename);
        return;
    }
    
    FILE *output_stream = fopen(result_filename, "wb");  // Открываем для записи в бинарном режиме
    if (!output_stream) {
        printf("ERROR: Unable to create output file %s\n", result_filename);
        fclose(input_stream);  // Закрываем входной файл перед выходом
        return;
    }
    
    printf("File compression in progress %s...\n", source_filename);
    
    // Этап 2: Построение таблицы частот
    unsigned int frequency_data[256];  // Массив для хранения частот всех возможных байтов (0-255)
    compute_frequencies(input_stream, frequency_data);
    
    // Этап 3: Построение дерева Хаффмана
    TreeNode *huffman_root = construct_huffman_tree(frequency_data);
    if (huffman_root == NULL) {
        printf("ERROR: The source file is empty\n");
        fclose(input_stream);
        fclose(output_stream);
        return;
    }
    
    // Этап 4: Генерация кодов Хаффмана
    EncodingRecord encoding_data[256];  // Таблица для хранения кодов
    memset(encoding_data, 0, sizeof(encoding_data));  // Инициализация нулями
    
    char encoding_buffer[256];  // Временный буфер для рекурсивной генерации кодов
    create_encodings(huffman_root, encoding_buffer, 0, encoding_data);
    
    // Этап 5: Запись таблицы частот в начало выходного файла
    // Это служебная информация, необходимая для восстановления файла
    // Без этой информации невозможно декодировать сжатые данные
    fwrite(frequency_data, sizeof(unsigned int), 256, output_stream);
    
    // Этап 6: Кодирование данных и запись в файл
    unsigned char bit_accumulator = 0;  // Буфер для накопления битов
    int accumulated_bits = 0;           // Счетчик битов в буфере
    
    int character;  // Переменная для чтения символов
    // Читаем исходный файл посимвольно
    while ((character = fgetc(input_stream)) != EOF) {
        // Получаем код Хаффмана для текущего символа из таблицы
        char *binary_code = encoding_data[(unsigned char)character].binary_sequence;
        
        // Если код найден (всегда должен находиться для символов с ненулевой частотой)
        if (binary_code) {
            // Проходим по каждому биту кода и записываем его в выходной файл
            for (int i = 0; binary_code[i] != '\0'; i++) {
                // Преобразуем символ '0' или '1' в бит 0 или 1
                unsigned char bit_value = (binary_code[i] == '1') ? 1 : 0;
                
                // Записываем бит в файл через буфер
                output_bit(output_stream, bit_value, &bit_accumulator, &accumulated_bits);
            }
        }
    }
    
    // Записываем оставшиеся биты (дополняем буфер до полного байта)
    empty_bit_buffer(output_stream, &bit_accumulator, &accumulated_bits);
    
    // Этап 7: Вывод статистики сжатия
    fseek(input_stream, 0, SEEK_END);           // Перемещаем указатель в конец входного файла
    long initial_size = ftell(input_stream);    // Получаем размер исходного файла
    
    fseek(output_stream, 0, SEEK_END);          // Перемещаем указатель в конец выходного файла
    long compressed_size = ftell(output_stream); // Получаем размер сжатого файла
    
    printf("Original size: %ld byte\n", initial_size);
    printf("Size after compression: %ld byte\n", compressed_size);
    
    // Вычисляем и выводим коэффициент сжатия
    if (initial_size > 0) {
        double compression_ratio = (double)compressed_size / initial_size;
        double compression_percentage = (1.0 - compression_ratio) * 100.0;
        printf("Compression Ratio: %.2f\n", compression_ratio);
        printf("Compression efficiency: %.1f%%\n", compression_percentage);
    }
    
    // Этап 8: Освобождение ресурсов
    destroy_tree(huffman_root);                  // Освобождаем дерево Хаффмана
    clear_encoding_table(encoding_data, 256);    // Освобождаем таблицу кодов
    fclose(input_stream);                        // Закрываем входной файл
    fclose(output_stream);                       // Закрываем выходной файл
    
    printf("Compression completed. The result is saved in %s\n", result_filename);
}

/*
Функция: Восстанавливает исходный файл из сжатого формата Хаффмана

  - compressed_filename: Путь к сжатому файлу
  - restored_filename:   Путь для сохранения восстановленного файла

*/
void perform_decompression(const char *compressed_filename, const char *restored_filename) {
    // Этап 1: Открытие файлов
    FILE *input_stream = fopen(compressed_filename, "rb");  // Открываем сжатый файл для чтения
    if (!input_stream) {
        printf("ERROR: Unable to open compressed file %s\n", compressed_filename);
        return;
    }
    
    FILE *output_stream = fopen(restored_filename, "wb");  // Открываем файл для восстановленных данных
    if (!output_stream) {
        printf("ERROR: Unable to create a recovered file %s\n", restored_filename);
        fclose(input_stream);  // Закрываем входной файл перед выходом
        return;
    }
    
    printf("File recovery in progress %s...\n", compressed_filename);
    
    // Этап 2: Чтение таблицы частот из начала сжатого файла
    // При сжатии мы записали таблицу частот в начало файла
    unsigned int frequency_data[256];
    
    // Читаем 256 значений типа unsigned int - это таблица частот
    if (fread(frequency_data, sizeof(unsigned int), 256, input_stream) != 256) {
        printf("ERROR: Invalid compressed file format\n");
        fclose(input_stream);
        fclose(output_stream);
        return;
    }
    
    // Этап 3: Построение дерева Хаффмана
    // Используем ту же функцию, что и при сжатии, с теми же частотами
    TreeNode *huffman_root = construct_huffman_tree(frequency_data);
    if (huffman_root == NULL) {
        printf("ERROR: Unable to build Huffman tree\n");
        fclose(input_stream);
        fclose(output_stream);
        return;
    }
    
    // Этап 4: Декодирование данных
    unsigned char bit_accumulator = 0;  // Буфер для чтения битов
    int accumulated_bits = 0;           // Счетчик оставшихся битов в буфере
    int end_of_file = 0;                // Флаг конца файла
    
    TreeNode *current_position = huffman_root;  // Начинаем обход с корня дерева
    
    // Предварительный подсчет общего количества символов
    // Это нужно для проверки корректности декодирования
    int total_symbols = 0;
    for (int i = 0; i < 256; i++) {
        total_symbols += frequency_data[i];  // Суммируем все частоты
    }
    
    int decoded_symbols = 0;  // Счетчик успешно декодированных символов
    
    // Читаем биты из файла до тех пор, пока не достигнем конца файла
    // или не декодируем все символы
    while (!end_of_file) {
        // Читаем один бит из файла
        unsigned char bit_value = input_bit(input_stream, &bit_accumulator, 
                                           &accumulated_bits, &end_of_file);
        
        // Если не достигли конца файла
        if (!end_of_file) {
            // Переходим по дереву в зависимости от прочитанного бита
            if (bit_value == 0) {
                current_position = current_position->child_left;   // Бит 0 - идем влево
            } else {
                current_position = current_position->child_right;  // Бит 1 - идем вправо
            }
            
            // Если достигли листа дерева (узел без потомков)
            if (current_position->child_left == NULL && 
                current_position->child_right == NULL) {
                // Записываем декодированный символ в выходной файл
                fputc(current_position->character, output_stream);
                
                // Возвращаемся к корню дерева для декодирования следующего символа
                current_position = huffman_root;
                decoded_symbols++;  // Увеличиваем счетчик декодированных символов
                
                // Дополнительная проверка: если декодировали все символы, выходим из цикла
                // Это предотвращает чтение лишних битов (например, дополняющих нулей)
                if (decoded_symbols >= total_symbols) {
                    break;
                }
            }
        }
    }
    
    // Вывод результатов декодирования
    printf("Recovery completed. The result is saved in %s\n", restored_filename);
    printf("Characters decoded: %d from %d\n", decoded_symbols, total_symbols);
    
    // Этап 5: Освобождение ресурсов
    destroy_tree(huffman_root);  // Освобождаем дерево Хаффмана
    fclose(input_stream);        // Закрываем входной файл
    fclose(output_stream);       // Закрываем выходной файл
}
