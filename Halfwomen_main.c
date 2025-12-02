//Halfwomen(library).c - Основной исполняемый файл программы сжатия Хаффмана


/*
  Использование:
    ./Halfwomen(library) encode исходный_файл сжатый_файл
    ./Halfwomen(library) decode сжатый_файл восстановленный_файл
    ./Halfwomen(library) test исходный_файл сжатый_файл восстановленный_файл
*/

//!!!!Включение необходимых заголовочных файлов!!!!

#include <stdio.h>   
#include <string.h> 
#include "Halfmen_library.h"

//!!!!Прототипы внутренних функций

/*
Функция: Выполняет полный цикл сжатия и восстановления для тестирования
  - initial_file:   Путь к исходному тестовому файлу
  - compressed_file: Путь для временного хранения сжатого файла
  - recovered_file:  Путь для временного хранения восстановленного файла
*/
void validate_compression_cycle(const char *initial_file, 
                                const char *compressed_file, 
                                const char *recovered_file);

/*
Функция: Основная логика обработки аргументов командной строки
  - argument_count:  Количество аргументов командной строки
  - argument_values: Массив строк-аргументов
Возвращает:
  - 0: Успешное выполнение
  - 1: Ошибка в аргументах командной строки
*/
int execute_program(int argument_count, char *argument_values[]);

//!!!!Реализация функции тестирования!!!!

/*
Функция:Тестирует полный цикл сжатия и восстановления файла
Алгоритм:
  1. Сжимает исходный файл в сжатый файл
  2. Восстанавливает файл из сжатого формата
  3. Сравнивает исходный и восстановленный файлы побайтово
  4. Выводит результат сравнения
  - initial_file:   Путь к исходному файлу (эталон)
  - compressed_file: Путь для сохранения сжатого файла
  - recovered_file:  Путь для сохранения восстановленного файла
*/
void validate_compression_cycle(const char *initial_file, 
                                const char *compressed_file, 
                                const char *recovered_file) {
    printf("Running a compression cycle test on a file: %s\n", initial_file);
    
    // Шаг 1: Сжатие исходного файла
    printf("1. File compression in progress...\n");
    perform_compression(initial_file, compressed_file);
    
    // Шаг 2: Восстановление файла из сжатого формата
    printf("2. File recovery in progress...\n");
    perform_decompression(compressed_file, recovered_file);
    
    // Шаг 3: Сравнение исходного и восстановленного файлов
    printf("3. Comparing files...\n");
    
    // Открываем оба файла для бинарного чтения
    FILE *original_data = fopen(initial_file, "rb");
    FILE *restored_data = fopen(recovered_file, "rb");
    
    // Проверяем, удалось ли открыть оба файла
    if (!original_data || !restored_data) {
        printf("ERROR: Unable to open files for comparison\n");
        
        // Закрываем файлы, если они были открыты
        if (original_data) fclose(original_data);
        if (restored_data) fclose(restored_data);
        
        return;
    }
    
    int is_identical = 1;      // Флаг идентичности файлов (1 - идентичны, 0 - различаются)
    int char_original, char_restored;  // Переменные для чтения символов
    
    // Сравниваем файлы побайтово
    while ((char_original = fgetc(original_data)) != EOF && 
           (char_restored = fgetc(restored_data)) != EOF) {
        
        // Если найдено несовпадение
        if (char_original != char_restored) {
            is_identical = 0;  // Устанавливаем флаг неидентичности
            break;             // Прерываем сравнение
        }
    }
    
    // Дополнительная проверка: оба файла должны закончиться одновременно
    // Если один файл закончился, а в другом еще есть данные - файлы разной длины
    if (is_identical && (fgetc(original_data) != EOF || fgetc(restored_data) != EOF)) {
        is_identical = 0;  // Файлы разной длины
    }
    
    // Закрываем файлы
    fclose(original_data);
    fclose(restored_data);
    
    // Выводим результат тестирования
    if (is_identical) {
        printf("\nTEST PASSED SUCCESSFULLY!\n");
        printf("  The original and restored files are completely identical.\n");
        printf("  The Huffman algorithm works correctly.\n");
    } else {
        printf("\nTEST FAILED!\n");
        printf("  Discrepancies were found between the original and recovered files.\n");
        printf("  The algorithm is not working correctly.\n");
    }
}

//!!!! Реализация основной логики программы!!!!

