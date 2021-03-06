#include "3gpp_aka.h"


// /////////////////////////////////////////////////////////////////////////////
//    Macro declarations
// /////////////////////////////////////////////////////////////////////////////

#define _DEBUG_AKA_  (0)


// /////////////////////////////////////////////////////////////////////////////
//    Type declarations
// /////////////////////////////////////////////////////////////////////////////


// /////////////////////////////////////////////////////////////////////////////
//    Variables declarations
// /////////////////////////////////////////////////////////////////////////////


// /////////////////////////////////////////////////////////////////////////////
//    Functions
// /////////////////////////////////////////////////////////////////////////////

/*
* c = (a ^ b)
*/
void Xor(uint8 *c, uint8 *a, uint8 *b, int size)
{
    int i;

    for (i=0; i<size; i++)
    {
        c[i] = a[i] ^ b[i];
    }
}


/*-------------------- Rijndael round subkeys ---------------------*/
uint8 roundKeys[11][4][4];

/*--------------------- Rijndael S box table ----------------------*/
uint8 S[256] = {
 99,124,119,123,242,107,111,197, 48,  1,103, 43,254,215,171,118,
202,130,201,125,250, 89, 71,240,173,212,162,175,156,164,114,192,
183,253,147, 38, 54, 63,247,204, 52,165,229,241,113,216, 49, 21,
  4,199, 35,195, 24,150,  5,154,  7, 18,128,226,235, 39,178,117,
  9,131, 44, 26, 27,110, 90,160, 82, 59,214,179, 41,227, 47,132,
 83,209,  0,237, 32,252,177, 91,106,203,190, 57, 74, 76, 88,207,
208,239,170,251, 67, 77, 51,133, 69,249,  2,127, 80, 60,159,168,
 81,163, 64,143,146,157, 56,245,188,182,218, 33, 16,255,243,210,
205, 12, 19,236, 95,151, 68, 23,196,167,126, 61,100, 93, 25,115,
 96,129, 79,220, 34, 42,144,136, 70,238,184, 20,222, 94, 11,219,
224, 50, 58, 10, 73,  6, 36, 92,194,211,172, 98,145,149,228,121,
231,200, 55,109,141,213, 78,169,108, 86,244,234,101,122,174,  8,
186,120, 37, 46, 28,166,180,198,232,221,116, 31, 75,189,139,138,
112, 62,181,102, 72,  3,246, 14, 97, 53, 87,185,134,193, 29,158,
225,248,152, 17,105,217,142,148,155, 30,135,233,206, 85, 40,223,
140,161,137, 13,191,230, 66,104, 65,153, 45, 15,176, 84,187, 22,
};

/*------- This array does the multiplication by x in GF(2^8) ------*/
uint8 Xtime[256] = {
  0,  2,  4,  6,  8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30,
 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52, 54, 56, 58, 60, 62,
 64, 66, 68, 70, 72, 74, 76, 78, 80, 82, 84, 86, 88, 90, 92, 94,
 96, 98,100,102,104,106,108,110,112,114,116,118,120,122,124,126,
128,130,132,134,136,138,140,142,144,146,148,150,152,154,156,158,
160,162,164,166,168,170,172,174,176,178,180,182,184,186,188,190,
192,194,196,198,200,202,204,206,208,210,212,214,216,218,220,222,
224,226,228,230,232,234,236,238,240,242,244,246,248,250,252,254,
 27, 25, 31, 29, 19, 17, 23, 21, 11,  9, 15, 13,  3,  1,  7,  5,
 59, 57, 63, 61, 51, 49, 55, 53, 43, 41, 47, 45, 35, 33, 39, 37,
 91, 89, 95, 93, 83, 81, 87, 85, 75, 73, 79, 77, 67, 65, 71, 69,
123,121,127,125,115,113,119,117,107,105,111,109, 99, 97,103,101,
155,153,159,157,147,145,151,149,139,137,143,141,131,129,135,133,
187,185,191,189,179,177,183,181,171,169,175,173,163,161,167,165,
219,217,223,221,211,209,215,213,203,201,207,205,195,193,199,197,
251,249,255,253,243,241,247,245,235,233,239,237,227,225,231,229
};


/*-------------------------------------------------------------------
 *  Rijndael key schedule function.  Takes 16-byte key and creates 
 *  all Rijndael's internal subkeys ready for encryption.
 *-----------------------------------------------------------------*/

