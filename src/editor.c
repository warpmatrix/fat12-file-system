#include "editor.h"

static size_t max(size_t num1, size_t num2);
static size_t min(size_t num1, size_t num2);

void getCmdLine(char *input, size_t inputSize) {
    system("stty -icanon");
    system("stty -echo");
    strcpy(input, "");
    size_t index = 0, len = 0;
    int ch;
    while ((ch = getchar()) != '\n') {
        size_t newLen = len, newIdx = index;
        switch (ch) {
            case 27:
                ch = getchar();
                if (ch == 91) {
                    ch = getchar();
                    if (ch == 65 || ch == 66)
                        printf("\a");
                    else if (ch == 67)
                        newIdx = min(len, index + 1);
                    else if (ch == 68)
                        newIdx = max(0, index - 1);
                }
                break;
            case 127: {
                if (index == 0) {
                    printf("\a");
                    break;
                }
                char newInp[inputSize];
                newIdx--, newLen--;
                strncpy(newInp, input, index - 1);
                newInp[index - 1] = '\0';
                if (index < len)
                    strncat(newInp, input + index + 1, len - index - 1);
                strcpy(input, newInp);
                break;
            }
            default: {
                char chstr[2] = {ch, '\0'};
                char newInp[inputSize];
                newLen++, newIdx++;
                strncpy(newInp, input, index);
                newInp[index] = '\0';
                strcat(newInp, chstr);
                strncat(newInp, input + index, len - index);
                strcpy(input, newInp);
                break;
            }
        }
        for (size_t i = 0; i < index; i++) printf("\x1b[D");
        printf("\x1b[0K%s", input);
        len = newLen, index = newIdx;
        for (size_t i = 0; i < newLen - newIdx; i++) printf("\x1b[D");
    }
    putchar(ch);  // output '\n'
    system("stty echo");
    system("stty icanon");
}

static size_t max(size_t num1, size_t num2) {
    return (num1 > num2 ? num1 : num2);
}

static size_t min(size_t num1, size_t num2) {
    return (num1 < num2 ? num1 : num2);
}
