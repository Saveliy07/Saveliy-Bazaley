#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/*Попозже надо переименовать библиотеку, т.к. хочу сделать общую для всего*/
#include "createfield.h"

// Состояния программы
typedef enum {
    STATE_INITIAL = 0,    // Начальное состояние, ничего не создано
    STATE_FIELD_CREATED,  // Поле создано (после SIZE)
    STATE_DINO_CREATED    // Динозавр создан (после START)
} ProgramState;

// Результат проверки команды
typedef enum {
    CHECK_VALID,          // Команда корректна
    CHECK_INVALID,        // Команда некорректна
    CHECK_COMMENT         // Строка является комментарием
} CheckResult;

/**
 * Функция для проверки корректности команды согласно требованиям
 * 
 * @param line - строка с командой для проверки
 * @param current_state - текущее состояние программы
 * @param line_number - номер строки для вывода ошибок
 * @param new_state - указатель для возврата нового состояния программы
 * @return CheckResult - результат проверки команды
 */
CheckResult validate_command(const char *line, ProgramState current_state, int line_number, ProgramState *new_state) {
    // Пропускаем пустые строки
    if (line[0] == '\0' || line[0] == '\n') {
        return CHECK_VALID;
    }
    
    // Проверяем, является ли строка комментарием
    if (strncmp(line, "//", 2) == 0) {
        return CHECK_COMMENT;
    }
    
    // Проверяем наличие левых пробелов
    if (line[0] == ' ' || line[0] == '\t') {
        printf("Ошибка на линии %d: Левое место не допустимо\n", line_number);
        return CHECK_INVALID;
    }
    
    // Извлекаем первое слово (команду)
    char command[32];
    if (sscanf(line, "%31s", command) != 1) {
        printf("Ошибка на линии %d: неподходящая команада\n", line_number);
        return CHECK_INVALID;
    }
    
    // Проверяем последовательность команд в зависимости от текущего состояния
    switch (current_state) {
        case STATE_INITIAL:
            // В начальном состоянии разрешена только команда SIZE
            if (strcmp(command, "SIZE") == 0) {
                *new_state = STATE_FIELD_CREATED;
                return CHECK_VALID;
            } else {
                printf("Ошибка на линии %d: первая раскоменченная линия должна быть SIZE, получено '%s'\n", 
                       line_number, command);
                return CHECK_INVALID;
            }
            break;
            
        case STATE_FIELD_CREATED:
            // После SIZE разрешены только START или комментарии
            if (strcmp(command, "START") == 0) {
                *new_state = STATE_DINO_CREATED;
                return CHECK_VALID;
            } else if (strcmp(command, "SIZE") == 0) {
                printf("Ошибка на линии %d: SIZE комманда уже использованна\n", line_number);
                return CHECK_INVALID;
            } else {
                printf("Ошибка на линии %d: Обнаружен START после SIZE, полученно '%s'\n", 
                       line_number, command);
                return CHECK_INVALID;
            }
            break;
            
        case STATE_DINO_CREATED:
            // После START разрешены команды действий с динозавром
            if (strcmp(command, "SIZE") == 0) {
                printf("Ошибка на линии %d: SIZE комманда уже использованна\n", line_number);
                return CHECK_INVALID;
            } else if (strcmp(command, "START") == 0) {
                printf("Ошибка на линии %d: START комманда уже использованна\n", line_number);
                return CHECK_INVALID;
            } else if (strcmp(command, "MOVE") == 0 || 
                       strcmp(command, "JUMP") == 0 || 
                       strcmp(command, "PAINT") == 0) {
                *new_state = STATE_DINO_CREATED; // Состояние не меняется
                return CHECK_VALID;
            } else {
                printf("Ошибка на линии %d: Неизвестная команда '%s'\n", line_number, command);
                return CHECK_INVALID;
            }
            break;
            
        default:
            printf("Ошибка на линии %d: неправильный статус программы\n", line_number);
            return CHECK_INVALID;
    }
}

// Условная компиляция для разных операционных систем
#ifdef _WIN32
    #include <windows.h>   // Windows-specific headers for Sleep()
    #define CLEAR_CONSOLE() system("cls")  // Макрос для очистки консоли в Windows
    #define SLEEP_SECONDS(x) Sleep((x)*1000) // Макрос для задержки в миллисекундах
