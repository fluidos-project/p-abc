#include <stdarg.h>
#include <setjmp.h>
#include <stddef.h>
#include <cmocka.h>
#include <Zp.h>
#include <Dpabc.h>



static void test_hidden_indexes_computation(void **state)
{
    int n=7;
    int nIndexReveal=3;
    int indexReveal[]={0,1,4};
    int nhidden=n-nIndexReveal;
    int hidden[nhidden];
    int k,i,h;
    k=i=h=0;
    while(i<n && k<nIndexReveal){
        while(i<indexReveal[k]){
            hidden[h++]=i++;
        }
        i++;
        k++;
    }
    while (i<n){
        hidden[h++]=i++;
    }
    //for(int j=0;j<nhidden;j++)
    //   print_message("%d ", hidden[j]);
    assert_int_equal(hidden[0],2);
    assert_int_equal(hidden[1],3);
    assert_int_equal(hidden[2],5);
    assert_int_equal(hidden[3],6);
}

static void test_simple_complete_flow(void **state)
{
	int nattr=3;
	int nkeys=2;
    char * seed="SeedForTheTest_test_simple_complete_flow";
	char * msg="signedMessage_flow";
	int msgLength=18;
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
		keyGen(&sks[i],&pks[i]);
	aggrKey=keyAggr((const publicKey **)pks,nkeys);
	for(int i=0;i<nkeys;i++)
		partialSigns[i]=sign(sks[i],epoch,(const Zp **)attributes);
	combinedSignature=combine((const publicKey **)pks,(const signature **)partialSigns,nkeys);
	assert_true(verify(aggrKey,combinedSignature,epoch,(const Zp **)attributes));
	token=presentZkToken(aggrKey,combinedSignature,epoch,(const Zp **)attributes,indexReveal,nIndexReveal,msg,msgLength);
	revealedAttributes[0]=zpCopy(attributes[0]);
	revealedAttributes[1]=zpCopy(attributes[2]);
	assert_true(verifyZkToken(token,aggrKey,epoch,(const Zp **)revealedAttributes,indexReveal,nIndexReveal,msg,msgLength));
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
	rgFree(rng);
	dpabcFreeStateData();
}

static void test_fraudulent_modifications_flow(void **state)
{
	int nattr=3;
	int nkeys=2;
    char * seed="SeedForTheTest_test_fraudulent_modifications";
	char * msg="signedMessage_fraud";
	int msgLength=19;
	int seedLength=44;
	Zp *attributes[nattr];
	Zp *modifiedAttr[nattr];
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
	Zp *modifiedRevealedAttributes[nIndexReveal];
	Zp *lessRevealedAttributes[nIndexReveal-1];
	Zp *epoch=zpFromInt(12034);
	Zp *modifiedEpoch=zpFromInt(15034);

	changeNattr(nattr);
	seedRng(seed,seedLength);
	for(int i=0;i<nattr;i++)
		attributes[i]=zpRandom(rng);
	for(int i=0;i<nattr;i++)
		modifiedAttr[i]=zpCopy(attributes[i]);
	zpAdd(attributes[0],epoch);
	for(int i=0;i<nkeys;i++)
		keyGen(&sks[i],&pks[i]);
	aggrKey=keyAggr((const publicKey **)pks,nkeys);
	for(int i=0;i<nkeys;i++)
		partialSigns[i]=sign(sks[i],epoch,(const Zp **)attributes);
	combinedSignature=combine((const publicKey **)pks,(const signature **)partialSigns,nkeys);
	assert_true(verify(aggrKey,combinedSignature,epoch,(const Zp **)attributes));
	assert_false(verify(aggrKey,combinedSignature,modifiedEpoch,(const Zp **)attributes));
	assert_false(verify(aggrKey,combinedSignature,epoch,(const Zp **)modifiedAttr));
	token=presentZkToken(aggrKey,combinedSignature,epoch,(const Zp **)attributes,indexReveal,nIndexReveal,msg,msgLength);
	revealedAttributes[0]=zpCopy(attributes[0]);
	revealedAttributes[1]=zpCopy(attributes[2]);
	modifiedRevealedAttributes[0]=zpCopy(attributes[0]);
	modifiedRevealedAttributes[1]=zpCopy(attributes[2]);
	lessRevealedAttributes[0]=zpCopy(attributes[0]);
	zpAdd(modifiedRevealedAttributes[1],modifiedRevealedAttributes[0]);
	assert_true(verifyZkToken(token,aggrKey,epoch,(const Zp **)revealedAttributes,indexReveal,nIndexReveal,msg,msgLength));
	assert_false(verifyZkToken(token,aggrKey,epoch,(const Zp **)modifiedRevealedAttributes,indexReveal,nIndexReveal,msg,msgLength));
	assert_false(verifyZkToken(token,aggrKey,epoch,(const Zp **)lessRevealedAttributes,indexReveal,nIndexReveal-1,msg,msgLength));
	assert_false(verifyZkToken(token,aggrKey,modifiedEpoch,(const Zp **)revealedAttributes,indexReveal,nIndexReveal,msg,msgLength));
	for(int i=0;i<nattr;i++)
		zpFree(attributes[i]);
	for(int i=0;i<nkeys;i++){
		dpabcPkFree(pks[i]);
		dpabcSkFree(sks[i]);
		dpabcSignFree(partialSigns[i]);
	}
	dpabcSignFree(combinedSignature);
	dpabcPkFree(aggrKey);
	for(int i=0;i<nIndexReveal;i++){
		zpFree(revealedAttributes[i]);
		zpFree(modifiedRevealedAttributes[i]);
	}	
	zpFree(lessRevealedAttributes[0]);
	dpabcZkFree(token);
	rgFree(rng);
	dpabcFreeStateData();
}

