#include <stdio.h>

int main(int argc, char const *argv[]) {
    FILE *fp = fopen("../disk/origin/disk1.flp", "rb");
    if (fp == NULL) {
        printf("The fp don't exist\n");
        return 0;
    }
    FILE *img = fopen("fat.img", "wb");
    for (size_t i = 0; i < 19 * 512; i++) {
        char ch;
        fscanf(fp, "%c", &ch);
        fprintf(img, "%c", ch);
    }
    fclose(fp);
    fclose(img);
    return 0;
}
