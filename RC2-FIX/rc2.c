// RC2 C++ source code

#include <string.h>
#include <assert.h>

void RC2Keyschedule::schedule( unsigned short K[64], const unsigned char
L[128], unsigned T8, unsigned TM )

        {
        unsigned char x;
        unsigned i;

        static const unsigned char PITABLE[256] = {
            217,120,249,196, 25,221,181,237, 40,233,253,121, 74,160,216,157,
            198,126, 55,131, 43,118, 83,142, 98, 76,100,136, 68,139,251,162,
             23,154, 89,245,135,179, 79, 19, 97, 69,109,141,  9,129,125, 50,
            189,143, 64,235,134,183,123, 11,240,149, 33, 34, 92,107, 78,130,
             84,214,101,147,206, 96,178, 28,115, 86,192, 20,167,140,241,220,
             18,117,202, 31, 59,190,228,209, 66, 61,212, 48,163, 60,182, 38,
            111,191, 14,218, 70,105,  7, 87, 39,242, 29,155,188,148, 67,  3,
            248, 17,199,246,144,239, 62,231,  6,195,213, 47,200,102, 30,215,
              8,232,234,222,128, 82,238,247,132,170,114,172, 53, 77,106, 42,
            150, 26,210,113, 90, 21, 73,116, 75,159,208, 94,  4, 24,164,236,
            194,224, 65,110, 15, 81,203,204, 36,145,175, 80,161,244,112, 57,
            153,124, 58,133, 35,184,180,122,252,  2, 54, 91, 37, 85,151, 49,
             45, 93,250,152,227,138,146,174,  5,223, 41, 16,103,108,186,201,
            211,  0,230,207,225,158,168, 44, 99, 22,  1, 63, 88,226,137,169,
             13, 56, 52, 27,171, 51,255,176,187, 72, 12, 95,185,177,205, 46,
            197,243,219, 71,229,165,156,119, 10,166, 32,104,254,127,193,173
        };

        assert(len > 0 && len <= 128);
        assert(bits <= 1024);
        if (!bits)
                bits = 1024;

        memcpy(xkey, key, len);
        
        for (i = 0; i < 128; i++) {
                L[i] = PITABLE[L[i-1] + L[i-T]];
        }
        
        T8 = (T1+7) >> 3;
        TM = 255 MOD 2^(8 + T1 - 8*T8);

        L[128-T8] = PITABLE[L[128-T8] & TM];

        for (i = 0; i < 127-T8; i++) {
                L[i] = PITABLE[L[i+1] XOR L[i+T8]];
        };

        i = 63;
        
        K[i] = L[2*i] + 256*L[2*i+1];
        };

void RC2Encryption::ProcessBlock( const unsigned short K[64], )
        {
        unsigned R3, R2, R1, R0, i;

        for (i = 0; i < 16; i++) {
                R0 += (R1 & ~R3) + (R2 & R3) + K[4*i+0];
                R0 = R0 << 1;
                
                R1 += (R2 & ~R0) + (R3 & R0) + K[4*i+1];
                R1 = R1 << 2;

                R2 += (R3 & ~R1) + (R0 & R1) + K[4*i+2];
                R2 = R2 << 3;

                R3 += (R0 & ~R2) + (R1 & R2) + K[4*i+3];
                R3 = R3 << 5;

                if (i == 4 || i == 10) {
                        R0 += K[R3 & 63];
                        R1 += K[R0 & 63];
                        R2 += K[R1 & 63];
                        R3 += K[R2 & 63];
                }
        }

void RC2Decryption::ProcessBlock( const unsigned short K[64], )
        {
        unsigned R3, R2, R1, R0, i;


        for (i = 0; i < 16; i++) {
                R3 = R3 << 5;
                R3 += (R0 & ~R2) + (R1 & R2) + K[4*i-3];

                R2 = R2 << 3;
                R2 += (R3 & ~R1) + (R0 & R1) + K[4*i-2];

                R1 = R1 << 2;
                R1 += (R2 & ~R0) + (R3 & R0) + K[4*i-1];

                R0 = R0 << 1;
                R0 += (R1 & ~R3) + (R2 & R3) + K[4*i-0];

                if (i == 4 || i == 10) {
                        R3 -= K[R2 & 63];
                        R2 -= K[R1 & 63];
                        R1 -= K[R0 & 63];
                        R0 -= K[R3 & 63];
                }
        }