/*
 * Copyright (C) 2024 nyaaaww
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

char fnamel[1024][1024];

long get_file_size(const char *filename) {
  FILE *file = fopen(filename, "rb");
  if (file == NULL) {
    perror("Error opening file for size check");
    return -1;
  }
  long size;
  fseek(file, 0, SEEK_END);
  size = ftell(file);
  rewind(file);
  fclose(file);
  return size;
}

int create_dir_recursive(const char *path) {
  char *cpy_path = strdup(path);
  if (!cpy_path) {
    perror("strdup failed");
    return -1;
  }

  char *cur_path = cpy_path;
  while (*cur_path) {
    if (*cur_path == '/') {
      *cur_path = '\0';  // temp cut dir to create
      if (mkdir(cpy_path, 0777) == -1 &&
          errno != EEXIST) {  // check dir is exists.
        free(cpy_path);
        return -1;
      }
      *cur_path = '/';
    }
    cur_path++;
  }

  free(cpy_path);
  return 0;
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
    fprintf(stderr, "A tool decode the wallpaper engine's mpkg file.\n");
    fprintf(stderr, "Example: %s test.mpkg\n", argv[0]);
    return EXIT_FAILURE;
  }

  FILE *file;
  int data;
  size_t result;
  memset(fnamel, 0x00, sizeof(fnamel));

  file = fopen(argv[1], "rb");
  if (file == NULL) {
    perror("Error opening file");
    return -1;
  }

  result = fread(&data, 4, 1, file);
  if (result != 1) {
    perror("Error reading file");
    fclose(file);
    return -1;
  }
  printf("Version Long: %d\n", data);
  char ver[data + 1];
  memset(ver, 0x00, sizeof(ver));
  fread(ver, 8, 1, file);
  printf("Version : %s\n", ver);

  int nof;
  fread(&nof, 4, 1, file);
  printf("Num of file : %d\n", nof);

  int *file_long = malloc(nof * sizeof(int));
  for (int i = 0; i < nof; i++) {
    int name_long;
    fread(&name_long, 4, 1, file);
    printf("\nName long : %d\n", name_long);

    // char fnamel[nof][name_long + 1];
    //   memset(fnamel, 0x00, sizeof(fnamel));
    fread(fnamel[i], name_long, 1, file);
    printf("Name : %s\n", fnamel[i]);

    // move file ptr down 4 bytes
    if (fseek(file, 4, SEEK_CUR) != 0) {
      perror("Error seeking in file");
      fclose(file);
      return -1;
    }

    fread(file_long + i, sizeof(int), 1, file);
    printf("File long : %d\n\n", file_long[i]);
    /*   for (int a = 0; a < nof; a++) {
         printf("File long : %d\n", file_long[a]);
       }*/

    if (i == nof - 1) {
      //   printf("DEBUG INFO: into write\n");
      int s;
      for (int l = 0; l < nof; l++) {
        //  printf("DEBUG PATH:%s\nDEBUG INFO: L is %d\n", fnamel[l], l);
        if (fnamel[l] == NULL) {
          continue;
        }
        char *buf;
        buf = (char *)malloc(file_long[l] + 1);
        fread(buf, file_long[l], 1, file);
        s = create_dir_recursive(fnamel[l]);
        if (s == -1) {
          perror("create dir failed");
          return -1;
        }
        FILE *wfile;
        wfile = fopen(fnamel[l], "wb");
        if (wfile == NULL) {
          perror("Error opening file");
          return EXIT_FAILURE;
        }

        size_t bytes_written;
        bytes_written = fwrite(buf, file_long[l], 1, wfile);
        if (bytes_written != 1) {
          perror("Error writing to file");
          fclose(wfile);
          return EXIT_FAILURE;
        }
        free(buf);
        fclose(wfile);
      }
    }
  }
  free(file_long);
  fclose(file);
  return 0;
}
