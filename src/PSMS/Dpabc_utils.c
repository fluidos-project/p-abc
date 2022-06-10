#include "Dpabc_utils.h"
#include "types_impl.h"
#include <stdlib.h>
#include <string.h>


void hash0(const Zp *m[], int mSize, Zp ** z, G2 ** g){
    int TAG_length=19;
    int zpBytes=zpByteSize();
    int nBytes=mSize*zpBytes;
    char bytes[nBytes+TAG_length];
    for(int j=0;j<mSize;j++){
        zpToBytes(bytes+TAG_length+(zpBytes*j),m[j]);
    }
    memcpy(bytes,"PABC-PSMS-V01-ENCZP",TAG_length); // Domain separation See https://datatracker.ietf.org/doc/draft-irtf-cfrg-hash-to-curve/
    *z=hashToZp(bytes,nBytes+TAG_length);
    memcpy(bytes,"PABC-PSMS-V01-ENCEC",TAG_length);
    *g=hashToG2(bytes,nBytes+TAG_length);
}

 //TODO Add domain separation in rest of methods See https://datatracker.ietf.org/doc/draft-irtf-cfrg-hash-to-curve/

Zp *hashPk(const publicKey * pk){
    int g1Bytes=g1ByteSize();
    int nBytes=g1Bytes*(pk->n+3);
    char bytes[nBytes];
    int k=0;
    g1ToBytes(bytes+(g1Bytes*k++),pk->vx);
    g1ToBytes(bytes+(g1Bytes*k++),pk->vy_m);
    g1ToBytes(bytes+(g1Bytes*k++),pk->vy_epoch);
    for(int i=0;i<pk->n;i++)
        g1ToBytes(bytes+(g1Bytes*k++),pk->vy[i]);
    return hashToZp(bytes,nBytes);
}


void hash1(const publicKey *pks[], int nkeys,Zp *t[]){
    for(int i=0;i<nkeys;i++)
        t[i]=hashPk(pks[i]);
}


void hash2(const char * m, int mLength, const publicKey * pk, const G2 * sigma1, const G2 *sigma2, const G3 * g3El, Zp ** result){
    int g1Bytes=g1ByteSize();
    int g2Bytes=g2ByteSize();
    int g3Bytes=g3ByteSize();
    int nBytes=mLength+g1Bytes*(pk->n+3)+g2Bytes*2+g3Bytes;
    char bytes[nBytes];
    char * aux;
    aux=bytes;
    g1ToBytes(aux,pk->vx);
    aux=aux+g1Bytes;
    g1ToBytes(aux,pk->vy_m);
    aux=aux+g1Bytes;
    g1ToBytes(aux,pk->vy_epoch);
    aux=aux+g1Bytes;
    for(int i=0;i<pk->n;i++){
        g1ToBytes(aux,pk->vy[i]);
        aux=aux+g1Bytes;
    }
    g2ToBytes(aux,sigma1);
    aux=aux+g2Bytes;
    g2ToBytes(aux,sigma2);
    aux=aux+g2Bytes;
    g3ToBytes(aux,g3El);
    aux=aux+g3Bytes;
    memcpy(aux,m,mLength);
    *result=hashToZp(bytes,nBytes);
}


        

