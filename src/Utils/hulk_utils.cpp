#include "hulk_utils.hpp"
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <cstdio>

char* hulk_str_concat(const char* a, const char* b) {
    size_t len = strlen(a) + strlen(b) + 1;
    char* res = (char*)malloc(len);
    strcpy(res, a);
    strcat(res, b);
    return res;
}

char* hulk_str_concat_space(const char* a, const char* b) {
    if (!a) a = "";
    if (!b) b = "";
    size_t len = strlen(a) + strlen(b) + 2;
    char* res = (char*)malloc(len);
    if (!res) {
        fprintf(stderr, "Error: malloc falló en hulk_str_concat_space\n");
        exit(1);
    }
    strcpy(res, a);
    strcat(res, " ");
    strcat(res, b);
    return res;
}

bool hulk_str_equals(const char* a, const char* b) {
    if (!a || !b) return false;
    return strcmp(a, b) == 0;
}

double hulk_rand() {
    return rand() / (double)RAND_MAX;
}

double hulk_log_base_checked(double x, double base) {
    if (x <= 0 || base <= 1) {
        fprintf(stderr, "Math Error: log(x, base) requires x > 0 and base > 1\n");
        exit(1);
    }
    return log(x) / log(base);
}