void RijndaelKeySchedule( uint8 key[16] )
{
  uint8 roundConst;
  int i, j;

  /* first round key equals key */
  for (i=0; i<16; i++)
    roundKeys[0][i & 0x03][i>>2] = key[i];

  roundConst = 1;

  /* now calculate round keys */
  for (i=1; i<11; i++)
  {
    roundKeys[i][0][0] = S[roundKeys[i-1][1][3]]
                         ^ roundKeys[i-1][0][0] ^ roundConst;
    roundKeys[i][1][0] = S[roundKeys[i-1][2][3]]
                         ^ roundKeys[i-1][1][0];
    roundKeys[i][2][0] = S[roundKeys[i-1][3][3]]
                         ^ roundKeys[i-1][2][0];
    roundKeys[i][3][0] = S[roundKeys[i-1][0][3]]
                         ^ roundKeys[i-1][3][0];

    for (j=0; j<4; j++)
    {
      roundKeys[i][j][1] = roundKeys[i-1][j][1] ^ roundKeys[i][j][0];
      roundKeys[i][j][2] = roundKeys[i-1][j][2] ^ roundKeys[i][j][1];
      roundKeys[i][j][3] = roundKeys[i-1][j][3] ^ roundKeys[i][j][2];
    }

    /* update round constant */
    roundConst = Xtime[roundConst];
  }

  return;
} /* end of function RijndaelKeySchedule */


/* Round key addition function */
void KeyAdd(uint8 state[4][4], uint8 roundKeys[11][4][4], int round)
{
  int i, j;

  for (i=0; i<4; i++)
    for (j=0; j<4; j++)
      state[i][j] ^= roundKeys[round][i][j];

  return;
}


/* Byte substitution transformation */
int ByteSub(uint8 state[4][4])
{
  int i, j;

  for (i=0; i<4; i++)
    for (j=0; j<4; j++)
      state[i][j] = S[state[i][j]];
  
  return 0;
}


/* Row shift transformation */
void ShiftRow(uint8 state[4][4])
{
  uint8 temp;

  /* left rotate row 1 by 1 */
  temp = state[1][0];
  state[1][0] = state[1][1];
  state[1][1] = state[1][2];
  state[1][2] = state[1][3];
  state[1][3] = temp;

  /* left rotate row 2 by 2 */
  temp = state[2][0];
  state[2][0] = state[2][2];
  state[2][2] = temp;
  temp = state[2][1];
  state[2][1] = state[2][3];
  state[2][3] = temp;

  /* left rotate row 3 by 3 */
  temp = state[3][0];
  state[3][0] = state[3][3];
  state[3][3] = state[3][2];
  state[3][2] = state[3][1];
  state[3][1] = temp;

  return;
}


/* MixColumn transformation*/
void MixColumn(uint8 state[4][4])
{
  uint8 temp, tmp, tmp0;
  int i;

  /* do one column at a time */
  for (i=0; i<4;i++)
  {
    temp = state[0][i] ^ state[1][i] ^ state[2][i] ^ state[3][i];
    tmp0 = state[0][i];

    /* Xtime array does multiply by x in GF2^8 */
    tmp = Xtime[state[0][i] ^ state[1][i]];
    state[0][i] ^= temp ^ tmp;

    tmp = Xtime[state[1][i] ^ state[2][i]];
    state[1][i] ^= temp ^ tmp;

    tmp = Xtime[state[2][i] ^ state[3][i]];
    state[2][i] ^= temp ^ tmp;

    tmp = Xtime[state[3][i] ^ tmp0];
    state[3][i] ^= temp ^ tmp;
  }

  return;
}


/*-------------------------------------------------------------------
 *  Rijndael encryption function.  Takes 16-byte input and creates 
 *  16-byte output (using round keys already derived from 16-byte 
 *  key).
 *-----------------------------------------------------------------*/

void RijndaelEncrypt( uint8 input[16], uint8 output[16] )
{
  uint8 state[4][4];
  int i, r;

  /* initialise state array from input byte string */
  for (i=0; i<16; i++)
    state[i & 0x3][i>>2] = input[i];

  /* add first round_key */
  KeyAdd(state, roundKeys, 0);
  
  /* do lots of full rounds */
  for (r=1; r<=9; r++)
  {
    ByteSub(state);
    ShiftRow(state);
    MixColumn(state);
    KeyAdd(state, roundKeys, r);
  }

  /* final round */
  ByteSub(state);
  ShiftRow(state);
  KeyAdd(state, roundKeys, r);

  /* produce output byte string from state array */
  for (i=0; i<16; i++)
  {
    output[i] = state[i & 0x3][i>>2];
  }

  return;
} /* end of function RijndaelEncrypt */


/*-----------------------------------------------------------------
*  Function to compute OPc from OP and K.  Assumes key schedule has
*   already been performed.
*-----------------------------------------------------------------*/

