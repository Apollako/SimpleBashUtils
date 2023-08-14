#ifndef F_GREP_H
#define F_GREP_H

typedef struct flag {
  int e;
  int i;
  int v;
  int c;
  int l;
  int n;
  int s;
  int h;
  int f;
  int o;
} flags;

void make_it_zero(flags *mode);
void parser(int argc, char **argv, char *patterns, flags *mode, int *success);
void reader_and_grep(char *patterns, FILE *f, flags *mode, int num_of_files,
                     char *file_name, int *file_if_l, int *success);
void mode_lc(flags *mode, int num_of_files, char *file_name, int *file_if_l,
             int count_match_lines);
void mode_for_regex(flags *mode, int *is_v, int *reg_mode);
void mode_o(regex_t *regular_patterns, char *temp_o, regmatch_t *pmatch,
            int is_v, int *success);
void grep_o(char *patterns, FILE *f, flags *mode, int num_of_files,
            char *file_name, int *success);

#endif // F_GREP_H