static void test_flow_with_serialization(void **state)
{
	int nattr=15;
	int nkeys=2;
    char * seed="SeedForTheTest_test_flow_with_serialization";
	char * msg="signedMessage_serial";
	int msgLength=20;
	int seedLength=44;
	Zp *attributes[nattr];
	ranGen * rng=rgInit(seed,seedLength);
	publicKey *pks[nkeys];
	publicKey *aggrKey;
	publicKey *aggrKeyRegenerated;
	secretKey *sks[nkeys];
	signature *partialSigns[nkeys];
	signature *combinedSignature;
	signature *combinedSignatureRegenerated;
	zkToken* token;
	zkToken* tokenRegenerated;
	int nIndexReveal=2;
    int indexReveal[]={0,2};
	Zp *revealedAttributes[nIndexReveal];
	Zp *epoch=zpFromInt(12034);

	changeNattr(nattr);
	seedRng(seed,seedLength);
	for(int i=0;i<nattr;i++)
		attributes[i]=zpRandom(rng);
	for(int i=0;i<nkeys;i++)
		keyGen(&sks[i],&pks[i]);
	aggrKey=keyAggr((const publicKey **)pks,nkeys);
	char aggrKeySerial[dpabcPkByteSize(aggrKey)];
	dpabcPkToBytes(aggrKeySerial,aggrKey);
	aggrKeyRegenerated=dpabcPkFromBytes(aggrKeySerial);
	assert_true(dpabcPkEquals(aggrKey,aggrKeyRegenerated));
	for(int i=0;i<nkeys;i++)
		partialSigns[i]=sign(sks[i],epoch,(const Zp **)attributes);
	combinedSignature=combine((const publicKey **)pks,(const signature **)partialSigns,nkeys); 
	char signSerial[dpabcSignByteSize()];
	dpabcSignToBytes(signSerial,combinedSignature);
	combinedSignatureRegenerated=dpabcSignFromBytes(signSerial);
	assert_true(verify(aggrKeyRegenerated,combinedSignature,epoch,(const Zp **)attributes));
	token=presentZkToken(aggrKeyRegenerated,combinedSignatureRegenerated,epoch,(const Zp **)attributes,indexReveal,nIndexReveal,msg,msgLength);
	char tokenSerial[dpabcZkByteSize(token)];
	dpabcZkToBytes(tokenSerial,token);
	tokenRegenerated=dpabcZkFromBytes(tokenSerial);
	revealedAttributes[0]=zpCopy(attributes[0]);
	revealedAttributes[1]=zpCopy(attributes[2]);
	assert_true(verifyZkToken(tokenRegenerated,aggrKeyRegenerated,epoch,(const Zp **)revealedAttributes,indexReveal,nIndexReveal,msg,msgLength));
	for(int i=0;i<nattr;i++)
		zpFree(attributes[i]);
	for(int i=0;i<nkeys;i++){
		dpabcPkFree(pks[i]);
		dpabcSkFree(sks[i]);
		dpabcSignFree(partialSigns[i]);
	}
	dpabcSignFree(combinedSignature);
	dpabcSignFree(combinedSignatureRegenerated);
	dpabcPkFree(aggrKey);
	dpabcPkFree(aggrKeyRegenerated);
	for(int i=0;i<nIndexReveal;i++)
		zpFree(revealedAttributes[i]);
	dpabcZkFree(token);
	dpabcZkFree(tokenRegenerated);
	rgFree(rng);
	dpabcFreeStateData();
}

int main()
{
    const struct CMUnitTest dpabctests[] =
    {
        cmocka_unit_test(test_hidden_indexes_computation),
		cmocka_unit_test(test_simple_complete_flow),
		cmocka_unit_test(test_fraudulent_modifications_flow),
		cmocka_unit_test(test_flow_with_serialization)
    };
	//cmocka_set_message_output(CM_OUTPUT_XML);
	// Define environment variable CMOCKA_XML_FILE=testresults/libc.xml 
    return cmocka_run_group_tests(dpabctests, NULL, NULL);
}