
#include <unity.h>
#include <string.h>
#include <TextUtilities.h>

void testTcUtilIntegerConversions() {
        char szBuffer[20];

        // first check the basic cases for the version that always starts at pos 0
        strcpy(szBuffer, "abc");
        ltoaClrBuff(szBuffer, 1234, 4, ' ', sizeof(szBuffer));
        TEST_ASSERT_EQUAL_STRING(szBuffer, "1234");
        ltoaClrBuff(szBuffer, 907, 4, ' ', sizeof(szBuffer));
        TEST_ASSERT_EQUAL_STRING(szBuffer, " 907");
        ltoaClrBuff(szBuffer, 22, 4, '0', sizeof(szBuffer));
        TEST_ASSERT_EQUAL_STRING(szBuffer, "0022");
        ltoaClrBuff(szBuffer, -22, 4, '0', sizeof(szBuffer));
        TEST_ASSERT_EQUAL_STRING(szBuffer, "-0022");
        ltoaClrBuff(szBuffer, -93, 2, NOT_PADDED, sizeof(szBuffer));
        TEST_ASSERT_EQUAL_STRING(szBuffer, "-93");
        ltoaClrBuff(szBuffer, 0, 4, NOT_PADDED, sizeof(szBuffer));
        TEST_ASSERT_EQUAL_STRING(szBuffer, "0");

        // and now test the appending version with zero padding
        strcpy(szBuffer, "val = ");
        fastltoa(szBuffer, 22, 4, '0', sizeof(szBuffer));
        TEST_ASSERT_EQUAL_STRING(szBuffer, "val = 0022");

        // and now test the appending version with an absolute divisor.
        strcpy(szBuffer, "val = ");
        fastltoa_mv(szBuffer, 22, 1000, '0', sizeof(szBuffer));
        TEST_ASSERT_EQUAL_STRING(szBuffer, "val = 022");

        // and lasty try the divisor version without 0.
        strcpy(szBuffer, "val = ");
        fastltoa_mv(szBuffer, 22, 10000, NOT_PADDED, sizeof(szBuffer));
        TEST_ASSERT_EQUAL_STRING(szBuffer, "val = 22");

        // and now try something bigger than the divisor
        strcpy(szBuffer, "val = ");
        fastltoa_mv(szBuffer, 222222, 10000, NOT_PADDED, sizeof(szBuffer));
        TEST_ASSERT_EQUAL_STRING(szBuffer, "val = 2222");
}

void testTcUtilHexCoversions() {
    char szBuffer[20];
    TEST_ASSERT_EQUAL('0', hexChar(0));
    TEST_ASSERT_EQUAL('9', hexChar(9));
    TEST_ASSERT_EQUAL('A', hexChar(10));
    TEST_ASSERT_EQUAL('F', hexChar(15));

    intToHexString(szBuffer, sizeof szBuffer, 0xfade, 4, true);
    TEST_ASSERT_EQUAL_STRING("0xFADE", szBuffer);

    intToHexString(szBuffer, sizeof szBuffer, 0x0000, 4, true);
    TEST_ASSERT_EQUAL_STRING("0x0000", szBuffer);
}

void testTcUtilFloatConversions() {
    char szBuffer[20];

    fastftoa(szBuffer, 12.30F, 4, sizeof szBuffer);
    TEST_ASSERT_EQUAL_STRING("12.3000", szBuffer);

    szBuffer[0]=0;
    fastftoa(szBuffer, -10.30F, 1, sizeof szBuffer);
    TEST_ASSERT_EQUAL_STRING("-10.3", szBuffer);
}

void testTcUtilLimits() {
    char szBuffer[20];

    // Test very large value
    szBuffer[0]=0;
    intToHexString(szBuffer, sizeof szBuffer, 0xFFF0FF0F, 8, true);
    TEST_ASSERT_EQUAL_STRING("0xFFF0FF0F", szBuffer);

    // test very small value with large number of places
    intToHexString(szBuffer, sizeof szBuffer, 0x1, 8, true);
    TEST_ASSERT_EQUAL_STRING("0x00000001", szBuffer);

    // test limits - method does nothing when buffer too small.
    intToHexString(szBuffer, 5, 0xFFF00000, 8, true);
    TEST_ASSERT_EQUAL_STRING("", szBuffer);

    // test large integers
    ltoaClrBuff(szBuffer, 147483647, 9, NOT_PADDED, sizeof szBuffer);
    TEST_ASSERT_EQUAL_STRING("147483647", szBuffer);
    ltoaClrBuff(szBuffer, -147483647, 9, NOT_PADDED, sizeof szBuffer);
    TEST_ASSERT_EQUAL_STRING("-147483647", szBuffer);


    ltoaClrBuff(szBuffer, -2147483647, 9, NOT_PADDED, 5);
    TEST_ASSERT_EQUAL_STRING("-147", szBuffer);

}


void setup() {
    UNITY_BEGIN();
    RUN_TEST(testTcUtilHexCoversions);
    RUN_TEST(testTcUtilIntegerConversions);
    RUN_TEST(testTcUtilFloatConversions);
    RUN_TEST(testTcUtilLimits);
    UNITY_END();
}

void loop() {}