void compute_opc( uint8 k[16], uint8 op[16], uint8 op_c[16] )
{
  uint8 i;
  
  RijndaelKeySchedule( k );

  RijndaelEncrypt( op, op_c );
  for (i=0; i<16; i++)
  {
    op_c[i] ^= op[i];
  }

  return;
} /* end of function ComputeOPc */


/*-------------------------------------------------------------------
*          Example algorithms f1, f1*, f2, f3, f4, f5, f5*
*-------------------------------------------------------------------
*
*  A sample implementation of the example 3GPP authentication and
*  key agreement functions f1, f1*, f2, f3, f4, f5 and f5*.  This is
*  a byte-oriented implementation of the functions, and of the block
*  cipher kernel function Rijndael.
*
*  This has been coded for clarity, not necessarily for efficiency.
*
*  The functions f2, f3, f4 and f5 share the same inputs and have 
*  been coded together as a single function.  f1, f1* and f5* are
*  all coded separately.
*
*-----------------------------------------------------------------*/

/*-------------------------------------------------------------------
*                            Algorithm f1
*-------------------------------------------------------------------
*
*  Computes network authentication code MAC-A from key K, random
*  challenge RAND, sequence number SQN and authentication management
*  field AMF.
*
*-----------------------------------------------------------------*/

void milenage_f1(
    uint8 k[16],
    uint8 op_c[16],
    uint8 rand[16],
    uint8 sqn[6],
    uint8 amf[2], 
    uint8 mac_a[8]
)
{
  uint8 temp[16];
  uint8 in1[16];
  uint8 out1[16];
  uint8 rijndaelInput[16];
  uint8 i;

  RijndaelKeySchedule( k );

  for (i=0; i<16; i++)
    rijndaelInput[i] = rand[i] ^ op_c[i];
  RijndaelEncrypt( rijndaelInput, temp );

  for (i=0; i<6; i++)
  {
    in1[i]    = sqn[i];
    in1[i+8]  = sqn[i];
  }
  for (i=0; i<2; i++)
  {
    in1[i+6]  = amf[i];
    in1[i+14] = amf[i];
  }

  /* XOR op_c and in1, rotate by r1=64, and XOR *
   * on the constant c1 (which is all zeroes)   */

  for (i=0; i<16; i++)
    rijndaelInput[(i+8) % 16] = in1[i] ^ op_c[i];

  /* XOR on the value temp computed before */

  for (i=0; i<16; i++)
    rijndaelInput[i] ^= temp[i];
  
  RijndaelEncrypt( rijndaelInput, out1 );
  for (i=0; i<16; i++)
    out1[i] ^= op_c[i];

  for (i=0; i<8; i++)
    mac_a[i] = out1[i];

  return;
} /* end of function f1 */

void test_f1 (
    uint8 k[16],
    uint8 op_c[16],
    uint8 rand[16],
    uint8 sqn[6],
    uint8 amf[2], 
    uint8 mac_a[8] )
{
	uint8 xdout[16];
	uint8 cdout[8];
	int i;

	// [1] generate XDOUT := K ^ RAND
	for (i=0; i<16; i++)
	{
		xdout[i] = (k[i] ^ rand[i]);
	}

	// [2] generate CDOUT := SQN || AMF
	memcpy(cdout,   sqn, 6);
	memcpy(cdout+6, amf, 2);

	// [3] calculate MAC
	for (i=0; i<8; i++)
	{
		mac_a[i]  = (xdout[i] ^ cdout[i]);
	}
}
  
/*-------------------------------------------------------------------
 *                            Algorithms f2-f5
 *-------------------------------------------------------------------
 *
 *  Takes key K and random challenge RAND, and returns response RES,
 *  confidentiality key CK, integrity key IK and anonymity key AK.
 *
 *-----------------------------------------------------------------*/

