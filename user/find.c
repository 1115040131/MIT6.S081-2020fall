// find.c
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void find(char *path, char *target) {
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if((fd = open(path, 0)) < 0){
        fprintf(2, "ls: cannot open %s\n", path);
        return;
    }

    if(fstat(fd, &st) < 0){
        fprintf(2, "ls: cannot stat %s\n", path);
        close(fd);
        return;
    }

    while (read(fd, &de, sizeof(de)) == sizeof(de)) { // 读取当前目录下所有文件和目录
        // 制作完整路径
        if (de.inum == 0) {
            continue;
        }
        strcpy(buf, path);
        p = buf + strlen(buf);
        *p++ = '/';
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;

        if(stat(buf, &st) < 0){
            printf("find: cannot stat %s\n", buf);
            continue;
        }

        switch(st.type) {
        case T_FILE:
            if (strcmp(target, de.name) == 0) {
                printf("%s\n", buf);
            }
            break;
        case T_DIR:
            if ((strcmp(de.name, ".") != 0) && (strcmp(de.name, "..") != 0)) {
                find(buf, target);
            }
        }
    }
    close(fd);
}

int main(int argc, char *argv[]){
    if (argc < 3) {
        fprintf(2, "usage: find <directory> <target file>\n");
        exit(1);
    }
    find(argv[1], argv[2]);
    exit(0);
}