#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "createfield.h"

// Реализация команды MOVE

// Проверка на указатели на динозавра и поле не NULL
int move_dino(struct Dino *dino, struct Field *field, Direction direction) {
    if (dino == NULL || field == NULL) {
        printf("Ошибка - динозавр или поле неиницилизированны");
        return 0;
    }
    
    
    // Сохраняем текущую позицию
    int old_x = dino->x;
    int old_y = dino->y;
    //Сохраняет текущие координаты динозавра и инициализирует новые координаты теми же значениями. Чтобы потом восстановить клетку, на которой стоял динозавр, и вычислить новую позицию.
    int new_x = old_x;
    int new_y = old_y;
    
    // Вычисляем новые координаты в зависимости от направления (с учетом топологии тора)
    switch (direction) {
        case DIR_UP:
            new_y = (old_y - 1 + field->height) % field->height; // вычитаем 1 (вверх), а потом прибавляем высоту чтобы избежать отрицательных значений; делим на высоту поля
            break;
        case DIR_DOWN:
            new_y = (old_y + 1) % field->height; // прибавляем 1 (вниз), делим на высоту поля
            break;
        case DIR_LEFT:
            new_x = (old_x - 1 + field->width) % field->width; // вычитем 1 (влево), прибаляем высоту чтобы избежать отрицательных значений; делим на ширину поля
            break;
        case DIR_RIGHT:
            new_x = (old_x + 1) % field->width; // прибавляем 1 (вправо), делим на ширину поля
            break;
        default: // Если case не равен 0,1,2,3
            printf("Ошибка - неизветсное направление\n");
            return 0;
    }
    
    // Проверяем, можно ли переместиться в новую клетку
    char target_cell = field->tiles[new_y][new_x];
    
    // Проверяем препятствия
    if (target_cell == '%') {  // Яма - возвращает ошибку, завершает программу
        printf("Ошибка - невозможно прыгнуть в яму (%d,%d)\n", new_x, new_y);
        return 0;
    } else if (target_cell == '^' || target_cell == '&' || target_cell == '@') {
        // Гора, дерево или камень - возвращает предупреждение
        printf("Предупреждение - нельзя войти в препядствие '%c' at (%d,%d), проигнорировать\n", 
               target_cell, new_x, new_y);
        return 1;  // Игнорируем команду, но не завершаем программу
    }
    
    // Восстанавливаем старую клетку
    field->tiles[old_y][old_x] = dino->steppedCell;
    
    // Сохраняем символ новой клетки и перемещаем динозавра
    dino->steppedCell = field->tiles[new_y][new_x];
    field->tiles[new_y][new_x] = '#';
    
    // Обновляем координаты динозавра
    dino->x = new_x;
    dino->y = new_y;
    
    return 1;
}

// Реализация команды PRNT

int paint_cell(struct Dino *dino, struct Field *field, char color) {
    if (dino == NULL || field == NULL) {
        printf("Ошибка - динозавр или поле не инициализированны\n");
        return 0;
    }
    
    // Проверяем, что цвет - строчная латинская буква
    if (color < 'a' || color > 'z') {
        printf("Ошибка - неправильный цвет '%c'. Должно быть в промежутке (a-z)\n", color);
        return 0;
    }
    
    // Проверяем, что динозавр находится в допустимых пределах поля
    if (dino->x < 0 || dino->x >= field->width || dino->y < 0 || dino->y >= field->height) {
        printf("Ошибка - динозавр за границами\n");
        return 0;
    }
    
    // Окрашиваем клетку, на которой стоит динозавр
    // Обновляем steppedCell, чтобы при уходе динозавра клетка оставалась окрашенной
    dino->steppedCell = color;
    field->tiles[dino->y][dino->x] = color;
    
    printf("Клетка (%d,%d) закрашена '%c'\n", dino->x, dino->y, color);
    return 1;
}


