// Структура поля (прямоугольника), где width - ширина прямоугольника, height - высота прямоугольника
struct Field {
    char **tiles;
    int width, height;
};

// Сруктура нашего динозавра. x, y - координаты динозавра по горизонтали и вертикали соответственно.
struct Dino {
    int x, y;
    char steppedCell;  // Символ, который был на клетке до прихода динозавра;
};

// Набор целочисленных констант типа Direction чтобы потом писать было удобнее
typedef enum {
    DIR_UP, // = 0
    DIR_DOWN, // = 1
    DIR_LEFT, // = 2
    DIR_RIGHT // = 3
} Direction;

struct Painted_Field {
    
};

// Функция для инициализации динозавра
int init_dino(struct Dino*, struct Field *field, int, int);

// Функция для создания поля
int init_field(struct Field*, int, int);

// Функция для определения размеров поля, а затем для его инициализации (чтение первой строки)
int parse_line_init_field(char *const, struct Field*);

// Функция для определения позиции динозавра (чтение второй строки)
int parse_line_init_dino(char *const, struct Dino*, struct Field*);

// Функция для того, чтобы печатать поле в терминале
void print_field(struct Field*);

// Функция для освобождения памяти поля
void free_field(struct Field *);

// Функция для сохранения поля в файл
void print_field_in_file(struct Field* field, const char* filename);

// Функция перемещения
int move_dino(struct Dino *dino, struct Field *field, Direction direction);

// Функция для покраски 
/*int paint_dino(struct Dino *dino, struct Field *field, )*/

int handle_paint_command(struct Dino *dino, struct Field *field, const char *color_str);
Direction parse_direction(const char *dir_str);
int handle_dig_command(struct Dino *dino, struct Field *field, const char *direction_str);
int handle_mound_command(struct Dino *dino, struct Field *field, const char *direction_str);
int move_dino(struct Dino *dino, struct Field *field, Direction direction);
int jump_dino(struct Dino *dino, struct Field *field, Direction direction, int distance);
int paint_cell(struct Dino *dino, struct Field *field, char color);
int create_obstacle(struct Dino *dino, struct Field *field, Direction direction, char obstacle_type);
// Команды деревьев
int handle_grow_command(struct Dino *dino, struct Field *field, const char *direction_str);
int handle_cut_command(struct Dino *dino, struct Field *field, const char *direction_str);

// Команды камней  
int handle_make_command(struct Dino *dino, struct Field *field, const char *direction_str);
int handle_push_command(struct Dino *dino, struct Field *field, const char *direction_str);
// Дополнительные команды
int handle_exec_command(const char *filename);
int handle_load_command(struct Field *field, struct Dino *dino, const char *filename);