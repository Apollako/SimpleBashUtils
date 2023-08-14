#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "f_cat.h"

int main(int argc, char **argv) {
  if (argc > 1) {
    flags mode;
    make_it_zero(&mode);
    int wrong_flag = 0;
    parcer(argc, argv, &mode, &wrong_flag);
    FILE *f = NULL;
    for (int i = 1; i < argc; i++) {
      if (argv[i][0] != '-') {
        f = fopen(argv[i], "r");
        if (f == NULL) {
          exit(1);
        } else {
          if (wrong_flag == 1) {
            fclose(f);
          } else {
            int count_flags =
                mode.b + mode.E + mode.n + mode.s + mode.T + mode.v;
            if (count_flags == 0) {
              print_file(f);
            } else {
              FILE *initial = fopen("initial.txt", "w");
              if (initial == NULL) {
                fclose(f);
              } else {
                FILE *buffer = fopen("buffer.txt", "w");
                if (buffer == NULL) {
                  fclose(f);
                  fclose(initial);
                } else {
                  copy_file(f, initial);
                  fclose(buffer);
                  do_this_mode(&mode, initial, buffer);
                  initial = fopen("initial.txt", "r");
                  print_file(initial);
                  remove_buffers("initial.txt", "buffer.txt");
                }
              }
            }
          }
        }
      }
    }
  }
  return 0;
}

void remove_buffers(char *buff_1, char *buff_2) {
  remove(buff_1);
  remove(buff_2);
}

void print_file(FILE *f) {
  char ch;
  ch = getc(f);
  while (ch != EOF) {
    printf("%c", ch);
    ch = getc(f);
  }
  fclose(f);
}

void mode_b(FILE *initial, FILE *buffer) {
  initial = fopen("initial.txt", "r");
  buffer = fopen("buffer.txt", "w");
  char ch;
  int number = 1;
  ch = getc(initial);
  if (ch != '\n') {
    fprintf(buffer, "%6d\t%c", number, ch);
    ch = getc(initial);
    number++;
  }
  int count_endlines = 0;
  while (ch != EOF) {
    while (ch == '\n') {
      fprintf(buffer, "%c", ch);
      ch = getc(initial);
      count_endlines++;
    }
    if (count_endlines > 0 && ch != EOF) {
      fprintf(buffer, "%6d\t%c", number, ch);
      number++;
      count_endlines = 0;
    } else {
      fprintf(buffer, "%c", ch);
    }
    ch = getc(initial);
  }
  fclose(buffer);
  fclose(initial);
  from_file2_to_file1_and_reopen(initial, buffer);
}

void mode_T(FILE *initial, FILE *buffer) {
  initial = fopen("initial.txt", "r");
  buffer = fopen("buffer.txt", "w");
  char ch;
  ch = getc(initial);
  while (ch != EOF) {
    if (ch == '\t') {
      fprintf(buffer, "^I");
    } else {
      fprintf(buffer, "%c", ch);
    }
    ch = getc(initial);
  }
  fclose(buffer);
  fclose(initial);
  from_file2_to_file1_and_reopen(initial, buffer);
}

void mode_v(FILE *initial, FILE *buffer) {
  initial = fopen("initial.txt", "r");
  buffer = fopen("buffer.txt", "w");
  char ch;
  ch = getc(initial);
  while (ch != EOF) {
    if ((ch == '\n') || (ch == '\t')) {
      fprintf(buffer, "%c", ch);
    } else if ((ch >= 0) && (ch < 32)) {
      ch += 64;
      fprintf(buffer, "^%c", ch);
    } else if (ch == 127) {
      fprintf(buffer, "^?");
    } else {
      fprintf(buffer, "%c", ch);
    }
    ch = getc(initial);
  }
  fclose(buffer);
  fclose(initial);
  from_file2_to_file1_and_reopen(initial, buffer);
}

void mode_E(FILE *initial, FILE *buffer) {
  initial = fopen("initial.txt", "r");
  buffer = fopen("buffer.txt", "w");
  char ch;
  ch = getc(initial);
  while (ch != EOF) {
    if (ch != '\n') {
      fprintf(buffer, "%c", ch);
    }
    if (ch == '\n') {
      fprintf(buffer, "$\n");
    }
    ch = getc(initial);
  }
  fclose(buffer);
  fclose(initial);
  from_file2_to_file1_and_reopen(initial, buffer);
}

