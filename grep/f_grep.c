#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "f_grep.h"

int main(int argc, char **argv) {
  if (argc > 2) {
    flags mode;
    make_it_zero(&mode);
    char *patterns;
    patterns = calloc(2048, sizeof(char));
    int success = 0;
    parser(argc, argv, patterns, &mode, &success);
    int i = optind;
    if (mode.e == 0 && mode.f == 0) {
      i++;
    }
    int num_of_files = argc - i;
    int *needed_files;
    needed_files = calloc(num_of_files, sizeof(int));
    int length = strlen(patterns);
    patterns[length - 1] = '\0';
    FILE *f = NULL;
    while (i <= argc - 1) {
      f = fopen(argv[i], "r");
      if (f != NULL) {
        int file_if_l = 0;
        if (mode.l == 1 || mode.c == 1 || (mode.v == 1 && mode.o == 1)) {
          reader_and_grep(patterns, f, &mode, num_of_files, argv[i], &file_if_l,
                          &success);
        } else {
          if (mode.o == 1) {
            grep_o(patterns, f, &mode, num_of_files, argv[i], &success);
          } else {
            reader_and_grep(patterns, f, &mode, num_of_files, argv[i],
                            &file_if_l, &success);
          }
        }
        fclose(f);
        if (file_if_l == 1) {
          printf("%s\n", argv[i]);
        }
      } else {
        if (mode.s == 0) {
          fprintf(stderr, "no such file or directory");
        }
      }
      i++;
    }
    free(patterns);
    free(needed_files);
  }
  return 0;
}

void parser(int argc, char **argv, char *patterns, flags *mode, int *success) {
  char opt;
  int no_flag_e = 1;
  int no_flag_f = 1;
  while ((opt = getopt_long(argc, argv, "e:ivclnshf:o", NULL, NULL)) != -1) {
    switch (opt) {
      case 'e':
        mode->e = 1;
        no_flag_e = 0;
        strcat(patterns, optarg);
        strcat(patterns, "|");
        break;
      case 'i':
        mode->i = 1;
        break;
      case 'v':
        mode->v = 1;
        break;
      case 'c':
        mode->c = 1;
        break;
      case 'l':
        mode->l = 1;
        break;
      case 'n':
        mode->n = 1;
        break;
      case 's':
        mode->s = 1;
        break;
      case 'h':
        mode->h = 1;
        break;
      case 'f':
        mode->f = 1;
        no_flag_f = 0;
        FILE *ff = NULL;
        ff = fopen(optarg, "r");
        if (ff == NULL) {
          free(patterns);
          exit(1);
        }
        char *fileline;
        fileline = calloc(2048, sizeof(char));
        while (feof(ff) == 0 && fgets(fileline, 2048, ff) != NULL) {
          if (strcmp(fileline, "\n") == 0) {
            *success = 1;
          }
          int length = 0;
          length = strlen(fileline);
          if (length > 1) {
            if (fileline[length - 1] == '\n') {
              fileline[length - 1] = '\0';
            }
          }
          if (length > 0) {
            strcat(patterns, fileline);
            strcat(patterns, "|");
          }
        }
        if (fileline != NULL) {
          free(fileline);
        }
        fclose(ff);
        break;
      case 'o':
        mode->o = 1;
        break;
      default:
        free(patterns);
        exit(1);
    }
  }
  if (no_flag_e == 1 && no_flag_f == 1) {
    strcat(patterns, argv[optind]);
    strcat(patterns, "|");
  }
}

void reader_and_grep(char *patterns, FILE *f, flags *mode, int num_of_files,
                     char *file_name, int *file_if_l, int *success) {
  int numerator = 0;
  char *fileline = calloc(2048, sizeof(char));
  char *temp_o;
  regex_t regular_patterns;
  regmatch_t pmatch[2];
  int reg_mode = REG_EXTENDED;
  int is_v = 0;
  int count_match_lines = 0;
  mode_for_regex(mode, &is_v, &reg_mode);
  regcomp(&regular_patterns, patterns, reg_mode);
  while (feof(f) == 0 && fgets(fileline, 2048, f) != NULL) {
    int length = strlen(fileline);
    if (fileline[length - 1] == '\n') {
      fileline[length - 1] = '\0';
    }
    numerator++;
    temp_o = fileline;
    if (mode->v == 1) {
      *success = 0;
    }
    if (regexec(&regular_patterns, temp_o, 1, pmatch, 0) == is_v || *success) {
      if ((num_of_files == 1) || (mode->h == 1)) {
        if (mode->l == 1) {
          *file_if_l = 1;
          count_match_lines++;
        } else if (mode->c == 0) {
          if (mode->n == 1) {
            printf("%d:%s\n", numerator, fileline);
          } else {
            printf("%s\n", fileline);
          }
        } else {
          count_match_lines++;
        }
      } else {
        if (mode->l == 1) {
          *file_if_l = 1;
          count_match_lines++;
        } else if (mode->c == 0) {
          if (mode->n == 1) {
            printf("%s:%d:%s\n", file_name, numerator, fileline);
          } else {
            printf("%s:%s\n", file_name, fileline);
          }
        } else {
          count_match_lines++;
        }
      }
    }
  }
  mode_lc(mode, num_of_files, file_name, file_if_l, count_match_lines);
  free(fileline);
  regfree(&regular_patterns);
}