// Вспомогательная функция для преобразования строки (UP, DOWN, LEFT, RIGHT) в направление\тип Direction (DIR_UP, ...)
Direction parse_direction(const char *dir_str) {
    if (strcmp(dir_str, "UP") == 0) return DIR_UP;
    if (strcmp(dir_str, "DOWN") == 0) return DIR_DOWN;
    if (strcmp(dir_str, "LEFT") == 0) return DIR_LEFT;
    if (strcmp(dir_str, "RIGHT") == 0) return DIR_RIGHT;
    return -1; // Неизвестное направление
} 

// Вспомогательная функция для обработки команды PAINT из строки
int handle_paint_command(struct Dino *dino, struct Field *field, const char *color_str) {
    if (color_str == NULL || strlen(color_str) != 1) {
        printf("Ошибка - неправильный ввод цвета. Используй - PAINT <letter>\n");
        return 0;
    }
    
    char color = color_str[0];
    return paint_cell(dino, field, color);
}

int create_obstacle(struct Dino *dino, struct Field *field, Direction direction, char obstacle_type) {
    if (dino == NULL || field == NULL) {
        printf("Ошибка - динозавр или поле неиницилизированно\n");
        return 0;
    }

    // Вычисляем координаты целевой клетки
    int target_x = dino->x;
    int target_y = dino->y;
    
    switch (direction) {
        case DIR_UP:
            target_y = (dino->y - 1 + field->height) % field->height;
            break;
        case DIR_DOWN:
            target_y = (dino->y + 1) % field->height;
            break;
        case DIR_LEFT:
            target_x = (dino->x - 1 + field->width) % field->width;
            break;
        case DIR_RIGHT:
            target_x = (dino->x + 1) % field->width;
            break;
        default:
            printf("Ошибка - неопознанное направление\n");
            return 0;
    }
    
    // Проверяем, что целевая клетка не занята динозавром
    if (target_x == dino->x && target_y == dino->y) {
        printf("Ошибка - нельзя создавать препятсятвие на месте динозавра\n");
        return 0;
    }
    
    char current_cell = field->tiles[target_y][target_x];
    
    // Обрабатываем специальные случаи взаимодействия
    if (obstacle_type == '^' && current_cell == '%') {
        // Насыпание горы в яму - засыпаем яму
        field->tiles[target_y][target_x] = '_';
        printf("Дыра заполнена горой (%d,%d), ячейка пуста\n", target_x, target_y);
    } else if (obstacle_type == '%' && current_cell == '^') {
        // Копание ямы в горе - убираем гору
        field->tiles[target_y][target_x] = '%';
        printf("Гора выкопана в (%d,%d), теперь клетка - дырка\n", target_x, target_y);
    } else if (current_cell == '_' || (current_cell >= 'a' && current_cell <= 'z')) {
        // Создаем препятствие на пустой или окрашенной клетке
        field->tiles[target_y][target_x] = obstacle_type;
        printf("%s created at (%d,%d)\n", 
               (obstacle_type == '%') ? "Дыра" : "Гора", target_x, target_y);
    } else {
        // Нельзя создать препятствие на другом препятствии (кроме специальных случаев выше)
        printf("Осторожно - невозможно создать %s на данном препятствии '%c' на (%d,%d)\n",
               (obstacle_type == '%') ? "hole" : "mound", current_cell, target_x, target_y);
        return 1; // Игнорируем команду, но не завершаем программу
    }
    
    return 1;
}

// Функция для обработки команды DIG
int handle_dig_command(struct Dino *dino, struct Field *field, const char *direction_str) {
    if (dino == NULL || field == NULL) {
        printf("Ошибка - динозавр или поле неиницилизированны\n");
        return 0;
    }
    
    Direction dir = parse_direction(direction_str);
    if (dir == -1) {
        printf("Ошибка - неопознанное направление '%s'\n", direction_str);
        return 0;
    }
    
    return create_obstacle(dino, field, dir, '%');
}

