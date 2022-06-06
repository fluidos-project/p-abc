#include <stdarg.h>
#include <setjmp.h>
#include <stddef.h>
#include <cmocka.h>
#include <Zp.h>
#include <g1.h>


static void test_copies(void **state)
{
    char * seed="Seed_test_copies_0123456789";
    int seedLength=27;
    ranGen * rng=rgInit(seed,seedLength);
    Zp* z=zpRandom(rng);
    G1* g1=g1Generator();
    g1Mul(g1,z);
    G1* g2=g1Copy(g1);
    G1* g3=g1Generator();
    zpAdd(z,z);
    g1Mul(g3,z);
    assert_true(g1Equals(g1,g2));
    g1Add(g1,g3);
    assert_false(g1Equals(g1,g2));
    g1Add(g2,g3);
    assert_true(g1Equals(g1,g2));
    g1CopyValue(g2,g3);
    assert_true(g1Equals(g2,g3));
    g1Add(g2,g1);
    assert_false(g1Equals(g2,g3));
    g1Add(g3,g1);
    assert_true(g1Equals(g2,g3));
    g1Free(g1);
    g1Free(g2);
    g1Free(g3);
    zpFree(z);
    rgFree(rng);
}

static void test_addition(void **state) //TODO Test using precomputed elements.
{
    char * seed="Seed_test_addition_0123456789";
    int seedLength=29;
    ranGen * rng=rgInit(seed,seedLength);
    G1* identity=g1Identity();
    Zp* z1=zpRandom(rng);
    Zp* z2=zpRandom(rng);
    G1* g1=g1Generator(); //g1=[z1]g
    g1Mul(g1,z1); 
    G1* g2=g1Generator(); //g2=[z2]g
    g1Mul(g2,z2);
    G1* g3=g1Generator();
    zpAdd(z1,z2);
    g1Mul(g3,z1);          //g3=[z1+z2]g
    G1* g4=g1Copy(g2);
    g1Add(g1,g2);
    assert_true(g1Equals(g1,g3)); //Sum is fine
    assert_true(g1Equals(g2,g4)); //Second operand not modified
    g1Add(g1,g2);
    g1Add(g2,g3);
    assert_true(g1Equals(g1,g2)); 
    g1Add(g1,identity);
    assert_true(g1Equals(g1,g2)); 
    g1Add(identity,g1);
    assert_true(g1Equals(g1,identity)); 
    g1Free(g1);
    g1Free(g2);
    g1Free(g3);
    g1Free(g4);
    g1Free(identity);
    zpFree(z1);
    zpFree(z2);
    rgFree(rng);
}



static void test_multiplication(void **state)
{
    char * seed="Seed_test_multiplication_0123456789";
    int seedLength=35;
    ranGen * rng=rgInit(seed,seedLength);
    Zp* one=zpFromInt(1);
    Zp* z1=zpFromInt(2);
    Zp* z2=zpFromInt(3);
    G1* g1=g1Generator(); 
    G1* g2=g1Generator(); 
    G1* g=g1Generator(); 
    g1Add(g1,g1);
    g1Mul(g2,z1);
    assert_true(g1Equals(g1,g2)); //g+g=[2]g
    g1Add(g1,g);
    g1CopyValue(g2,g);
    g1Mul(g2,z2);
    assert_true(g1Equals(g1,g2)); //g+g+g=[3]g
    g1CopyValue(g1,g);
    g1CopyValue(g2,g);
    zpRandomValue(rng,z1);
    g1Mul(g1,z1);
    zpAdd(z1,one);
    g1Mul(g2,z1);
    g1Add(g1,g);
    assert_true(g1Equals(g1,g2)); //[r]g+g=[r+1]g
    g1Free(g1);
    g1Free(g2);
    g1Free(g);
    zpFree(z1);
    zpFree(z2);
}

static void test_serial(void **state){
    char * seed="Seed_test_serial_0123456789";
    int seedLength=27;
    ranGen * rng=rgInit(seed,seedLength);
    Zp* z=zpRandom(rng);
    G1* g1=g1Generator();
    G1* g2;
    int nbytes=g1ByteSize();
    char bytes1[nbytes];
    g1ToBytes(bytes1,g1);
    g2=g1FromBytes(bytes1);
    assert_true(g1Equals(g1,g2));
    g1Free(g2);
    bytes1[3]+=1;
    g2=g1FromBytes(bytes1);
    assert_false(g1Equals(g1,g2));
    zpFree(z);
    g1Free(g1);
    g1Free(g2);
    rgFree(rng);
}

int main()
{
    const struct CMUnitTest g1tests[] =
    {
        cmocka_unit_test(test_copies),
        cmocka_unit_test(test_addition),
        cmocka_unit_test(test_multiplication),
        cmocka_unit_test(test_serial)
    };
    //cmocka_set_message_output(CM_OUTPUT_XML);
	// Define environment variable CMOCKA_XML_FILE=testresults/libc.xml 
    return cmocka_run_group_tests(g1tests, NULL, NULL);
}