void grep_o(char *patterns, FILE *f, flags *mode, int num_of_files,
            char *file_name, int *success) {
  int numerator = 0;
  char *fileline = calloc(2048, sizeof(char));
  char *temp_o;
  regex_t regular_patterns;
  regmatch_t pmatch[2];
  int reg_mode = REG_EXTENDED;
  int is_v = 0;
  mode_for_regex(mode, &is_v, &reg_mode);
  regcomp(&regular_patterns, patterns, reg_mode);
  while (feof(f) == 0 && fgets(fileline, 2048, f) != NULL) {
    int length = strlen(fileline);
    if (fileline[length - 1] == '\n') {
      fileline[length - 1] = '\0';
    }
    numerator++;
    temp_o = fileline;
    if (mode->v == 1) {
      *success = 0;
    }
    if (regexec(&regular_patterns, temp_o, 1, pmatch, 0) == is_v || *success) {
      if ((num_of_files == 1) || (mode->h == 1)) {
        if (mode->n == 1) {
          printf("%d:", numerator);
          mode_o(&regular_patterns, temp_o, pmatch, is_v, success);
        } else {
          mode_o(&regular_patterns, temp_o, pmatch, is_v, success);
        }
      } else {
        if (mode->n == 1) {
          printf("%s:%d:", file_name, numerator);
          mode_o(&regular_patterns, temp_o, pmatch, is_v, success);
        } else {
          printf("%s:", file_name);
          mode_o(&regular_patterns, temp_o, pmatch, is_v, success);
        }
      }
    }
  }
  free(fileline);
  regfree(&regular_patterns);
}

void mode_o(regex_t *regular_patterns, char *temp_o, regmatch_t *pmatch,
            int is_v, int *success) {
  while (regexec(regular_patterns, temp_o, 1, pmatch, 0) == is_v || *success) {
    for (int i = pmatch[0].rm_so; i < pmatch[0].rm_eo; i++) {
      printf("%c", temp_o[i]);
    }
    temp_o = temp_o + pmatch[0].rm_eo;
    printf("\n");
  }
}

void mode_for_regex(flags *mode, int *is_v, int *reg_mode) {
  if (mode->i == 1) {
    *reg_mode = *reg_mode | REG_ICASE;
  }
  if (mode->v == 1) {
    *is_v = REG_NOMATCH;
  }
  if (mode->f == 1) {
    *reg_mode = *reg_mode | REG_NEWLINE;
  }
  if (mode->o == 0) {
    *reg_mode = *reg_mode | REG_NOSUB;
  }
}

void mode_lc(flags *mode, int num_of_files, char *file_name, int *file_if_l,
             int count_match_lines) {
  if (mode->c == 1) {
    if (mode->l == 1) {
      if ((num_of_files == 1) || (mode->h == 1)) {
        if (count_match_lines > 0) {
          printf("1\n%s\n", file_name);
        } else {
          printf("0\n");
        }
      } else {
        if (count_match_lines > 0) {
          printf("%s:1\n%s\n", file_name, file_name);
        } else {
          printf("%s:0\n", file_name);
        }
      }
      *file_if_l = 0;
    } else {
      if ((num_of_files == 1) || (mode->h == 1)) {
        printf("%d\n", count_match_lines);
      } else {
        printf("%s:%d\n", file_name, count_match_lines);
      }
    }
  }
}

void make_it_zero(flags *mode) {
  mode->e = 0;
  mode->c = 0;
  mode->i = 0;
  mode->v = 0;
  mode->l = 0;
  mode->n = 0;
  mode->s = 0;
  mode->h = 0;
  mode->f = 0;
  mode->o = 0;
}
