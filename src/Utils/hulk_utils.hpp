#ifndef HULK_UTILS_HPP
#define HULK_UTILS_HPP

#ifdef __cplusplus
extern "C" {
#endif

char* hulk_str_concat(const char* a, const char* b);
char* hulk_str_concat_space(const char* a, const char* b);
bool hulk_str_equals(const char* a, const char* b);
double hulk_rand();
double hulk_log_base_checked(double x, double base);

#ifdef __cplusplus
}
#endif

#endif
