#include <stdio.h>

#define nyi(f)	fprintf(stderr, "Not yet implemented: " #f "\n");

void scan_int(int* ip) { scanf("%i", ip); }
void print_int(int i) { printf("%i", i); }
void scan_float(float* fp) { scanf("%f", fp); }
void print_float(float f) { printf("%f", f); }
void scan_boolean(int* bp) { scanf("%i", bp); }
void print_boolean(int b) { printf("%s", b ? "true" : "false"); }
void print_endofline(void) { putchar('\n'); }
void print_blank(void) { putchar(' '); }
void print_string(const char* s) { printf("%s", s); }
void* alfa_malloc(size_t s) { nyi(alfa_malloc); return 0; }
void alfa_free(void* p) { nyi(alfa_free); }
void ld_float(void) { nyi(ld_float); }