#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "createfield.h"

// Инициализирует динозавра (размещает его на поле)
int init_dino(struct Dino *dino, struct Field *field, int x, int y) {
    // Проверка входных параметров на NULL
    if (dino == NULL || field == NULL || field->tiles == NULL) {
        printf("Ошибка - неверные параметры для задания динозавра\n");
        return 0;
    }
    
    if (x < 0 || x >= field->width || y < 0 || y >= field->height) {
        printf("Ошибка - координаты динозавра за границами\n");
        return 0;
    }
    
    dino->x = x;
    dino->y = y;

    for (int i = 0; i < field->height; ++i) {

        // Проходимся по значениям ширины поля
        for (int j = 0; j < field->width; ++j) {

            // Проверяем совпадение координат точек и вставляем динозавра
            if (i == y && j == x) {
                dino->steppedCell = field->tiles[i][j];
                field->tiles[i][j] = '#';
            }
        }
    }
    return 1;
}

// Создает двумерный массив для игрового поля и инициализирует его пустыми клетками.
int init_field(struct Field *field, int w, int h) {
    field->width = w;
    field->height = h;
    char **rows = calloc(h, sizeof(char*));

    if (rows == NULL) {
        perror("Ошибка");
        return 0;
    }

    for (int i=0; i<h; ++i) {
        rows[i] = calloc(w, sizeof(char));
        if (rows[i] == NULL) {
            printf("Ошибка - Невозможно выделить память для строки \n");
        
            for (int j=0; j<w; ++j) {
                if (rows[j] == NULL) return 0;
            }

            free(rows);
            return 0;
        }
    }

    for (int i=0; i<h; ++i) {
        for (int j=0; j<w; ++j) {
            rows[i][j] = '_';
        }
    }

    field->tiles = rows;

    return 1;
}

int parse_line_init_field(char * const line, struct Field * field) {
    char *value = calloc(128, sizeof(char));
    char *p = line;
    int offset = 0;
    
    // Читаем команду
    if (sscanf(p, "%s %n", value, &offset) != 1) {
        free(value);
        return 0;  // Ошибка чтения команды
    }
    
    if (strcmp(value, "SIZE") == 0) {
        p += offset;
        int x, y;
        
        // Проверяем что прочитали 2 числа
        if (sscanf(p, "%d %d", &x, &y) != 2) {
            printf("Ошибка - необнаружено 2 числа после SIZE\n");
            free(value);
            return 0;
        }
        
        // Проверяем валидность размеров
        if (x < 10 || y < 10 || x > 100 || y > 100) {
            printf("Ошибка - неверный размер поля. Минимальная длина поля 10, максимальная 100.\n");
            free(value);
            return 0;
        }
        
        // Инициализируем поле и возвращаем конфиг
        if (init_field(field, x, y) == 0) {
            printf("Ошибка - невозможно создать поле\n");
            free(value);
            return 0;
        }
        
        return 1;  // Успешное завершение функции
        
    } else {
        printf("Ошибка - неизвестная комманда '%s'\n", value);
        free(value);
        return 0;
    }
}

int parse_line_init_dino(char * const line, struct Dino* dino, struct Field* field) {
    char *value = calloc(128, sizeof(char));
    char *p = line;
    int offset = 0;
    sscanf(p, "%s %n", value, &offset);

    // Проверяем является ли первое слово "START"
    if (strcmp(value, "START") == 0) {
        p += offset;
        int x, y;
        sscanf(p, "%d %d", &x, &y);

        // Проверяем, что после слова "START" написано 2 числа - координаты динозавра
        if (sscanf(p, "%d %d", &x, &y) != 2) {
            printf("Ошибка - необнаруженно два числа после размера SIZE\n");
            free(value);
            return 0;
        }

        // Проверяем, что значения координат входят в границы поля
        if ((x < 0) || (x >= field->width) || (y < 0) || (y >= field->height)) {
            printf("Ошибка - координаты динозавра должны быть внутри границ поля.\n");
            free(value);
            return 0;
        }

        if (init_dino(dino, field, x, y) == 0) {
            printf("Ошибка - не удалось установить позицию динозавра '%s'\n", value);
            free(value);
            return 0;
        }
        free(value);
        return 1;
    } else {
        printf("Ошибка - неизвестная комманда '%s'\n", value);
        free(value);
        return 0;
    }
}

void print_field(struct Field* field) {

    // Проходимся по значениям высоты поля
    for (int i = 0; i < field->height; ++i) {
// Проходимся по значениям ширины поля
        for (int j = 0; j < field->width; ++j) {
            printf("%c ", field->tiles[i][j]);
        }
        
        // Переходим на следующую строку
        printf("\n");
    }
}

void free_field(struct Field *field) {
    if (field->tiles != NULL) {
        for (int i = 0; i < field->height; ++i) {

            // Освобождаем каждую строку
            free(field->tiles[i]);
        }
    }
        // Освобождаем массив указателей
        free(field->tiles);
        field->tiles = NULL;
    }

void print_field_in_file(struct Field* field, const char* output) {
    if (field == NULL || field->tiles == NULL) {
        printf("Ошибка - Неизвестная - поле не создано\n");
        return;
    }
    
    if (output == NULL) {
        printf("Ошибка -  Output filename is NULL\n");
        return;
    }
    
    FILE *file = fopen(output, "w");
    if (file == NULL) {
        printf("Ошибка - невозможно открыть output file '%s'\n", output);
        return;
    }
    
    // Сохраняем поле в файл
    for (int i = 0; i < field->height; ++i) {
        for (int j = 0; j < field->width; ++j) {
            fprintf(file, "%c", field->tiles[i][j]);
        }
        fprintf(file, "\n");  // Новая строка после каждой строки поля
    }
    
    fclose(file);
    printf("Поле успешно загруженно в '%s'\n", output);
    }
