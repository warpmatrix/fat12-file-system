#include <stdio.h>

int main(int argc, char const *argv[]) {
    FILE *fp = fopen("disk2.flp", "rb");
    if (fp == NULL) {
        printf("The fp don't exist\n");
        return 0;
    }
    FILE *img = fopen("mbr.img", "wb");
    if (img == NULL) {
        printf("The img don't exist\n");
        return 0;
    }
    for (size_t i = 0; i < 512; i++) {
        char ch;
        fscanf(fp, "%c", &ch);
        fprintf(img, "%c", ch);
    }
    fclose(fp);
    fclose(img);
    return 0;
}
