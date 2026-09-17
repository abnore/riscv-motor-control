#include <stdio.h>
/* Two version, vector-magnitude and power analysis. 2/3 and sqrt(2/3), This
 * is a small program for the first one */

typedef struct {
    float a;
    float b;
} ab_vector;

#define SQRT3 1.73205080757f
#define CLARKE_CONST (1.0f/SQRT3)

ab_vector clarke_tran(float iA, float iB, float iC)
{
    ab_vector ab;

    // Equations simplified in a stable system that obeys Kirchhoff (No leaks)
    // i.e. iA + iB + iC = 0
    ab.a = iA;
    ab.b = (iB - iC) * CLARKE_CONST;

    return ab;
}

int main(void)
{
    ab_vector ab = clarke_tran(0, 1, -1);
    printf("a should be 0, a=%f, and b should be 1.54701, b=%f\n", ab.a, ab.b);
    return 0;
}
