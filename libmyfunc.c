// libmyfunc.c - No external dependencies
#include <string.h>

int process_data(void* input, size_t input_size, 
                 void* output, size_t* output_size) {
    const char* in_str = (const char*)input;
    char* out_str = (char*)output;

    size_t i;
    
    // Manual uppercase conversion
    for (i = 0; i < input_size && in_str[i]; i++) {
        char c = in_str[i];
        out_str[i] = (c >= 'a' && c <= 'z') ? c - 32 : c;
    }
    out_str[i] = '\0';
    *output_size = i + 1;
    return 0;
}