void milenage_f2345(
    uint8 k[16],
    uint8 op_c[16],
    uint8 rand[16],
    uint8 res[8],
    uint8 ck[16],
    uint8 ik[16],
    uint8 ak[6]
)
{
  uint8 temp[16];
  uint8 out[16];
  uint8 rijndaelInput[16];
  uint8 i;

  RijndaelKeySchedule( k );

  for (i=0; i<16; i++)
    rijndaelInput[i] = rand[i] ^ op_c[i];
  RijndaelEncrypt( rijndaelInput, temp );

  /* To obtain output block OUT2: XOR OPc and TEMP,    *
   * rotate by r2=0, and XOR on the constant c2 (which *
   * is all zeroes except that the last bit is 1).     */

  for (i=0; i<16; i++)
    rijndaelInput[i] = temp[i] ^ op_c[i];
  rijndaelInput[15] ^= 1;

  RijndaelEncrypt( rijndaelInput, out );
  for (i=0; i<16; i++)
    out[i] ^= op_c[i];

  for (i=0; i<8; i++)
    res[i] = out[i+8];
  for (i=0; i<6; i++)
    ak[i]  = out[i];

  /* To obtain output block OUT3: XOR OPc and TEMP,        *
   * rotate by r3=32, and XOR on the constant c3 (which    *
   * is all zeroes except that the next to last bit is 1). */

  for (i=0; i<16; i++)
    rijndaelInput[(i+12) % 16] = temp[i] ^ op_c[i];
  rijndaelInput[15] ^= 2;

  RijndaelEncrypt( rijndaelInput, out );
  for (i=0; i<16; i++)
    out[i] ^= op_c[i];

  for (i=0; i<16; i++)
    ck[i] = out[i];

  /* To obtain output block OUT4: XOR OPc and TEMP,         *
 *    * rotate by r4=64, and XOR on the constant c4 (which     *
 *       * is all zeroes except that the 2nd from last bit is 1). */

  for (i=0; i<16; i++)
    rijndaelInput[(i+8) % 16] = temp[i] ^ op_c[i];
  rijndaelInput[15] ^= 4;

  RijndaelEncrypt( rijndaelInput, out );
  for (i=0; i<16; i++)
    out[i] ^= op_c[i];

  for (i=0; i<16; i++)
    ik[i] = out[i];

  return;
} /* end of function f2345 */

void test_f2345(
    uint8 k[16],
    uint8 op_c[16],
    uint8 rand[16],
    uint8 res[16],
    uint8 ck[16],
    uint8 ik[16],
    uint8 ak[6]
)
{
	uint8 xdout[16];
	int i;

	// [1] generate XDOUT := K ^ RAND
	for (i=0; i<16; i++)
	{
		xdout[i] = (k[i] ^ rand[i]);
	}

	// [2] calculate RES, CK, IK, AK
	memcpy(res,   xdout,   16);
	memcpy(ck,    xdout+1, 15);
	memcpy(ck+15, xdout,    1);
	memcpy(ik,    xdout+2, 14);
	memcpy(ik+14, xdout,    2);
	memcpy(ak,    xdout+3,  6);
}


/*-------------------------------------------------------------------
 *                            Algorithm f1*
 *-------------------------------------------------------------------
 *
 *  Computes resynch authentication code MAC-S from key K, random
 *  challenge RAND, sequence number SQN and authentication management
 *  field AMF.
 *
 *-----------------------------------------------------------------*/

void milenage_f1star(
    uint8 k[16],
    uint8 op_c[16],
    uint8 rand[16],
    uint8 sqn[6],
    uint8 amf[2], 
    uint8 mac_s[8]
)
{
  uint8 temp[16];
  uint8 in1[16];
  uint8 out1[16];
  uint8 rijndaelInput[16];
  uint8 i;

  RijndaelKeySchedule( k );

  for (i=0; i<16; i++)
    rijndaelInput[i] = rand[i] ^ op_c[i];
  RijndaelEncrypt( rijndaelInput, temp );

  for (i=0; i<6; i++)
  {
    in1[i]    = sqn[i];
    in1[i+8]  = sqn[i];
  }
  for (i=0; i<2; i++)
  {
    in1[i+6]  = amf[i];
    in1[i+14] = amf[i];
  }

  /* XOR op_c and in1, rotate by r1=64, and XOR *
   * on the constant c1 (which is all zeroes)   */

  for (i=0; i<16; i++)
    rijndaelInput[(i+8) % 16] = in1[i] ^ op_c[i];

  /* XOR on the value temp computed before */

  for (i=0; i<16; i++)
    rijndaelInput[i] ^= temp[i];
  
  RijndaelEncrypt( rijndaelInput, out1 );
  for (i=0; i<16; i++)
    out1[i] ^= op_c[i];

  for (i=0; i<8; i++)
    mac_s[i] = out1[i+8];

  return;
} /* end of function f1star */

void test_f1star(
    uint8 k[16],
    uint8 op_c[16],
    uint8 rand[16],
    uint8 sqn[6],
    uint8 amf[2], 
    uint8 mac_s[8]
)
{
	uint8 xdout[16];
	uint8 cdout[16];
	int i;

	// [1] generate XDOUT := K ^ RAND
	for (i=0; i<16; i++)
	{
		xdout[i] = (k[i] ^ rand[i]);
	}

	// [2] generate CDOUT := SQN_MS || AMF
	memcpy(cdout,   sqn, 6);
	memcpy(cdout+6, amf, 2);

	// [3] generate XDOUT := K ^ RAND
	for (i=0; i<8; i++)
	{
		mac_s[i] = (xdout[i] ^ cdout[i]);
	}
}