// Функция для обработки команды MOUND
int handle_mound_command(struct Dino *dino, struct Field *field, const char *direction_str) {
    if (dino == NULL || field == NULL) {
        printf("Ошибка - динозавр или поле неиницилизированно\n");
        return 0;
    }
    
    Direction dir = parse_direction(direction_str);
    if (dir == -1) {
        printf("Ошибка - неопознанное направление '%s'\n", direction_str);
        return 0;
    }
    
    return create_obstacle(dino, field, dir, '^');
}

// Реализация команды JUMP
int jump_dino(struct Dino *dino, struct Field *field, Direction direction, int n) {
    // Проверка на нулевые указатели динозавра и поля
    if (dino == NULL || field == NULL) {
        printf("Ошибка - Не задано поле или динозавр");
        return 0;  // Возвращаем 0 при ошибке
    }
    
    // Проверка что длина прыжка положительна
    if (n <= 0) {
        printf("Ошибка - можно прыгать только на положительную величину\n");
        return 0;  // Возвращаем 0 при недопустимой длине прыжка
    }

    // Инициализация переменных для изменения координат
    int dx = 0, dy = 0;
    
    // Определение направления движения по осям X и Y
    switch (direction) {
        case DIR_UP:    dy = -1; break;    // Вверх - уменьшение Y
        case DIR_DOWN:  dy = 1; break;     // Вниз - увеличение Y
        case DIR_LEFT:  dx = -1; break;    // Влево - уменьшение X
        case DIR_RIGHT: dx = 1; break;     // Вправо - увеличение X
        default: 
            printf("Ошибка - неопознанное направление\n");
            return 0;  // Возвращаем 0 при неизвестном направлении
    }

    // Сохраняем текущие координаты динозавра
    int old_x = dino->x;
    int old_y = dino->y;
    
    // Изначально планируем прыгнуть на n клеток
    int target_step = n;

    // Проверяем путь на наличие препятствий - проходим по всем клеткам на пути прыжка
    for (int i = 1; i <= n; i++) {
        // Вычисляем координаты текущей проверяемой клетки с учетом тороидальной топологии
        int new_x = (old_x + i * dx + field->width) % field->width;
        int new_y = (old_y + i * dy + field->height) % field->height;

        char cell = field->tiles[new_y][new_x];
        // Если встречаем гору, дерево или камень, уменьшаем длину прыжка до предыдущей клетки
        if (cell == '^' || cell == '&' || cell == '@') {
            target_step = i - 1;  // Останавливаемся перед препятствием
            printf("Осторожно - остановка возле препядствия '%c' на (%d,%d), прыгать только %d шагами\n", 
               cell, new_x, new_y, target_step);
            break;  // Прерываем цикл проверки
        }
    }

    // Вычисляем конечные координаты приземления с учетом тороидальной топологии
    int land_x = (old_x + target_step * dx + field->width) % field->width;
    int land_y = (old_y + target_step * dy + field->height) % field->height;

    // Проверяем, что клетка приземления не является ямой (только если прыжок состоялся)
    if (target_step >= 1 && field->tiles[land_y][land_x] == '%') {
        printf("Ошибка - нельзя прыгнуть в дыру на (%d,%d)\n", land_x, land_y);
        return 0;  // Возвращаем 0 при попытке приземлиться в яму
    }

    // Если прыжок не состоялся (target_step = 0), остаемся на месте
    if (target_step == 0) {
        return 1;  // Возвращаем 1 - команда обработана (хоть и без перемещения)
    }

    // Восстанавливаем символ на старой позиции динозавра
    field->tiles[old_y][old_x] = dino->steppedCell;

    // Сохраняем символ новой клетки и размещаем динозавра на новой позиции
    dino->steppedCell = field->tiles[land_y][land_x];
    field->tiles[land_y][land_x] = '#';  // '#' - символ динозавра

    // Обновляем координаты динозавра в структуре
    dino->x = land_x;
    dino->y = land_y;

    return 1;  // Успешное выполнение команды
}

