#include <stdio.h>
#include <stdlib.h>

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

int print_head(const char *file_name){
    FILE *file;
    int data;
    size_t result;

    // 打开文件
    file = fopen(file_name, "rb"); 
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
    char ver[data+1];
    memset(ver,0x00,sizeof(ver));
    fread(ver, 8, 1, file);
    printf("Version : %s\n", ver);

    int nof;
    fread(&nof, 4, 1, file);
    printf("Num of file : %d\n", nof);

    for(int i=0;i<nof;i++){

    int name_long;
    fread(&name_long, 4, 1, file);
    printf("\nName long : %d\n", name_long);


    char fnamel[nof][name_long+1];
    memset(fnamel,0x00,sizeof(fnamel));
    fread(fnamel[i], name_long, 1, file);
    printf("Name : %s\n", fnamel[i]);

    // 将文件指针下移4个字节
    if (fseek(file, 4, SEEK_CUR) != 0) {
        perror("Error seeking in file");
        fclose(file);
        return -1;
    }

    int file_long;
    fread(&file_long, 4, 1, file);
    printf("File long : %d\n", file_long);
    }
    fclose(file);
    return 0;
}

int main(int argc,char *argv[]){

    FILE *file;
    int data;
    char *buf;
    long file_size=get_file_size(argv[1]);
    size_t result;

    buf=(char *)malloc(file_size+1);
    if (buf == NULL) {
        perror("Error allocating memory");
        fclose(file);
        return -1;
    }

print_head(argv[1]);

    file = fopen(argv[1], "rb");
    if (file == NULL) {
        perror("Error opening file");
        return -1;
    }

    result = fread(buf, 1, file_size, file);
    if (result != file_size) {
        perror("Error reading file");
        free(buf);
        fclose(file);
        return -1;
    }
    // 关闭文件
    fclose(file);

    return 0;
}
