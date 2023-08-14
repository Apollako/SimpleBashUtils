#ifndef F_CAT_H
#define F_CAT_H

typedef struct flag {
  int b;
  int E;
  int n;
  int s;
  int T;
  int v;
} flags;

void make_it_zero(flags *mode);
void parcer(int argc, char **argv, flags *mode, int *error);
void do_this_mode(flags *mode, FILE *f, FILE *buffer);
void copy_file(FILE *f, FILE *buffer);
void print_file(FILE *f);
void mode_E(FILE *initial, FILE *buffer);
void mode_s(FILE *initial, FILE *buffer);
void mode_n(FILE *initial, FILE *buffer);
void mode_b(FILE *initial, FILE *buffer);
void mode_v(FILE *initial, FILE *buffer);
void mode_T(FILE *initial, FILE *buffer);
void from_file2_to_file1_and_reopen(FILE *file1, FILE *file2);
void remove_buffers(char *buff_1, char *buff_2);

#endif // F_CAT_H
