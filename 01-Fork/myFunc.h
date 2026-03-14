#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"

bool isPrime(int n) {
    if(n<=1) return false;
    if(n<=3) return true;
    for(int i=2;i*i<=n;i++)
        if(!(n%i))
            return false;
    return true;
}