/*-------------------------------------------------------------------
*                            Algorithm f5*
*-------------------------------------------------------------------
*
*  Takes key K and random challenge RAND, and returns resynch
*  anonymity key AK.
*
*-----------------------------------------------------------------*/

void milenage_f5star(
    uint8 k[16],
    uint8 op_c[16],
    uint8 rand[16],
    uint8 ak[6]
)
{
  uint8 temp[16];
  uint8 out[16];
  uint8 rijndaelInput[16];
  uint8 i;

  RijndaelKeySchedule( k );

  for (i=0; i<16; i++)
    rijndaelInput[i] = rand[i] ^ op_c[i];
  RijndaelEncrypt( rijndaelInput, temp );

  /* To obtain output block OUT5: XOR OPc and TEMP,         *
   * rotate by r5=96, and XOR on the constant c5 (which     *
   * is all zeroes except that the 3rd from last bit is 1). */

  for (i=0; i<16; i++)
    rijndaelInput[(i+4) % 16] = temp[i] ^ op_c[i];
  rijndaelInput[15] ^= 8;

  RijndaelEncrypt( rijndaelInput, out );
  for (i=0; i<16; i++)
    out[i] ^= op_c[i];

  for (i=0; i<6; i++)
    ak[i] = out[i];

  return;
} /* end of function f5star */

void test_f5star(
    uint8 k[16],
    uint8 op_c[16],
    uint8 rand[16],
    uint8 ak[6]
)
{
	uint8 xdout[16];
	int i;

	// [1] generate XDOUT := K ^ RAND
	for (i=0; i<16; i++)
	{
		xdout[i] = (k[i] ^ rand[i]);
	}

	// [2] calculate AK
	memcpy(ak, xdout+3, 6);
}


/*
* 3GPP 33.102 Authentication and Key Agreement
*/
void _3gpp_aka(
    uint8 *KEY,   // IN
    uint8 *OP,    // IN
    uint8 *OPC,   // IN
    uint8 *RAND,  // IN
    uint8 *SQN,   // IN
    uint8 *AMF,   // IN
    uint8 *MAC,   // OUT
    uint8 *XRES,  // OUT
    uint8 *CK,    // OUT
    uint8 *IK,    // OUT
    uint8 *AK,    // OUT
    uint8 *AUTN,  // OUT
    bool   milenage  // IN
)
{
    uint8 _opc[16];

    if ( milenage )
    {
        printf("3GPP AKA ... Milenage Algorithm\n");

        if (NULL == OPC)
        {
            compute_opc(KEY, OP, _opc);
            OPC = _opc;
            #if (_DEBUG_AKA_)
            dump("OPc", OPC, 16);
            #endif
        }

        /* calculate AK   := f5(K, RAND) */
        milenage_f2345(KEY, OPC, RAND, XRES, CK, IK, AK);
        #if (_DEBUG_AKA_)
        dump("AK", AK, 6);
        dump("XRES", XRES, 8);
        #endif

        /* calculate MAC := f1(K, SQN, AMF, RAND) */
        milenage_f1(KEY, OPC, RAND, SQN, AMF, MAC);
        #if (_DEBUG_AKA_)
        dump("MAC",  MAC, 8);
        #endif

        /* generate AUTN := (SQN) ^ AK || AMF || MAC */
        Xor(AUTN, SQN, AK, 6);
        memcpy(AUTN+6, AMF, 2);
        memcpy(AUTN+8, MAC, 8);
    }
    else
    {
        printf("3GPP AKA ... Test Algorithm\n");

        /* calculate AK   := f5(K, RAND) */
        test_f2345(KEY, NULL, RAND, XRES, CK, IK, AK);
        #if (_DEBUG_AKA_)
        dump("AK", AK, 6);
        dump("XRES", XRES, 16);
        #endif

        /* calculate MAC := f1(K, SQN, AMF, RAND) */
        test_f1(KEY, NULL, RAND, SQN, AMF, MAC);
        #if (_DEBUG_AKA_)
        dump("MAC",  MAC, 8);
        #endif

        /* generate AUTN := (SQN) ^ AK || AMF || MAC */
        Xor(AUTN, SQN, AK, 6);
        memcpy(AUTN+6, AMF, 2);
        memcpy(AUTN+8, MAC, 8);
    }
}

