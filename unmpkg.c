#include <stdio.h>

long get_file_size(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Error opening file for size check");
        return -1; // 返回-1表示打开文件失败
    }

    long size;
    fseek(file, 0, SEEK_END); // 移动文件指针到文件末尾
    size = ftell(file);       // 获取当前位置，即文件大小
    rewind(file);             // 将文件指针重置到文件开始

    fclose(file);    
    return size;
}

int main(int argc,char *argv[]){

}