#else
    #include <unistd.h>    // Unix-specific headers for sleep()
    #define CLEAR_CONSOLE() system("clear") // Макрос для очистки консоли в Unix
    #define SLEEP_SECONDS(x) sleep(x)       // Макрос для задержки в секундах
#endif

// Функция для чтения первого слова
char *readthefirstword(char *line) {
    char *value = calloc(1024, sizeof(char));
    if (value == NULL) return NULL;
    
    char *p = line;
    int offset = 0;
    sscanf(p, "%s %n", value, &offset);
    // УБРАН free(value) - не освобождаем память здесь!
    return value;
}

// Функция удаления символа новой строки из строки
void remove_newline(char *str) {
    size_t len = strlen(str);     // Получение длины строки
    if (len > 0 && str[len-1] == '\n') { // Проверка на наличие символа новой строки
        str[len-1] = '\0';        // Замена символа новой строки на нуль-терминатор
    }
}


int main(int argn, char *args[]) {

    // Инициализация переменных для обработки аргументов командной строки
    int interval = 1;        // Задержка между обновлениями (по умолчанию 1 секунда)
    int no_display = 0;      // Флаг отключения визуализации (0 - включено)
    int no_save = 0;         // Флаг отключения сохранения в файл (0 - включено)
    int error_occurred = 0;  // Флаг возникновения ошибки (0 - нет ошибки)

    // Проверяет, что программа запущена ТОЛЬКО с двумя аргументами (input.txt и output.txt).
    if (argn < 3) {
        printf("Wrong arguments!\n");
        return 1;
    }

    // Парсинг опций командной строки (начиная с 3-го аргумента)
    for (int i = 3; i < argn; i++) {
        if (strcmp(args[i], "-interval") == 0 && i+1 < argn) { // Обработка интервала
            interval = atoi(args[i+1]);  // Преобразование строки в число
            if (interval <= 0) interval = 1; // Проверка корректности интервала
            i++;  // Пропуск следующего аргумента (значения интервала)
        } else if (strcmp(args[i], "-no-display") == 0) { // Обработка флага отключения дисплея
            no_display = 1;  // Установка флага отключения визуализации
        } else if (strcmp(args[i], "-no-save") == 0) { // Обработка флага отключения сохранения
            no_save = 1;     // Установка флага отключения сохранения
        }
    }

    // Открытие входного файла для чтения
    FILE *input_file = fopen(args[1], "r");
    // Открываем файл для ввода
    if (input_file == NULL) {
        printf("Ошибка открытия входного файла");
        fclose(input_file);
        return 1;
    }

    // Объявление переменных для работы с программой
    char line[1024];
    struct Field field;
    struct Dino dino;

     // Флаги состояния программы
    int field_initialized = 0;
    int dino_initialized = 0;
    int line_number = 0;


    while (fgets(line, sizeof(line), input_file) && !error_occurred) {
        line_number++;       // Увеличение счетчика строк
        remove_newline(line); // Удаление символа новой строки

        // Пропуск комментариев и пустых строк
        if (strncmp(line, "//", 2) == 0 || line[0] == '\0') {
            continue;  // Переход к следующей итерации цикла
        }

        // Проверка на наличие левых пробелов (отступов)
        if (line[0] == ' ' || line[0] == '\t') {
            printf("Ошибка на линии %d: левое место не допустимо\n", line_number);
            error_occurred = 1;  // Установка флага ошибки
            continue;            // Переход к следующей итерации
        }

        // Извлечение первого слова (команды) из строки
        char *action = calloc(1024, sizeof(char));
        action = readthefirstword(line);

        // Обработка команды SIZE
        if (strcmp(action, "SIZE") == 0) {
            int config = parse_line_init_field(line, &field);
            if (config == 0) {
                printf("Не создано поле\n");
                return 1;
            } 
            field_initialized = 1;
            printf(" Поле инициализированны : %dx%d\n", field.width, field.height);
        }
        
        // Обработка команды START
        else if (strcmp(action, "START") == 0) {
            int config = parse_line_init_dino(line, &dino, &field);
            if (config == 0) {
                printf("Не создан динозавр\n");
                return 1;
            } 
            dino_initialized = 1;
            printf("Динозавр расположен на (%d,%d)\n", dino.x, dino.y);
        }
        
        // Обработка команды MOVE
        else if (strcmp(action, "MOVE") == 0) {
            if (!field_initialized || !dino_initialized) {
                printf("Ошибка - динозавр и поле должны быть инициализированны до движения\n");
                free(action);
                continue;
            }
            
            // Извлекаем направление из строки
            char direction_str[10];
            if (sscanf(line, "%*s %9s", direction_str) == 1) {
                Direction dir = parse_direction(direction_str);
                if (dir != -1) {
                    if (!move_dino(&dino, &field, dir)) {
                        // Если move_dino вернул 0, значит критическая ошибка (падение в яму)
                        printf("Фатальная ошибка во время движения\n");
                        free(action);
                        fclose(input_file);
                        return 1;
                    }
                } else {
                    printf("Ошибка - неизвестное направление '%s'\n", direction_str);
                }
            } else {
                printf("Ошибка - неправильный MOVE коммандный формат. Использовать - MOVE <direction>\n");
            } 
        } 

        
        // Обработка команды JUMP - прыжок динозавра
        else if (strcmp(action, "JUMP") == 0) {
            if (!field_initialized || !dino_initialized) { // Проверка готовности поля и динозавра
                printf("Ошибка на линии %d: Поле и динозавр должны быть созданы раньше прыжка\n", line_number);
                error_occurred = 1;
                continue;
            }
            char dir_str[10];     // Буфер для хранения направления
            int n;                // Длина прыжка
            if (sscanf(line, "JUMP %s %d", dir_str, &n) != 2) { // Парсинг направления и длины прыжка
                printf("Ошибка на линии %d: Неправильная комманда прыжка\n", line_number);
                error_occurred = 1;
                continue;
            }
            Direction dir = parse_direction(dir_str); // Преобразование строки в направление
            if (dir == -1) {      // Проверка корректности направления
                printf("Ошибка на линии %d: Неизвестная направление на прыжок\n", line_number);
                error_occurred = 1;
                continue;
            }
            if (!jump_dino(&dino, &field, dir, n)) { // Вызов функции прыжка
                error_occurred = 1; // Установка флага ошибки при неудачном прыжке
                continue;
            }
        }

        // обработка команды PAINT
        else if (strcmp(action, "PAINT") == 0) {
            if (!field_initialized || !dino_initialized) {
                printf("Ошибка - динозавр и поле должны быть созданы до покраски\n");
                free(action);
                continue;
            }
            
        // Извлекаем аргумент цвета из строки
            char color_str[2];
            if (sscanf(line, "%*s %1s", color_str) == 1) {
                handle_paint_command(&dino, &field, color_str);
            } else {
                printf("Ошибка: неправильный PAINT формат комманды. Использовать: PAINT <letter>\n");
            }
        }   

        // Обработка команды DIG
        else if (strcmp(action, "DIG") == 0) {
            if (!field_initialized || !dino_initialized) {
                printf("Ошибка - Динозавр и поле должны быть созданы копания\n");
                free(action);
                continue;
            }
            
            char direction_str[10];
            if (sscanf(line, "%*s %9s", direction_str) == 1) {
                handle_dig_command(&dino, &field, direction_str);
            } else {
                printf("Ошибка: неправильный DIG коммандный формат. Использовать: DIG <direction>\n");
            }
        }

        // Обработка команды MOUND
        else if (strcmp(action, "MOUND") == 0) {
            if (!field_initialized || !dino_initialized) {
                printf("Ошибка - поле и динозавр должны быть созданы до горы\n");
                free(action);
                continue;
            }
            
            char direction_str[10];
            if (sscanf(line, "%*s %9s", direction_str) == 1) {
                handle_mound_command(&dino, &field, direction_str);
            } else {
                printf("Ошибка: неверный MOUND формат комманды. Использовать: MOUND <direction>\n");
            }
        }
        

        // Обработка команды GROW - создание дерева
        else if (strcmp(action, "GROW") == 0) {
            if (!field_initialized || !dino_initialized) {
                printf("Ошибка: поле и динозавр должны быть созданы до посадки деревья\n");
                free(action); // Освобождение памяти команды
                continue; // Пропуск команды
            }
            
            char direction_str[10]; // Буфер для хранения направления
            if (sscanf(line, "%*s %9s", direction_str) == 1) {
                handle_grow_command(&dino, &field, direction_str); // Вызов функции создания дерева
            } else {
                printf("Ошибка - неверный GROW формат коммады. Использовать: GROW <direction>\n"); // Ошибка формата
            }
        }

        // Обработка команды CUT - срубание дерева
        else if (strcmp(action, "CUT") == 0) {
            if (!field_initialized || !dino_initialized) {
                printf("Ошибка - динозавр и поле должны быть созданы до вырубки леса\n");
                free(action); // Освобождение памяти команды
                continue; // Пропуск команды
            }
            
            char direction_str[10]; // Буфер для хранения направления
            if (sscanf(line, "%*s %9s", direction_str) == 1) {
                handle_cut_command(&dino, &field, direction_str); // Вызов функции срубания дерева
            } else {
                printf("Ошибка - неверный CUT формат комманды. Использовать: CUT <direction>\n"); // Ошибка формата
            }
        }

        // ============================ НОВЫЕ КОМАНДЫ КАМНЕЙ ============================

        // Обработка команды MAKE - создание камня
        else if (strcmp(action, "MAKE") == 0) {
            if (!field_initialized || !dino_initialized) {
                printf("Ошибка - поле и динозавр должны быть созданы до постановки камней\n");
                free(action); // Освобождение памяти команды
                continue; // Пропуск команды
            }
            
            char direction_str[10]; // Буфер для хранения направления
            if (sscanf(line, "%*s %9s", direction_str) == 1) {
                handle_make_command(&dino, &field, direction_str); // Вызов функции создания камня
            } else {
                printf("Ошибка - неправильная MAKE формат комманды. Используй: MAKE <direction>\n"); // Ошибка формата
            }
        }

        // Обработка команды PUSH - перемещение камня
        else if (strcmp(action, "PUSH") == 0) {
            if (!field_initialized || !dino_initialized) {
                printf("Ошибка - поле и динозавр должны быть созданы до движка\n");
                free(action); // Освобождение памяти команды
                continue; // Пропуск команды
            }
            
            char direction_str[10]; // Буфер для хранения направления
            if (sscanf(line, "%*s %9s", direction_str) == 1) {
                handle_push_command(&dino, &field, direction_str); // Вызов функции перемещения камня
            } else {
                printf("Ошибка - неправильный PUSH формат комманды. Используй: PUSH <direction>\n"); // Ошибка формата
            }
        }


        // Обработка неизвестных команд
        else {
            printf("Ошибка на линии %d: Неизветсная комманда '%s'\n", line_number, action);
            error_occurred = 1;  // Установка флага ошибки
            continue;            // Переход к следующей итерации
        }

        // Визуализация состояния после каждой команды (если не отключено)
        if (!no_display && field_initialized) {
            CLEAR_CONSOLE();     // Очистка консоли
            printf("После комманды на линии %d:\n", line_number); // Вывод номера команды
            print_field(&field); // Вывод текущего состояния поля
            SLEEP_SECONDS(interval); // Задержка для визуализации
        }
    } // конец цикла обработки комманд

        /*// ВЫВОД ПОЛЯ В КОНСОЛЬ (теперь выполняется для КАЖДОЙ команды)
        if (field_initialized) {
            printf("\n--- Current field state after command '%s' ---\n", action); // потом можно убрать
            print_field(&field);
            printf("--- End of field ---\n\n"); // потом можно убрать*/

        // Сохранение поля в файл для вывода
        if (!no_save && field_initialized) {
            print_field_in_file(&field, args[2]);
        }

    // Освобождение ресурсов
    fclose(input_file);  // Закрытие входного файла

    if (field_initialized) {
        free_field(&field); // Освобождение памяти поля (если было создано)
    }

    return error_occurred; // Возврат кода завершения (0 - успех, 1 - ошибка)
}

// gcc -I. DinoLauncher.c commands.c createfield.c -o  DinoLauncher.exe
// ./DinoLauncher.exe input.txt output.txt