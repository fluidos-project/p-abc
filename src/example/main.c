#include <stdio.h>
#include <Zp.h>
#include <Dpabc.h>



int main() {
    printf("Starting example\n");
    int nattr=3;
	int nkeys=2;
    char * seed="SeedForMainExampleBinary";
	char * msg="signedMessage";
	int msgLength=13;
	int seedLength=40;
	Zp *attributes[nattr];
	ranGen * rng=rgInit(seed,seedLength);
	publicKey *pks[nkeys];
	publicKey *aggrKey;
	secretKey *sks[nkeys];
	signature *partialSigns[nkeys];
	signature *combinedSignature;
	zkToken* token;
	int nIndexReveal=2;
    int indexReveal[]={0,2};
	Zp *revealedAttributes[nIndexReveal];
	Zp *epoch=zpFromInt(12034);

    changeNattr(nattr);
	seedRng(seed,seedLength);
	for(int i=0;i<nattr;i++)
		attributes[i]=zpRandom(rng);
	for(int i=0;i<nkeys;i++)
		keyGen(&(sks[i]),&(pks[i]));
	aggrKey=keyAggr((const publicKey **)pks,nkeys);
	for(int i=0;i<nkeys;i++)
		partialSigns[i]=sign(sks[i],epoch,(const Zp **)attributes);
	combinedSignature=combine((const publicKey **)pks,(const signature **)partialSigns,nkeys);
	printf("Verification result 0: %d\n",verify(pks[0],partialSigns[0],epoch,(const Zp **)attributes));
	printf("Verification result 1: %d\n",verify(pks[1],partialSigns[1],epoch,(const Zp **)attributes));
	printf("Verification result: %d\n",verify(aggrKey,combinedSignature,epoch,(const Zp **)attributes));
	token=presentZkToken(aggrKey,combinedSignature,epoch,(const Zp **)attributes,indexReveal,nIndexReveal,msg,msgLength);
	revealedAttributes[0]=zpCopy(attributes[0]);
	revealedAttributes[1]=zpCopy(attributes[2]);
	printf("Zk verification result: %d\n",verifyZkToken(token,aggrKey,epoch,(const Zp **)revealedAttributes,indexReveal,nIndexReveal,msg,msgLength));
	for(int i=0;i<nattr;i++)
		zpFree(attributes[i]);
	for(int i=0;i<nkeys;i++){
		dpabcPkFree(pks[i]);
		dpabcSkFree(sks[i]);
		dpabcSignFree(partialSigns[i]);
	}
	dpabcSignFree(combinedSignature);
	dpabcPkFree(aggrKey);
	for(int i=0;i<nIndexReveal;i++)
		zpFree(revealedAttributes[i]);
	dpabcZkFree(token);
    zpFree(epoch);
	rgFree(rng);
	dpabcFreeStateData();
    return 0;
}
