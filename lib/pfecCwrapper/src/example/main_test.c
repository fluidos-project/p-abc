#include <stdio.h>
#include <Zp.h>
#include <g1.h>
#include <g2.h>
#include <g3.h>
#include <pair.h>


int main() {
    char bytes[]="RandomBytes";
    Zp *a = zpFromInt(5);
    Zp *b = zpFromInt(8);
    zpAdd(a,b);

    printf( "sum is ");
    zpPrint(a);
    printf("\n");
    zpFree(a);
    zpFree(b);
    a=zpRandom(rgInit(bytes,sizeof(bytes)/sizeof(bytes[0])));
    printf("Random Zp\n");
    zpPrint(a);
    printf("\n");
    zpFree(a);
    G1 *g=g1Generator();
    printf("G1 generator\n");
    g1Print(g);
    G2 *g2=g2Generator();
    printf("G1 generator\n");
    g2Print(g2);
    G3 *g3=pair(g,g2);    
    printf("e(g1,g2)\n");
    g3Print(g3);
    G1 * hashedG1=hashToG1(bytes,sizeof(bytes)/sizeof(bytes[0]));
    printf("Hashed G1\n");
    g1Print(hashedG1);
    G2 * hashedG2=hashToG2(bytes,sizeof(bytes)/sizeof(bytes[0]));
    printf("Hashed G2\n");
    g2Print(hashedG2);
    g1Free(g);
    g2Free(g2);
    g3Free(g3);
    return 0;
}