void mode_s(FILE *initial, FILE *buffer) {
  initial = fopen("initial.txt", "r");
  buffer = fopen("buffer.txt", "w");
  char ch;
  char ch_next;
  ch = getc(initial);
  while (ch != EOF) {
    if (ch != '\n') {
      fprintf(buffer, "%c", ch);
    }
    if (ch == '\n') {
      int count = 1;
      fprintf(buffer, "%c", ch);
      ch_next = getc(initial);
      while (ch_next == '\n') {
        count++;
        ch_next = getc(initial);
      }
      if (count >= 2) {
        fprintf(buffer, "\n%c", ch_next);
      } else {
        fprintf(buffer, "%c", ch_next);
      }
    }
    ch = getc(initial);
  }
  fclose(buffer);
  fclose(initial);
  from_file2_to_file1_and_reopen(initial, buffer);
}

void mode_n(FILE *initial, FILE *buffer) {
  initial = fopen("initial.txt", "r");
  buffer = fopen("buffer.txt", "w");
  char pred, ch;
  int number = 1;
  ch = getc(initial);
  if (ch != EOF) {
    fprintf(buffer, "%6d\t%c", number, ch);
  }
  pred = ch;
  ch = getc(initial);
  while (ch != EOF) {
    if (pred != '\n') {
      fprintf(buffer, "%c", ch);
    }
    if (pred == '\n') {
      number++;
      fprintf(buffer, "%6d\t%c", number, ch);
    }
    pred = ch;
    ch = getc(initial);
  }
  fclose(buffer);
  fclose(initial);
  from_file2_to_file1_and_reopen(initial, buffer);
}

void from_file2_to_file1_and_reopen(FILE *file1, FILE *file2) {
  file1 = fopen("initial.txt", "w");
  file2 = fopen("buffer.txt", "r");
  char ch;
  ch = getc(file2);
  while (ch != EOF) {
    fprintf(file1, "%c", ch);
    ch = getc(file2);
  }
  fclose(file2);
  fclose(file1);
}

void copy_file(FILE *f, FILE *buffer) {
  char ch;
  ch = getc(f);
  while (ch != EOF) {
    fprintf(buffer, "%c", ch);
    ch = getc(f);
  }
  fclose(f);
  fclose(buffer);
}

void parcer(int argc, char **argv, flags *mode, int *error) {
  int index_first_flag = 1;
  int index_last_flag = argc - 2;
  for (int i = index_first_flag; i <= index_last_flag; i++) {
    if ((strcmp(argv[i], "-b") == 0) ||
        (strcmp(argv[i], "--number-nonblank") == 0)) {
      mode->b = 1;
    } else if ((strcmp(argv[i], "-n") == 0) ||
               (strcmp(argv[i], "--number") == 0)) {
      mode->n = 1;
    } else if (strcmp(argv[i], "-e") == 0) {
      mode->E = 1;
      mode->v = 1;
    } else if (strcmp(argv[i], "-E") == 0) {
      mode->E = 1;
    } else if ((strcmp(argv[i], "-s") == 0) ||
               (strcmp(argv[i], "--squeeze-blank") == 0)) {
      mode->s = 1;
    } else if (strcmp(argv[i], "-v") == 0) {
      mode->v = 1;
    } else if (strcmp(argv[i], "-t") == 0) {
      mode->T = 1;
      mode->v = 1;
    } else if (strcmp(argv[i], "-T") == 0) {
      mode->T = 1;
    } else if (argv[i][0] == '-') {
      *error = 1;
    }
  }
  if (mode->b == 1) {
    mode->n = 0;
  }
}

void do_this_mode(flags *mode, FILE *initial, FILE *buffer) {
  if (mode->s == 1) {
    mode_s(initial, buffer);
  }
  if (mode->T == 1) {
    mode_T(initial, buffer);
  }
  if (mode->b == 1) {
    mode_b(initial, buffer);
  }
  if (mode->E == 1) {
    mode_E(initial, buffer);
  }
  if (mode->n == 1) {
    mode_n(initial, buffer);
  }
  if (mode->v == 1) {
    mode_v(initial, buffer);
  }
}

void make_it_zero(flags *mode) {
  mode->b = 0;
  mode->E = 0;
  mode->n = 0;
  mode->s = 0;
  mode->T = 0;
  mode->v = 0;
}