// Реализация команды GROW (создание дерева)
int grow_tree(struct Dino *dino, struct Field *field, Direction direction) {
    if (dino == NULL || field == NULL) {
        printf("Ошибка - динозавр или поле неиницилизированно\n");
        return 0;
    }

    // Вычисляем координаты целевой клетки
    int target_x = dino->x;
    int target_y = dino->y;
    
    switch (direction) {
        case DIR_UP:
            target_y = (dino->y - 1 + field->height) % field->height;
            break;
        case DIR_DOWN:
            target_y = (dino->y + 1) % field->height;
            break;
        case DIR_LEFT:
            target_x = (dino->x - 1 + field->width) % field->width;
            break;
        case DIR_RIGHT:
            target_x = (dino->x + 1) % field->width;
            break;
        default:
            printf("Ошибка - неопознанное направление\n");
            return 0;
    }
    
    // Проверяем, что целевая клетка пустая
    char current_cell = field->tiles[target_y][target_x];
    if (current_cell == '_' || (current_cell >= 'a' && current_cell <= 'z')) {
        field->tiles[target_y][target_x] = '&';
        printf("Дерево создано на (%d,%d)\n", target_x, target_y);
        return 1;
    } else {
        printf("Осторожно - невозможно поставить дерево на неподходящей клетке '%c' на (%d,%d)\n", 
               current_cell, target_x, target_y);
        return 1; // Игнорируем команду, но не завершаем программу
    }
}

// Реализация команды CUT (удаление дерева)
int cut_tree(struct Dino *dino, struct Field *field, Direction direction) {
    if (dino == NULL || field == NULL) {
        printf("Ошибка - динозавр или поле не инициализированны\n");
        return 0;
    }

    // Вычисляем координаты целевой клетки
    int target_x = dino->x;
    int target_y = dino->y;
    
    switch (direction) {
        case DIR_UP:
            target_y = (dino->y - 1 + field->height) % field->height;
            break;
        case DIR_DOWN:
            target_y = (dino->y + 1) % field->height;
            break;
        case DIR_LEFT:
            target_x = (dino->x - 1 + field->width) % field->width;
            break;
        case DIR_RIGHT:
            target_x = (dino->x + 1) % field->width;
            break;
        default:
            printf("Ошибка - неправильное направление\n");
            return 0;
    }
    
    // Проверяем, что в целевой клетке дерево
    char current_cell = field->tiles[target_y][target_x];
    if (current_cell == '&') {
        // Восстанавливаем пустую клетку
        field->tiles[target_y][target_x] = '_';
        printf("Дерево срублено на (%d,%d), клетка не подходит\n", target_x, target_y);
        return 1;
    } else {
        printf("Осторожно - нет деревью для срубки на (%d,%d), найдено '%c'\n", 
               target_x, target_y, current_cell);
        return 1; // Игнорируем команду, но не завершаем программу
    }
}

// Реализация команды MAKE (создание камня)
int make_stone(struct Dino *dino, struct Field *field, Direction direction) {
    if (dino == NULL || field == NULL) {
        printf("Ошибка - динозавр или поле не заданы\n");
        return 0;
    }

    // Вычисляем координаты целевой клетки
    int target_x = dino->x;
    int target_y = dino->y;
    
    switch (direction) {
        case DIR_UP:
            target_y = (dino->y - 1 + field->height) % field->height;
            break;
        case DIR_DOWN:
            target_y = (dino->y + 1) % field->height;
            break;
        case DIR_LEFT:
            target_x = (dino->x - 1 + field->width) % field->width;
            break;
        case DIR_RIGHT:
            target_x = (dino->x + 1) % field->width;
            break;
        default:
            printf("Ошибка - неизвестное направление\n");
            return 0;
    }
    
    // Проверяем, что целевая клетка пустая
    char current_cell = field->tiles[target_y][target_x];
    if (current_cell == '_' || (current_cell >= 'a' && current_cell <= 'z')) {
        field->tiles[target_y][target_x] = '@';
        printf("Камень сделан на (%d,%d)\n", target_x, target_y);
        return 1;
    } else {
        printf("Осторожно - невозможно разместить камень на неподходящей клетке '%c' на (%d,%d)\n", 
               current_cell, target_x, target_y);
        return 1; // Игнорируем команду, но не завершаем программу
    }
}

