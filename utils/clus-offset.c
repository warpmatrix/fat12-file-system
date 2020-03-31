#include <stdio.h>

int main(int argc, char const *argv[]) {
    unsigned short clus;
    while (scanf("%hx", &clus) != EOF) {
        unsigned long long int offset = (31 + clus) * 512;
        printf("0x%llx\n", offset);
    }
    return 0;
}