/*
Функция:Обрабатывает аргументы командной строки и запускает соответствующий режим
Алгоритм:
  1. Проверяет минимальное количество аргументов
  2. Определяет режим работы по первому аргументу
  3. Проверяет количество аргументов для выбранного режима
  4. Вызывает соответствующую функцию
  5. Возвращает код завершения
  - argument_count:  Количество аргументов (argc)
  - argument_values: Массив аргументов (argv)
Возвращает:
  - 0: Программа выполнена успешно
  - 1: Ошибка в аргументах командной строки
*/
int execute_program(int argument_count, char *argument_values[]) {
    // Проверка минимального количества аргументов
    // Программа должна получать как минимум: имя_программы команда файл1 файл2
    if (argument_count < 4) {
        printf("ERROR: Incorrect number of command line arguments!\n\n");
        
        // Вывод справки по использованию программы
        printf("USING THE PROGRAM:\n");
        printf("  File compression:            %s encode <source_file> <compressed_file>\n", 
               argument_values[0]);
        printf("  File recovery:    %s decode <compressed_file> <recovered_file>\n", 
               argument_values[0]);
        printf("  Testing (full cycle): %s test <source_file> <compressed_file> <recovered_file>\n", 
               argument_values[0]);
        printf("\nEXAMPLES:\n");
        printf("  %s encode document.txt compressed.huf\n", argument_values[0]);
        printf("  %s decode compressed.huf restored.txt\n", argument_values[0]);
        printf("  %s test document.txt compressed.huf restored.txt\n", argument_values[0]);
        
        return 1;  // Возвращаем код ошибки
    }
    
    // Определение и выполнение команды
    
    // РЕЖИМ: СЖАТИЕ ФАЙЛА (ENCODE)
    if (strcmp(argument_values[1], "encode") == 0) {
        // Проверка количества аргументов для режима encode
        if (argument_count != 4) {
            printf("ERROR: Two files must be specified for compression mode\n");
            printf("Usage: %s encode <  source_file> <compressed_file>\n", 
                   argument_values[0]);
            return 1;
        }
        
        // Вызов функции сжатия
        perform_compression(argument_values[2], argument_values[3]);
    }
    
    // РЕЖИМ: ВОССТАНОВЛЕНИЕ ФАЙЛА (DECODE)
    else if (strcmp(argument_values[1], "decode") == 0) {
        // Проверка количества аргументов для режима decode
        if (argument_count != 4) {
            printf("ERROR: Recovery mode requires two files.\n");
            printf("Usage: %s decode <compressed_file> <recovered_file>\n", 
                   argument_values[0]);
            return 1;
        }
        
        // Вызов функции восстановления
        perform_decompression(argument_values[2], argument_values[3]);
    }
    
    // РЕЖИМ: ТЕСТИРОВАНИЕ (TEST)
    else if (strcmp(argument_values[1], "test") == 0) {
        // Проверка количества аргументов для режима test
        if (argument_count != 5) {
            printf("ERROR: Three files must be specified for testing mode\n");
            printf("Usage: %s test <source_file> <compressed_file> <recovered_file>\n", 
                   argument_values[0]);
            return 1;
        }
        
        // Вызов функции тестирования
        validate_compression_cycle(argument_values[2], argument_values[3], argument_values[4]);
    }
    
    // НЕИЗВЕСТНАЯ КОМАНДА
    else {
        printf("ERROR: Unknown command '%s'\n", argument_values[1]);
        printf("Valid commands: encode, decode, test\n");
        return 1;
    }
    
    return 0;  // Успешное завершение программы
}

//Точка входа в программу!!!!
int main(int argc, char *argv[]) {
    // Передаем управление основной логике программы
    return execute_program(argc, argv);
}

/* 

1. КОМПИЛЯЦИЯ ПРОГРАММЫ:

   gcc Halfwomen\(library\).c tree\(functions\).c -o huffman_program
   
   Для Windows:
      gcc Halfwomen_main.c tree_function.c -o Halfwomen_main.exe

2. ИСПОЛЬЗОВАНИЕ ПРОГРАММЫ:


   а) РЕЖИМ СЖАТИЯ (ENCODE):

      ./Halfwomen_main.exe encode input.txt encode.bin
      

   б) РЕЖИМ ВОССТАНОВЛЕНИЯ (DECODE):

      ./Halfwomen_main.exe decode encode.bin decode.txt
      

   в) РЕЖИМ ТЕСТИРОВАНИЯ (TEST):
      ./huffmann.exe test input.txt encode.bin decode.txt

3. ФОРМАТ СЖАТЫХ ФАЙЛОВ:

   Сжатый файл имеет следующую структуру:
   - Первые 256 * sizeof(unsigned int) байт: таблица частот символов
   - Остальная часть: битовый поток с кодами Хаффмана


4. ОГРАНИЧЕНИЯ ПРОГРАММЫ:

   - Программа работает с любыми бинарными файлами
   - Максимальный размер файла ограничен системными возможностями
   - Эффективность сжатия зависит от распределения символов в файле


*/