// Реализация команды PUSH (пинание камня)
int push_stone(struct Dino *dino, struct Field *field, Direction direction) {
    if (dino == NULL || field == NULL) {
        printf("Ошибка - динозавр или поле не задано\n");
        return 0;
    }

    // Вычисляем координаты камня
    int stone_x = dino->x;
    int stone_y = dino->y;
    
    switch (direction) {
        case DIR_UP:
            stone_y = (dino->y - 1 + field->height) % field->height;
            break;
        case DIR_DOWN:
            stone_y = (dino->y + 1) % field->height;
            break;
        case DIR_LEFT:
            stone_x = (dino->x - 1 + field->width) % field->width;
            break;
        case DIR_RIGHT:
            stone_x = (dino->x + 1) % field->width;
            break;
        default:
            printf("Ошибка - неизвестное направление\n");
            return 0;
    }
    
    // Проверяем, что в целевой клетке камень
    if (field->tiles[stone_y][stone_x] != '@') {
        printf("Осторожно - нет камня для перемещения на (%d,%d)\n", stone_x, stone_y);
        return 1; // Игнорируем команду, но не завершаем программу
    }
    
    // Вычисляем координаты следующей клетки за камнем
    int next_x = stone_x;
    int next_y = stone_y;
    
    switch (direction) {
        case DIR_UP:
            next_y = (stone_y - 1 + field->height) % field->height;
            break;
        case DIR_DOWN:
            next_y = (stone_y + 1) % field->height;
            break;
        case DIR_LEFT:
            next_x = (stone_x - 1 + field->width) % field->width;
            break;
        case DIR_RIGHT:
            next_x = (stone_x + 1) % field->width;
            break;
    }
    
    // Проверяем следующую клетку на препятствия
    char next_cell = field->tiles[next_y][next_x];
    if (next_cell == '&' || next_cell == '^' || next_cell == '@') {
        printf("Осторожно - нельзя передвинуть камень в препядствие '%c' на (%d,%d)\n", 
               next_cell, next_x, next_y);
        return 1; // Игнорируем команду, но не завершаем программу
    }
    
    // Перемещаем камень
    if (next_cell == '%') {
        // Камень попадает в яму - засыпаем яму
        field->tiles[next_y][next_x] = '_';
        printf("Камень перемещен в дыру на (%d,%d), дыра заполнена\n", next_x, next_y);
    } else {
        // Просто перемещаем камень
        field->tiles[next_y][next_x] = '@';
    }
    
    // Восстанавливаем исходную клетку (где был камень)
    field->tiles[stone_y][stone_x] = '_';
    
    printf("Камень перемещен из (%d,%d) в (%d,%d)\n", stone_x, stone_y, next_x, next_y);
    return 1;
}

// Обработчики команд из строки
int handle_grow_command(struct Dino *dino, struct Field *field, const char *direction_str) {
    Direction dir = parse_direction(direction_str);
    if (dir == -1) {
        printf("Ошибка - неизвестное направление '%s'\n", direction_str);
        return 0;
    }
    return grow_tree(dino, field, dir);
}

int handle_cut_command(struct Dino *dino, struct Field *field, const char *direction_str) {
    Direction dir = parse_direction(direction_str);
    if (dir == -1) {
        printf("Ошибка - неизвестное направление '%s'\n", direction_str);
        return 0;
    }
    return cut_tree(dino, field, dir);
}

int handle_make_command(struct Dino *dino, struct Field *field, const char *direction_str) {
    Direction dir = parse_direction(direction_str);
    if (dir == -1) {
        printf("Ошибка - неизвестное направление '%s'\n", direction_str);
        return 0;
    }
    return make_stone(dino, field, dir);
}

int handle_push_command(struct Dino *dino, struct Field *field, const char *direction_str) {
    Direction dir = parse_direction(direction_str);
    if (dir == -1) {
        printf("Ошибка - неизвестное направление '%s'\n", direction_str);
        return 0;
    }
    return push_stone(dino, field, dir);
}