#include <stdio.h>

#include "lib/triple_buffer/TripleBuffer.h"

int main() {
    LocalTripleBuffer<int> buff{};

    int* w;
    int* r;

    w = buff.write();
    *w = 5;
    w = buff.write();

    r = buff.read();
    if(*r != 5) {
        printf("failed unit test :(\n");
    }
}
