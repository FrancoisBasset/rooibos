#ifndef PROMPT_H
#define PROMPT_H
extern int prompt_active;
extern char *command;

void prompt_init(void);
void prompt_show(void);
void prompt_hide(void);
void prompt_exec(void);
void prompt_write(char *typed);
void prompt_erase(void);
char* prompt_transform_key(char *key);
void prompt_move_left(void);
void prompt_move_right(void);
void prompt_up(void);
void prompt_down(void);
void prompt_on_key_press(XKeyEvent key_event, KeySym key_sym);
#endif
