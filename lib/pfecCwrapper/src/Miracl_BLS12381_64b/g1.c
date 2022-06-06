#include <g1.h>
#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#define CEIL(a,b) (((a)-1)/(b)+1)

// Methods for hashing from AMCL, following https://datatracker.ietf.org/doc/draft-irtf-cfrg-hash-to-curve/, until they are fully integrated/standardized
/*
1. u = hash_to_field(msg, 2)
2. Q0 = map_to_curve(u[0])
3. Q1 = map_to_curve(u[1])
4. R = Q0 + Q1              # Point addition
5. P = clear_cofactor(R)
6. return P
*/


/* https://datatracker.ietf.org/doc/draft-irtf-cfrg-hash-to-curve/ */
static void hash_to_field_BLS12381(int hash,int hlen,FP_BLS12381 *u,octet *DST,octet *M, int ctr)
{
    int i,j,L,k,m;
    BIG_384_58 q,w,r;
    DBIG_384_58 dx;
    char okm[512],fd[256];
    octet OKM = {0,sizeof(okm),okm};
    BIG_384_58_rcopy(q, Modulus_BLS12381);
    k=BIG_384_58_nbits(q);
    BIG_384_58_rcopy(r, CURVE_Order_BLS12381);
    m=BIG_384_58_nbits(r);
    L=CEIL(k+CEIL(m,2),8);
    XMD_Expand(hash,hlen,&OKM,L*ctr,DST,M);
    for (i=0;i<ctr;i++)
    {
        for (j=0;j<L;j++)
            fd[j]=OKM.val[i*L+j];
        BIG_384_58_dfromBytesLen(dx,fd,L);
        BIG_384_58_dmod(w,dx,q);
        FP_BLS12381_nres(&u[i],w);
    }
}

static int htp_BLS12381(const char *mess, int n, ECP_BLS12381 *P)
{
    int res=0;
    BIG_384_58 r;
    FP_BLS12381 u[2];
    ECP_BLS12381 P1;
    char dst[100];
    char msg[2000];
    octet MSG = {0,sizeof(msg),msg};
    octet DST = {0,sizeof(dst),dst};
    BIG_384_58_rcopy(r, CURVE_Order_BLS12381);
    OCT_jbytes(&MSG,mess,n);
    OCT_jstring(&DST,(char *)"QUUX-V01-CS02-with-BLS12381G1_XMD:SHA-256_SSWU_RO_");
    hash_to_field_BLS12381(MC_SHA2,HASH_TYPE_BLS12381,u,&DST,&MSG,2);
    ECP_BLS12381_map2point(P,&u[0]);
    ECP_BLS12381_map2point(&P1,&u[1]);
    ECP_BLS12381_add(P,&P1);
    ECP_BLS12381_cfp(P);
    ECP_BLS12381_affine(P);
    return res;
}

//Header methods

G1* g1Generator(){
    G1 *r=malloc(sizeof(G1));
    r->p=malloc(sizeof(ECP_BLS12381));
    ECP_BLS12381_generator(r->p);
    return r;
}

G1* g1Identity(){
    G1 *r=malloc(sizeof(G1));
    r->p=malloc(sizeof(ECP_BLS12381));
    ECP_BLS12381_inf(r->p);
    return r;
}

int g1ByteSize(){
    return 2*MODBYTES_384_58+1; //0x04|x|y TODO, Check compressed serializations for possible usage
}

void g1ToBytes(char *res, const G1 *a){
    octet o={0,2*MODBYTES_384_58+1,res};
    ECP_BLS12381_toOctet(&o,a->p,false);
}

G1* hashToG1(const char *bytes, int n){
    G1 *r=malloc(sizeof(G1));
    r->p=malloc(sizeof(ECP_BLS12381));
    htp_BLS12381(bytes,n,r->p);
    return r;
} 

G1 * g1FromBytes(const char *bytes){
    G1 *r=malloc(sizeof(G1));
    r->p=malloc(sizeof(ECP_BLS12381));
    octet o={0,2*MODBYTES_384_58+1,bytes};
    ECP_BLS12381_fromOctet(r->p,&o);
    return r;
}

void g1Add(G1* a, const G1* b){
    ECP_BLS12381_add(a->p,b->p); 
}

void g1Sub(G1* a, const G1* b){
    ECP_BLS12381_sub(a->p,b->p); 
}

void g1Mul(G1* a, const Zp* b){
    ECP_BLS12381_mul(a->p,b->z); 
}

void g1InvMul(G1* a, const Zp* b){
    Zp aux;
    BIG_384_58_rcopy(aux.z,b->z);
    zpNeg(&aux);
    ECP_BLS12381_mul(a->p,aux.z); 
}

int g1IsIdentity(const G1* a){
    return ECP_BLS12381_isinf(a->p);
}

int g1Equals(const G1* a, const G1* b){
    return ECP_BLS12381_equals(a->p,b->p);
}


G1* g1Copy(const G1* a){
    G1 * res=malloc(sizeof(G1));
    res->p=malloc(sizeof(ECP_BLS12381));
    ECP_BLS12381_copy(res->p,a->p);
    return res;
}

void g1CopyValue(G1* a, const G1* b){
    ECP_BLS12381_copy(a->p,b->p);
}

void g1Print(const G1* e){
    ECP_BLS12381_outputxyz(e->p);
}

void g1Free(G1* e){
    free(e->p);
    free(e);
}

