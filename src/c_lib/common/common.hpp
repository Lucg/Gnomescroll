#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <time.h>

unsigned int sanitize_player_name(char* name);

bool in_array_int(int* arr, int arr_max, int val)
{
    for (int i=0; i<arr_max; i++)
        if (arr[i] == val) return true;
    return false;
}

int count_digits(int n)
{
    int digits = 0;
    do {
        digits++;
        n /= 10;
    } while (n > 0);
    return digits;
}

void address_from_string(char* ip, int address[4]);
void address_from_uint32(uint32_t ip, uint8_t address[4]);

char* get_time_str();
time_t atott(const char* str);

time_t utc_now();

char* strip_whitespace(const char* str)
{
    int i=0;
    while (isspace(str[i])) i++;
    int start = i;
    i = strlen(str)-1;
    while (isspace(str[i])) i--;
    int end = i;
    int len = end-start+1;
    char* nstr = (char*)malloc((len+1) * sizeof(char));
    strncpy(nstr, &str[start], len);
    nstr[len] = '\0';
    return nstr;
}

unsigned char* hexstr_to_char(const char* hexstr)
{
    unsigned int len = (unsigned int)strlen(hexstr);
    GS_ASSERT(len%2 == 0);
    unsigned int final_len = len / 2;
    unsigned char* chrs = (unsigned char*)malloc((final_len+1) * sizeof(unsigned char));
    unsigned int i=0,j=0;
    for (; i<len; i+=2, j++)
        chrs[j] = (hexstr[i] % 32 + 9) % 25 * 16 + (hexstr[i+1] % 32 + 9) % 25;
    chrs[final_len] = '\0';
    return chrs;
}

static unsigned int strhash(const char* s)
{
    unsigned int highorder;
    unsigned int h = 0;
    int i;
    for (i=0; s[i] != '\0'; i++)
    {
         highorder = h & 0xf8000000;    // extract high-order 5 bits from h
                                        // 0xf8000000 is the hexadecimal representation
                                        //   for the 32-bit number with the first five 
                                        //   bits = 1 and the other bits = 0   
         h = h << 5;                    // shift h left by 5 bits
         h = h ^ (highorder >> 27);     // move the highorder 5 bits to the low-order
                                        //   end and XOR into h
         h = h ^ (unsigned int)s[i];                  // XOR h and ki
    }
    return h;
}

inline bool is_strict_char(const char c) __attribute__((always_inline));
inline bool is_strict_char(const char c)
{
    return (c >= 33 && c <= 126);
}

bool str_starts_with(const char* str, const char* match)
{
    if (match[0] == '\0') return true;
    int i=0;
    char c = '\0';
    char d = '\0';
    while ((c = str[i]) != '\0' && (d = match[i]) != '\0')
    {
        if (c != d) return false;
        i++;
    }
    return (match[i] == '\0');
}
