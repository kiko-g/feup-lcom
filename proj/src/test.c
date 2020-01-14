#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>


int bcd_to_binary(int bcd) {
    return (int)(10*((bcd & 0xF0) >> 4) + (bcd & 0x0F));
}


int main()
{
    unsigned num = 99;
    
    int digit1 = num / 10;
    int digit2 = num % 10;
    printf("digits: %d %d\n", digit1, digit2);
}