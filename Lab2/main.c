#include <stdio.h>
#include <stdint.h>

#define uintMod 1000000000
#define size 35

struct uint1024_t {
    uint32_t arr[size];
    uint8_t undBehavior;
    size_t countNull;
};

uint32_t arr_to_uint(uint32_t *array) {
    uint32_t value = 0;
    uint32_t t = 1;
    for (int i = 8; i >= 0; i--) {
        value += array[i] * t;
        t *= 10;
    }
    return value;
}

struct uint1024_t from_uint(unsigned int x) {
    struct uint1024_t number;
    number.undBehavior = 0;

    for (int i = 2; i < size; i++) {
        number.arr[i] = 0;
    }
    number.countNull = 33;
    number.arr[0] = x % uintMod;
    number.arr[1] = x / uintMod;

    return number;
}

struct uint1024_t add_op(struct uint1024_t x, struct uint1024_t y) {
    struct uint1024_t result;
    uint8_t mod = 0;
    for (int i = 0; i < size; i++) {
        uint64_t sum = x.arr[i] + y.arr[i] + mod;
        result.arr[i] = sum % uintMod;
        mod = sum/uintMod;
    }
    if (mod) {
        result.undBehavior = 1;
    }
    return result;
}

struct uint1024_t subtr_op(struct uint1024_t x, struct uint1024_t y) {
    struct uint1024_t result;
    result.undBehavior = 0;
    uint32_t mod = 0;
    for (int i = 0; i < size; i++) {
        if (x.arr[i] >= y.arr[i] + mod) {
            result.arr[i] = x.arr[i] - (y.arr[i] + mod);
            mod = 0;
        }
        else {
            result.arr[i] = x.arr[i] + uintMod - (y.arr[i] + mod);
            mod = 1;
        }
    }
    return result;
}

struct uint1024_t mult_op(struct uint1024_t x, struct  uint1024_t y) {
    struct uint1024_t result = from_uint(0);
    uint32_t res1;
    uint64_t res, mod;
    short pos;
    for (int i = 0; i < (size - y.countNull); i++) {
        mod = 0;
        pos = i;
        for (int j = 0; j < size; j++) {
            res = x.arr[j] * y.arr[i] + mod;
            mod = res / uintMod;
            res1 = res % uintMod;
            result.arr[pos++] += res1;

            if (pos < size) {
                result.arr[pos] += mod;
            }
        }
        if (mod) {
            result.undBehavior = 1;
        }
    }
    return result;
}

void printf_value(struct uint1024_t x) {
    if (!x.undBehavior) {
        int pos = size - 1;
        while (x.arr[pos] == 0) {
            pos--;
        }
        for (int i = pos; i >= 0; i--) {
            printf("%0.9u", x.arr[i]);
        }
    }
    else {
        printf("UndefinedBehavior");
    }
    printf("\n");
}

void scanf_value(struct uint1024_t* x) {

    for (int i = 0; i < size; i++) {
        x->arr[i] = 0;
    }
    char str[315];
    for (int i = 0; i < 315; i++) {
        str[i] = 0;
    }

    int sym = 0;
    int pos = 0;
    int count = 0;
    while ((sym = getchar()) != '\n') {
        if (pos > 315) {
            x->undBehavior = 1;
            return;
        }
        str[pos++] = sym;
        count++;
    }

    uint32_t array[9];
    for (int j = 0; j < 9; j++) {
        array[j] = 0;
    }

    int chunk = 8;
    int uintSize = 0;

    for (int i = (count - 1); i >= 0; i--) {
        array[chunk--] = str[i] - '0';

        if (chunk == -1) {
            chunk = 8;
            x->arr[uintSize++] = arr_to_uint(array);
            for (int j = 0; j < 9; j++) {
                array[j] = 0;
            }
        }
    }
    if (chunk != 8) {
        x->arr[uintSize] = arr_to_uint(array);
    }

    size_t tail = size - 1;
    x->countNull = 0;
    while (x->arr[tail--] == 0) {
        x->countNull++;
    }
}

int main() {
    struct uint1024_t numInp1, numInp2, numOp;
    struct uint1024_t *numInp = &numInp1;
    uint32_t n;

    // first number
    printf("Input your uint1024 number: \n");
    scanf_value(numInp);
    if (numInp1.undBehavior) {
        printf("UndefinedBehavior\n");
        return 0;
    }

    // second number
    printf("Input your int number: \n");
    scanf("%u", &n);
    numInp2 = from_uint(n);

    //operations
    numOp = add_op(numInp1, numInp2);
    printf("Sum of numbers: \n");
    printf_value(numOp);

    numOp = subtr_op(numInp1, numInp2);
    printf("Difference between numbers: \n");
    printf_value(numOp);

    printf("Product of numbers: \n");
    numOp = mult_op(numInp1, numInp2);
    printf_value(numOp);

    return 0;
}
