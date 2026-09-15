#include "state.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snake_utils.h"

/* Helper function definitions */
static void set_board_at(game_state_t *state, unsigned int row, unsigned int col, char ch);
static bool is_tail(char c);
static bool is_head(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static char head_to_body(char c);
static unsigned int get_next_row(unsigned int cur_row, char c);
static unsigned int get_next_col(unsigned int cur_col, char c);
static void find_head(game_state_t *state, unsigned int snum);
static char next_square(game_state_t *state, unsigned int snum);
static void update_tail(game_state_t *state, unsigned int snum);
static void update_head(game_state_t *state, unsigned int snum);

/* Task 1 */
game_state_t *create_default_state()
{
  // TODO: Implement this function.
  game_state_t *state = malloc(sizeof(game_state_t));
  if (state == NULL)
  {
    free(state);
    return NULL;
  }
  state->num_rows = 18;
  state->num_snakes = 1;
  unsigned int num_cols = 21;

  // Initialize the board
  state->board = malloc(sizeof(char *) * state->num_rows);
  if (state->board == NULL)
  {
    free(state);
    return NULL;
  }

  char *default_board[18] = {

      "####################",
      "#                  #",
      "# d>D    *         #",
      "#                  #",
      "#                  #",
      "#                  #",
      "#                  #",
      "#                  #",
      "#                  #",
      "#                  #",
      "#                  #",
      "#                  #",
      "#                  #",
      "#                  #",
      "#                  #",
      "#                  #",
      "#                  #",
      "####################"};

  // 初始化每一行
  for (unsigned int i = 0; i < state->num_rows; i++)
  {
    state->board[i] = malloc(sizeof(char) * num_cols);
    if (state->board[i] == NULL)
    {
      for (unsigned int j = 0; j < i; j++)
      {
        free(state->board[j]);
      }
      free(state->board);
      free(state);
      return NULL;
    }

    strcpy(state->board[i], default_board[i]);
  }

  state->snakes = malloc(sizeof(snake_t));
  if (state->snakes == NULL)
  {
    for (unsigned int i = 0; i < state->num_rows; i++)
    {
      free(state->board[i]);
    }
    free(state->board);
    free(state);
    return NULL;
  }

  snake_t *snake = malloc(sizeof(snake_t));
  if (snake == NULL)
  {
    free(state->snakes);
    for (unsigned int i = 0; i < state->num_rows; i++)
    {
      free(state->board[i]);
    }
    free(state->board);
    free(state);
    return NULL;
  }

  // 设置蛇的初始位置和状态
  snake->tail_row = 2;
  snake->tail_col = 2;
  snake->head_row = 2;
  snake->head_col = 4;
  snake->live = true;
  state->snakes[0] = *snake;
  state->num_snakes = 1;

  return state;
}

/* Task 2 */
void free_state(game_state_t *state)
{
  // TODO: Implement this function.
  for (unsigned int i = 0; i < state->num_rows; i++)
  {
    free(state->board[i]);
  }
  free(state->board);
  free(state->snakes);
  free(state);
  return;
}

/* Task 3 */
void print_board(game_state_t *state, FILE *fp)
{
  // TODO: Implement this function.
  for (unsigned int i = 0; i < state->num_rows; i++)
  {
    for (unsigned int j = 0; j < strlen(state->board[i]); j++)
    {
      fprintf(fp, "%c", state->board[i][j]);
    }
    fprintf(fp, "\n");
  }
  return;
}

/*
  Saves the current state into filename. Does not modify the state object.
  (already implemented for you).
*/
void save_board(game_state_t *state, char *filename)
{
  FILE *f = fopen(filename, "w");
  print_board(state, f);
  fclose(f);
}

/* Task 4.1 */

/*
  Helper function to get a character from the board
  (already implemented for you).
*/
char get_board_at(game_state_t *state, unsigned int row, unsigned int col) { return state->board[row][col]; }

/*
  Helper function to set a character on the board
  (already implemented for you).
*/
static void set_board_at(game_state_t *state, unsigned int row, unsigned int col, char ch)
{
  state->board[row][col] = ch;
}

/*
  Returns true if c is part of the snake's tail.
  The snake consists of these characters: "wasd"
  Returns false otherwise.
*/
static bool is_tail(char c)
{
  // TODO: Implement this function.
  if (c == 'w' || c == 'a' || c == 's' || c == 'd')
  {
    return true;
  }
  return false;
}

/*
  Returns true if c is part of the snake's head.
  The snake consists of these characters: "WASDx"
  Returns false otherwise.
*/
static bool is_head(char c)
{
  // TODO: Implement this function.
  if (c == 'W' || c == 'A' || c == 'S' || c == 'D' || c == 'x')
  {
    return true;
  }

  return false;
}

/*
  Returns true if c is part of the snake.
  The snake consists of these characters: "wasd^<v>WASDx"
*/
static bool is_snake(char c)
{
  // TODO: Implement this function.
  if (is_tail(c) || is_head(c) || c == '^' || c == '<' || c == '>' || c == 'v')
  {
    return true;
  }
  return false;
}

/*
  Converts a character in the snake's body ("^<v>")
  to the matching character representing the snake's
  tail ("wasd").
*/
static char body_to_tail(char c)
{
  // TODO: Implement this function.
  if (c == '^')
  {
    return 'w';
  }
  else if (c == '<')
  {
    return 'a';
  }
  else if (c == 'v')
  {
    return 's';
  }
  else if (c == '>')
  {
    return 'd';
  }
  return '?';
}

/*
  Converts a character in the snake's head ("WASD")
  to the matching character representing the snake's
  body ("^<v>").
*/
static char head_to_body(char c)
{
  // TODO: Implement this function.
  if (c == 'W')
  {
    return '^';
  }
  else if (c == 'A')
  {
    return '<';
  }
  else if (c == 'S')
  {
    return 'v';
  }
  else if (c == 'D')
  {
    return '>';
  }
  return '?';
}

/*
  Returns cur_row + 1 if c is 'v' or 's' or 'S'.
  Returns cur_row - 1 if c is '^' or 'w' or 'W'.
  Returns cur_row otherwise.
*/
static unsigned int get_next_row(unsigned int cur_row, char c)
{
  // TODO: Implement this function.
  if (c == 'v' || c == 's' || c == 'S')
  {
    return cur_row + 1;
  }
  else if (c == '^' || c == 'w' || c == 'W')
  {
    return cur_row - 1;
  }
  return cur_row;
}

/*
  Returns cur_col + 1 if c is '>' or 'd' or 'D'.
  Returns cur_col - 1 if c is '<' or 'a' or 'A'.
  Returns cur_col otherwise.
*/
static unsigned int get_next_col(unsigned int cur_col, char c)
{
  // TODO: Implement this function.
  if (c == '>' || c == 'd' || c == 'D')
  {
    return cur_col + 1;
  }
  else if (c == '<' || c == 'a' || c == 'A')
  {
    return cur_col - 1;
  }
  return cur_col;
}

/*
  Task 4.2

  Helper function for update_state. Return the character in the cell the snake is moving into.

  This function should not modify anything.
*/
static char next_square(game_state_t *state, unsigned int snum)
{
  // TODO: Implement this function.
  unsigned int head_row = state->snakes[snum].head_row;
  unsigned int head_col = state->snakes[snum].head_col;
  char head_char = get_board_at(state, head_row, head_col); // Get the character at the head position

  // Get next row and col
  unsigned int next_row = get_next_row(head_row, head_char);
  unsigned int next_col = get_next_col(head_col, head_char);
  if (next_row < state->num_rows && next_col < strlen(state->board[next_row]))
  {
    return get_board_at(state, next_row, next_col);
  }
  return '?';
}

/*
  Task 4.3

  Helper function for update_state. Update the head...

  ...on the board: add a character where the snake is moving

  ...in the snake struct: update the row and col of the head

  Note that this function ignores food, walls, and snake bodies when moving the head.
*/
static void update_head(game_state_t *state, unsigned int snum)
{
  // TODO: Implement this function.
  unsigned int head_row = state->snakes[snum].head_row;
  unsigned int head_col = state->snakes[snum].head_col;
  char head_char = get_board_at(state, head_row, head_col); // Get the character at the head position

  // 将当前snake's head 转换为身体字符
  char body_char = head_to_body(head_char);
  set_board_at(state, head_row, head_col, body_char); // Update the board with the new body character

  unsigned int next_row = get_next_row(head_row, head_char);
  unsigned int next_col = get_next_col(head_col, head_char);

  set_board_at(state, next_row, next_col, head_char); // Update the board with the new head position

  state->snakes[snum].head_row = next_row; // Update the snake struct with the new head position
  state->snakes[snum].head_col = next_col;

  return;
}

/*
  Task 4.4

  Helper function for update_state. Update the tail...

  ...on the board: blank out the current tail, and change the new
  tail from a body character (^<v>) into a tail character (wasd)

  ...in the snake struct: update the row and col of the tail
*/
static void update_tail(game_state_t *state, unsigned int snum)
{
  // TODO: Implement this function.
  unsigned int tail_col = state->snakes[snum].tail_col;
  unsigned int tail_row = state->snakes[snum].tail_row;

  char tail_char = get_board_at(state, tail_row, tail_col); // Get the character at the tail position

  unsigned int next_row = get_next_row(tail_row, tail_char);
  unsigned int next_col = get_next_col(tail_col, tail_char);

  set_board_at(state, tail_row, tail_col, ' ');                               // Blank out the current tail position
  char new_tail_char = body_to_tail(get_board_at(state, next_row, next_col)); // Convert the new tail character from body to tail
  set_board_at(state, next_row, next_col, new_tail_char);                     // Update the board with the new tail position

  state->snakes[snum].tail_row = next_row; // Update the snake struct with the new tail position
  state->snakes[snum].tail_col = next_col;

  return;
}

/* Task 4.5 */
void update_state(game_state_t *state, int (*add_food)(game_state_t *state))
{
  // TODO: Implement this function.
  for (unsigned int i = 0; i < state->num_snakes; i++)
  {
    // 如果蛇已经死亡，跳过
    if (!state->snakes[i].live)
    {
      continue;
    }

    // 获取蛇头的位置和下一格的字符
    unsigned int head_row = state->snakes[i].head_row;
    unsigned int head_col = state->snakes[i].head_col;
    char next_char = next_square(state, i);

    // 如果下一格是墙或者蛇身，蛇死亡
    if (next_char == '#' || is_snake(next_char))
    {
      set_board_at(state, head_row, head_col, 'x');
      state->snakes[i].live = false;
    }
    else if (next_char == '*')
    {                        // fruit eaten,grow snake, update head twice and add food
      update_head(state, i); // 头前进，尾巴不动，相当于长度+1
      add_food(state);
    }
    else
    {
      // 如果移动到空格，正常移动
      update_head(state, i);
      update_tail(state, i);
    }
  }

  return;
}

/* Task 5.1 */
char *read_line(FILE *fp)
{
  // TODO: Implement this function.
  if (fp == NULL)
  {
    return NULL;
  }

  size_t capacity = 100;
  char *buffer = malloc(capacity);
  if (buffer == NULL)
  {
    return NULL;
  }

  size_t len = 0;
  while (fgets(buffer + len, capacity - len, fp) != NULL)
  {
    len += strlen(buffer + len);

    // 检查是否读取到完整的一行
    if (len > 0 && buffer[len - 1] == '\n')
    {
      buffer[len - 1] = '\0'; // 去掉换行符
      return buffer;
    }

    // 如果缓冲区不足，扩展缓冲区
    capacity *= 2;
    char *new_buffer = realloc(buffer, capacity);
    if (new_buffer == NULL)
    {
      free(buffer);
      return NULL;
    }
    buffer = new_buffer;
  }

  // 如果文件结束但没有读取到任何内容
  if (len == 0)
  {
    free(buffer);
    return NULL;
  }

  return buffer;
}

/* Task 5.2 */
game_state_t *load_board(FILE *fp)
{
  if (fp == NULL)
  {
    return NULL;
  }

  // 初始化 game_state_t
  game_state_t *state = malloc(sizeof(game_state_t));
  if (state == NULL)
  {
    return NULL;
  }

  state->num_rows = 0;
  state->board = NULL;
  state->num_snakes = 0; // Task 5 要求：初始化为 0
  state->snakes = NULL;  // Task 5 要求：初始化为 NULL

  char *line = NULL;
  // 逐行读取文件
  while ((line = read_line(fp)) != NULL)
  {
    // 动态扩展 board 数组
    char **new_board = realloc(state->board, sizeof(char *) * (state->num_rows + 1));
    if (new_board == NULL)
    {
      free(line);
      free_state(state);
      return NULL;
    }

    state->board = new_board;
    state->board[state->num_rows] = line;
    state->num_rows++;
  }

  free(line); // 释放临时行缓冲区
  return state;
}

/*
  Task 6.1

  Helper function for initialize_snakes.
  Given a snake struct with the tail row and col filled in,
  trace through the board to find the head row and col, and
  fill in the head row and col in the struct.
*/
static void find_head(game_state_t *state, unsigned int snum)
{
  // TODO: Implement this function.
  unsigned int row = state->snakes[snum].tail_row;
  unsigned int col = state->snakes[snum].tail_col;

  while (true)
  {
    char current_char = get_board_at(state, row, col);

    if (is_head(current_char))
    {
      state->snakes[snum].head_row = row;
      state->snakes[snum].head_col = col;
      return;
    }

    // Move to the next
    row = get_next_row(row, current_char);
    col = get_next_col(col, current_char);
  }
}

/* Task 6.2 */
game_state_t *initialize_snakes(game_state_t *state)
{
  // TODO: Implement this function.
  for (unsigned int i = 0; i < state->num_rows; i++)
  {
    for (unsigned int j = 0; j < strlen(state->board[i]); j++)
    {
      // 如果是蛇的尾部
      if (is_tail(state->board[i][j]))
      {
        snake_t *new_snake = realloc(state->snakes, sizeof(snake_t) * (state->num_snakes + 1));
        if (new_snake == NULL)
        {
          free_state(state);
          return NULL;
        }
        state->snakes = new_snake;

        state->snakes[state->num_snakes].tail_row = i;
        state->snakes[state->num_snakes].tail_col = j;

        find_head(state, state->num_snakes);
        state->snakes[state->num_snakes].live = true;
        state->num_snakes++;
      }
    }
  }

  return state;
}
