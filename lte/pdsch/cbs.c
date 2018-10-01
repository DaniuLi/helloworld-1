#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#define CEIL(x, y)   (((x) + ((y) - 1)) / (y))
#define FLOOR(x, y)  ((x) / (y))

#define Z  (6144)

#define MAX_ITBS  (27)
#define MAX_NPRB  (110)

/*
* TS 36.213 Table 7.1.7.2.1-1: Transport block size table (dimension 27 ��110)
*/
int tb_size[] = 
{
    // Nprb: 1 to 10
       16,    32,    56,    88,   120,   152,   176,   208,   224,   256,
       24,    56,    88,   144,   176,   208,   224,   256,   328,   344,
       32,    72,   144,   176,   208,   256,   296,   328,   376,   424,
       40,   104,   176,   208,   256,   328,   392,   440,   504,   568,
       56,   120,   208,   256,   328,   408,   488,   552,   632,   696,
       72,   144,   224,   328,   424,   504,   600,   680,   776,   872,
      328,   176,   256,   392,   504,   600,   712,   808,   936,  1032,
      104,   224,   328,   472,   584,   712,   840,   968,  1096,  1224,
      120,   256,   392,   536,   680,   808,   968,  1096,  1256,  1384,
      136,   296,   456,   616,   776,   936,  1096,  1256,  1416,  1544,
      144,   328,   504,   680,   872,  1032,  1224,  1384,  1544,  1736,
      176,   376,   584,   776,  1000,  1192,  1384,  1608,  1800,  2024,
      208,   440,   680,   904,  1128,  1352,  1608,  1800,  2024,  2280,
      224,   488,   744,  1000,  1256,  1544,  1800,  2024,  2280,  2536,
      256,   552,   840,  1128,  1416,  1736,  1992,  2280,  2600,  2856,
      280,   600,   904,  1224,  1544,  1800,  2152,  2472,  2728,  3112,
      328,   632,   968,  1288,  1608,  1928,  2280,  2600,  2984,  3240,
      336,   696,  1064,  1416,  1800,  2152,  2536,  2856,  3240,  3624,
      376,   776,  1160,  1544,  1992,  2344,  2792,  3112,  3624,  4008,
      408,   840,  1288,  1736,  2152,  2600,  2984,  3496,  3880,  4264,
      440,   904,  1384,  1864,  2344,  2792,  3240,  3752,  4136,  4584,
      488,  1000,  1480,  1992,  2472,  2984,  3496,  4008,  4584,  4968,
      520,  1064,  1608,  2152,  2664,  3240,  3752,  4264,  4776,  5352,
      552,  1128,  1736,  2280,  2856,  3496,  4008,  4584,  5160,  5736,
      584,  1192,  1800,  2408,  2984,  3624,  4264,  4968,  5544,  5992,
      616,  1256,  1864,  2536,  3112,  3752,  4392,  5160,  5736,  6200,
      712,  1480,  2216,  2984,  3752,  4392,  5160,  5992,  6712,  7480,

    // Nprb: 11 to 20
      288,   328,   344,   376,   392,   424,   456,   488,   504,   536,
      376,   424,   456,   488,   520,   568,   600,   632,   680,   712,
      472,   520,   568,   616,   648,   696,   744,   776,   840,   872,
      616,   680,   744,   808,   872,   904,   968,  1032,  1096,  1160,
      776,   840,   904,  1000,  1064,  1128,  1192,  1288,  1352,  1416,
      968,  1032,  1128,  1224,  1320,  1384,  1480,  1544,  1672,  1736,
     1128,  1224,  1352,  1480,  1544,  1672,  1736,  1864,  1992,  2088,
     1320,  1480,  1608,  1672,  1800,  1928,  2088,  2216,  2344,  2472,
     1544,  1672,  1800,  1928,  2088,  2216,  2344,  2536,  2664,  2792,
     1736,  1864,  2024,  2216,  2344,  2536,  2664,  2856,  2984,  3112,
     1928,  2088,  2280,  2472,  2664,  2792,  2984,  3112,  3368,  3496,
     2216,  2408,  2600,  2792,  2984,  3240,  3496,  3624,  3880,  4008,
     2472,  2728,  2984,  3240,  3368,  3624,  3880,  4136,  4392,  4584,
     2856,  3112,  3368,  3624,  3880,  4136,  4392,  4584,  4968,  5160,
     3112,  3496,  3752,  4008,  4264,  4584,  4968,  5160,  5544,  5736,
     3368,  3624,  4008,  4264,  4584,  4968,  5160,  5544,  5736,  6200,
     3624,  3880,  4264,  4584,  4968,  5160,  5544,  5992,  6200,  6456,
     4008,  4392,  4776,  5160,  5352,  5736,  6200,  6456,  6712,  7224,
     4392,  4776,  5160,  5544,  5992,  6200,  6712,  7224,  7480,  7992,
     4776,  5160,  5544,  5992,  6456,  6968,  7224,  7736,  8248,  8504,
     5160,  5544,  5992,  6456,  6968,  7480,  7992,  8248,  8760,  9144,
     5544,  5992,  6456,  6968,  7480,  7992,  8504,  9144,  9528,  9912,
     5992,  6456,  6968,  7480,  7992,  8504,  9144,  9528, 10296, 10680,
     6200,  6968,  7480,  7992,  8504,  9144,  9912, 10296, 11064, 11448,
     6712,  7224,  7992,  8504,  9144,  9912, 10296, 11064, 11448, 12216,
     6968,  7480,  8248,  8760,  9528, 10296, 10680, 11448, 12216, 12576,
     8248,  8760,  9528, 10296, 11064, 11832, 12576, 13536, 14112, 14688,

    // Nprb: 21 to 30
      568,   600,   616,   648,   680,   712,   744,   776,   776,   808,
      744,   776,   808,   872,   904,   936,   968,  1000,  1032,  1064,
      936,   968,  1000,  1064,  1096,  1160,  1192,  1256,  1288,  1320,
     1224,  1256,  1320,  1384,  1416,  1480,  1544,  1608,  1672,  1736,
     1480,  1544,  1608,  1736,  1800,  1864,  1928,  1992,  2088,  2152,
     1864,  1928,  2024,  2088,  2216,  2280,  2344,  2472,  2536,  2664,
     2216,  2280,  2408,  2472,  2600,  2728,  2792,  2984,  2984,  3112,
     2536,  2664,  2792,  2984,  3112,  3240,  3368,  3368,  3496,  3624,
     2984,  3112,  3240,  3368,  3496,  3624,  3752,  3880,  4008,  4264,
     3368,  3496,  3624,  3752,  4008,  4136,  4264,  4392,  4584,  4776,
     3752,  3880,  4008,  4264,  4392,  4584,  4776,  4968,  5160,  5352,
     4264,  4392,  4584,  4776,  4968,  5352,  5544,  5736,  5992,  5992,
     4776,  4968,  5352,  5544,  5736,  5992,  6200,  6456,  6712,  6712,
     5352,  5736,  5992,  6200,  6456,  6712,  6968,  7224,  7480,  7736,
     5992,  6200,  6456,  6968,  7224,  7480,  7736,  7992,  8248,  8504,
     6456,  6712,  6968,  7224,  7736,  7992,  8248,  8504,  8760,  9144,
     6712,  7224,  7480,  7736,  7992,  8504,  8760,  9144,  9528,  9912,
     7480,  7992,  8248,  8760,  9144,  9528,  9912, 10296, 10296, 10680,
     8248,  8760,  9144,  9528,  9912, 10296, 10680, 11064, 11448, 11832,
     9144,  9528,  9912, 10296, 10680, 11064, 11448, 12216, 12576, 12960,
     9912, 10296, 10680, 11064, 11448, 12216, 12576, 12960, 13536, 14112,
    10680, 11064, 11448, 12216, 12576, 12960, 13536, 14112, 14688, 15264,
    11448, 11832, 12576, 12960, 13536, 14112, 14688, 15264, 15840, 16416,
    12216, 12576, 12960, 13536, 14112, 14688, 15264, 15840, 16416, 16992,
    12960, 13536, 14112, 14688, 15264, 15840, 16416, 16992, 17568, 18336,
    13536, 14112, 14688, 15264, 15840, 16416, 16992, 17568, 18336, 19080,
    15264, 16416, 16992, 17568, 18336, 19080, 19848, 20616, 21384, 22152,

    // Nprb: 31 to 40
      840,   872,   904,   936,   968,  1000,  1032,  1032,  1064,  1096,
     1128,  1160,  1192,  1224,  1256,  1288,  1352,  1384,  1416,  1416,
     1384,  1416,  1480,  1544,  1544,  1608,  1672,  1672,  1736,  1800,
     1800,  1864,  1928,  1992,  2024,  2088,  2152,  2216,  2280,  2344,
     2216,  2280,  2344,  2408,  2472,  2600,  2664,  2728,  2792,  2856,
     2728,  2792,  2856,  2984,  3112,  3112,  3240,  3368,  3496,  3496,
     3240,  3368,  3496,  3496,  3624,  3752,  3880,  4008,  4136,  4136,
     3752,  3880,  4008,  4136,  4264,  4392,  4584,  4584,  4776,  4968,
     4392,  4584,  4584,  4776,  4968,  4968,  5160,  5352,  5544,  5544,
     4968,  5160,  5160,  5352,  5544,  5736,  5736,  5992,  6200,  6200,
     5544,  5736,  5736,  5992,  6200,  6200,  6456,  6712,  6712,  6968,
     6200,  6456,  6712,  6968,  6968,  7224,  7480,  7736,  7736,  7992,
     6968,  7224,  7480,  7736,  7992,  8248,  8504,  8760,  8760,  9144,
     7992,  8248,  8504,  8760,  9144,  9144,  9528,  9912,  9912, 10296,
     8760,  9144,  9528,  9912,  9912, 10296, 10680, 11064, 11064, 11448,
     9528,  9912, 10296, 10296, 10680, 11064, 11448, 11832, 11832, 12216,
     9912, 10296, 10680, 11064, 11448, 11832, 12216, 12216, 12576, 12960,
    11064, 11448, 11832, 12216, 12576, 12960, 13536, 13536, 14112, 14688,
    12216, 12576, 12960, 13536, 14112, 14112, 14688, 15264, 15264, 15840,
    13536, 13536, 14112, 14688, 15264, 15264, 15840, 16416, 16992, 16992,
    14688, 14688, 15264, 15840, 16416, 16992, 16992, 17568, 18336, 18336,
    15840, 15840, 16416, 16992, 17568, 18336, 18336, 19080, 19848, 19848,
    16992, 16992, 17568, 18336, 19080, 19080, 19848, 20616, 21384, 21384,
    17568, 18336, 19080, 19848, 19848, 20616, 21384, 22152, 22152, 22920,
    19080, 19848, 19848, 20616, 21384, 22152, 22920, 22920, 23688, 24496,
    19848, 20616, 20616, 21384, 22152, 22920, 23688, 24496, 24496, 25456,
    22920, 23688, 24496, 25456, 25456, 26416, 27376, 28336, 29296, 29296,

    // Nprb: 41 to 50
     1128,  1160,  1192,  1224,  1256,  1256,  1288,  1320,  1352,  1384,
     1480,  1544,  1544,  1608,  1608,  1672,  1736,  1736,  1800,  1800,
     1800,  1864,  1928,  1992,  2024,  2088,  2088,  2152,  2216,  2216,
     2408,  2472,  2536,  2536,  2600,  2664,  2728,  2792,  2856,  2856,
     2984,  2984,  3112,  3112,  3240,  3240,  3368,  3496,  3496,  3624,
     3624,  3752,  3752,  3880,  4008,  4008,  4136,  4264,  4392,  4392,
     4264,  4392,  4584,  4584,  4776,  4776,  4968,  4968,  5160,  5160,
     4968,  5160,  5352,  5352,  5544,  5736,  5736,  5992,  5992,  6200,
     5736,  5992,  5992,  6200,  6200,  6456,  6456,  6712,  6968,  6968,
     6456,  6712,  6712,  6968,  6968,  7224,  7480,  7480,  7736,  7992,
     7224,  7480,  7480,  7736,  7992,  7992,  8248,  8504,  8504,  8760,
     8248,  8504,  8760,  8760,  9144,  9144,  9528,  9528,  9912,  9912,
     9528,  9528,  9912,  9912, 10296, 10680, 10680, 11064, 11064, 11448,
    10680, 10680, 11064, 11448, 11448, 11832, 12216, 12216, 12576, 12960,
    11832, 12216, 12216, 12576, 12960, 12960, 13536, 13536, 14112, 14112,
    12576, 12960, 12960, 13536, 13536, 14112, 14688, 14688, 15264, 15264,
    13536, 13536, 14112, 14112, 14688, 14688, 15264, 15840, 15840, 16416,
    14688, 15264, 15264, 15840, 16416, 16416, 16992, 17568, 17568, 18336,
    16416, 16416, 16992, 17568, 17568, 18336, 18336, 19080, 19080, 19848,
    17568, 18336, 18336, 19080, 19080, 19848, 20616, 20616, 21384, 21384,
    19080, 19848, 19848, 20616, 20616, 21384, 22152, 22152, 22920, 22920,
    20616, 21384, 21384, 22152, 22920, 22920, 23688, 24496, 24496, 25456,
    22152, 22920, 22920, 23688, 24496, 24496, 25456, 25456, 26416, 27376,
    23688, 24496, 24496, 25456, 25456, 26416, 27376, 27376, 28336, 28336,
    25456, 25456, 26416, 26416, 27376, 28336, 28336, 29296, 29296, 30576,
    26416, 26416, 27376, 28336, 28336, 29296, 29296, 30576, 31704, 31704,
    30576, 30576, 31704, 32856, 32856, 34008, 35160, 35160, 36696, 36696,

    // Nprb: 51 to 60
     1416,  1416,  1480,  1480,  1544,  1544,  1608,  1608,  1608,  1672,
     1864,  1864,  1928,  1992,  1992,  2024,  2088,  2088,  2152,  2152,
     2280,  2344,  2344,  2408,  2472,  2536,  2536,  2600,  2664,  2664,
     2984,  2984,  3112,  3112,  3240,  3240,  3368,  3368,  3496,  3496,
     3624,  3752,  3752,  3880,  4008,  4008,  4136,  4136,  4264,  4264,
     4584,  4584,  4776,  4776,  4776,  4968,  4968,  5160,  5160,  5352,
     5352,  5352,  5544,  5736,  5736,  5992,  5992,  5992,  6200,  6200,
     6200,  6456,  6456,  6712,  6712,  6712,  6968,  6968,  7224,  7224,
     7224,  7224,  7480,  7480,  7736,  7736,  7992,  7992,  8248,  8504,
     7992,  8248,  8248,  8504,  8760,  8760,  9144,  9144,  9144,  9528,
     9144,  9144,  9144,  9528,  9528,  9912,  9912, 10296, 10296, 10680,
    10296, 10680, 10680, 11064, 11064, 11448, 11448, 11832, 11832, 12216,
    11832, 11832, 12216, 12216, 12576, 12576, 12960, 12960, 13536, 13536,
    12960, 13536, 13536, 14112, 14112, 14688, 14688, 14688, 15264, 15264,
    14688, 14688, 15264, 15264, 15840, 15840, 16416, 16416, 16992, 16992,
    15840, 15840, 16416, 16416, 16992, 16992, 17568, 17568, 18336, 18336,
    16416, 16992, 16992, 17568, 17568, 18336, 18336, 19080, 19080, 19848,
    18336, 19080, 19080, 19848, 19848, 20616, 20616, 20616, 21384, 21384,
    19848, 20616, 21384, 21384, 22152, 22152, 22920, 22920, 23688, 23688,
    22152, 22152, 22920, 22920, 23688, 24496, 24496, 25456, 25456, 25456,
    23688, 24496, 24496, 25456, 25456, 26416, 26416, 27376, 27376, 28336,
    25456, 26416, 26416, 27376, 27376, 28336, 28336, 29296, 29296, 30576,
    27376, 28336, 28336, 29296, 29296, 30576, 30576, 31704, 31704, 32856,
    29296, 29296, 30576, 30576, 31704, 31704, 32856, 32856, 34008, 34008,
    31704, 31704, 32856, 32856, 34008, 34008, 35160, 35160, 36696, 36696,
    32856, 32856, 34008, 34008, 35160, 35160, 36696, 36696, 37888, 37888,
    37888, 37888, 39232, 40576, 40576, 40576, 42368, 42368, 43816, 43816,

    // Nprb: 61 to 70
     1672,  1736,  1736,  1800,  1800,  1800,  1864,  1864,  1928,  1928,
     2216,  2280,  2280,  2344,  2344,  2408,  2472,  2472,  2536,  2536,
     2728,  2792,  2856,  2856,  2856,  2984,  2984,  3112,  3112,  3112,
     3624,  3624,  3624,  3752,  3752,  3880,  3880,  4008,  4008,  4136,
     4392,  4392,  4584,  4584,  4584,  4776,  4776,  4968,  4968,  4968,
     5352,  5544,  5544,  5736,  5736,  5736,  5992,  5992,  5992,  6200,
     6456,  6456,  6456,  6712,  6712,  6968,  6968,  6968,  7224,  7224,
     7480,  7480,  7736,  7736,  7992,  7992,  8248,  8248,  8504,  8504,
     8504,  8760,  8760,  9144,  9144,  9144,  9528,  9528,  9528,  9912,
     9528,  9912,  9912, 10296, 10296, 10296, 10680, 10680, 11064, 11064,
    10680, 11064, 11064, 11448, 11448, 11448, 11832, 11832, 12216, 12216,
    12216, 12576, 12576, 12960, 12960, 13536, 13536, 13536, 14112, 14112,
    14112, 14112, 14112, 14688, 14688, 15264, 15264, 15264, 15840, 15840,
    15840, 15840, 16416, 16416, 16992, 16992, 16992, 17568, 17568, 18336,
    17568, 17568, 18336, 18336, 18336, 19080, 19080, 19848, 19848, 19848,
    18336, 19080, 19080, 19848, 19848, 20616, 20616, 20616, 21384, 21384,
    19848, 19848, 20616, 20616, 21384, 21384, 22152, 22152, 22152, 22920,
    22152, 22152, 22920, 22920, 23688, 23688, 24496, 24496, 24496, 25456,
    24496, 24496, 24496, 25456, 25456, 26416, 26416, 27376, 27376, 27376,
    26416, 26416, 27376, 27376, 28336, 28336, 29296, 29296, 29296, 30576,
    28336, 29296, 29296, 29296, 30576, 30576, 31704, 31704, 31704, 32856,
    30576, 31704, 31704, 31704, 32856, 32856, 34008, 34008, 35160, 35160,
    32856, 34008, 34008, 34008, 35160, 35160, 36696, 36696, 36696, 37888,
    35160, 35160, 36696, 36696, 37888, 37888, 37888, 39232, 39232, 40576,
    36696, 37888, 37888, 39232, 39232, 40576, 40576, 42368, 42368, 42368,
    39232, 39232, 40576, 40576, 40576, 42368, 42368, 43816, 43816, 43816,
    45352, 45352, 46888, 46888, 48936, 48936, 48936, 51024, 51024, 52752,

    // Nprb: 71 to 80
     1992,  1992,  2024,  2088,  2088,  2088,  2152,  2152,  2216,  2216,
     2600,  2600,  2664,  2728,  2728,  2792,  2792,  2856,  2856,  2856,
     3240,  3240,  3240,  3368,  3368,  3368,  3496,  3496,  3496,  3624,
     4136,  4264,  4264,  4392,  4392,  4392,  4584,  4584,  4584,  4776,
     5160,  5160,  5160,  5352,  5352,  5544,  5544,  5544,  5736,  5736,
     6200,  6200,  6456,  6456,  6712,  6712,  6712,  6968,  6968,  6968,
     7480,  7480,  7736,  7736,  7736,  7992,  7992,  8248,  8248,  8248,
     8760,  8760,  8760,  9144,  9144,  9144,  9528,  9528,  9528,  9912,
     9912,  9912, 10296, 10296, 10680, 10680, 10680, 11064, 11064, 11064,
    11064, 11448, 11448, 11832, 11832, 11832, 12216, 12216, 12576, 12576,
    12576, 12576, 12960, 12960, 12960, 13536, 13536, 13536, 14112, 14112,
    14112, 14688, 14688, 14688, 15264, 15264, 15840, 15840, 15840, 16416,
    16416, 16416, 16416, 16992, 16992, 17568, 17568, 17568, 18336, 18336,
    18336, 18336, 19080, 19080, 19080, 19848, 19848, 19848, 20616, 20616,
    20616, 20616, 20616, 21384, 21384, 22152, 22152, 22152, 22920, 22920,
    22152, 22152, 22152, 22920, 22920, 23688, 23688, 23688, 24496, 24496,
    22920, 23688, 23688, 24496, 24496, 24496, 25456, 25456, 25456, 26416,
    25456, 26416, 26416, 26416, 27376, 27376, 27376, 28336, 28336, 29296,
    28336, 28336, 29296, 29296, 29296, 30576, 30576, 30576, 31704, 31704,
    30576, 30576, 31704, 31704, 32856, 32856, 32856, 34008, 34008, 34008,
    32856, 34008, 34008, 34008, 35160, 35160, 35160, 36696, 36696, 36696,
    35160, 36696, 36696, 36696, 37888, 37888, 39232, 39232, 39232, 40576,
    37888, 39232, 39232, 40576, 40576, 40576, 42368, 42368, 42368, 43816,
    40576, 40576, 42368, 42368, 43816, 43816, 43816, 45352, 45352, 45352,
    43816, 43816, 45352, 45352, 45352, 46888, 46888, 46888, 48936, 48936,
    45352, 45352, 46888, 46888, 46888, 48936, 48936, 48936, 51024, 51024,
    52752, 52752, 55056, 55056, 55056, 55056, 57336, 57336, 57336, 59256,

    // Nprb: 81 to 90
     2280,  2280,  2280,  2344,  2344,  2408,  2408,  2472,  2472,  2536,
     2984,  2984,  2984,  3112,  3112,  3112,  3240,  3240,  3240,  3240,
     3624,  3624,  3752,  3752,  3880,  3880,  3880,  4008,  4008,  4008,
     4776,  4776,  4776,  4968,  4968,  4968,  5160,  5160,  5160,  5352,
     5736,  5992,  5992,  5992,  5992,  6200,  6200,  6200,  6456,  6456,
     7224,  7224,  7224,  7480,  7480,  7480,  7736,  7736,  7736,  7992,
     8504,  8504,  8760,  8760,  8760,  9144,  9144,  9144,  9144,  9528,
     9912,  9912, 10296, 10296, 10296, 10680, 10680, 10680, 11064, 11064,
    11448, 11448, 11448, 11832, 11832, 12216, 12216, 12216, 12576, 12576,
    12960, 12960, 12960, 13536, 13536, 13536, 13536, 14112, 14112, 14112,
    14112, 14688, 14688, 14688, 14688, 15264, 15264, 15264, 15840, 15840,
    16416, 16416, 16992, 16992, 16992, 17568, 17568, 17568, 18336, 18336,
    18336, 19080, 19080, 19080, 19080, 19848, 19848, 19848, 20616, 20616,
    20616, 21384, 21384, 21384, 22152, 22152, 22152, 22920, 22920, 22920,
    22920, 23688, 23688, 24496, 24496, 24496, 25456, 25456, 25456, 25456,
    24496, 25456, 25456, 25456, 26416, 26416, 26416, 27376, 27376, 27376,
    26416, 26416, 27376, 27376, 27376, 28336, 28336, 28336, 29296, 29296,
    29296, 29296, 30576, 30576, 30576, 30576, 31704, 31704, 31704, 32856,
    31704, 32856, 32856, 32856, 34008, 34008, 34008, 35160, 35160, 35160,
    35160, 35160, 35160, 36696, 36696, 36696, 37888, 37888, 37888, 39232,
    37888, 37888, 39232, 39232, 39232, 40576, 40576, 40576, 42368, 42368,
    40576, 40576, 42368, 42368, 42368, 43816, 43816, 43816, 45352, 45352,
    43816, 43816, 45352, 45352, 45352, 46888, 46888, 46888, 48936, 48936,
    46888, 46888, 46888, 48936, 48936, 48936, 51024, 51024, 51024, 51024,
    48936, 51024, 51024, 51024, 52752, 52752, 52752, 52752, 55056, 55056,
    51024, 52752, 52752, 52752, 55056, 55056, 55056, 55056, 57336, 57336,
    59256, 59256, 61664, 61664, 61664, 63776, 63776, 63776, 66592, 66592,

    // Nprb: 91 to 100
     2536,  2536,  2600,  2600,  2664,  2664,  2728,  2728,  2728,  2792,
     3368,  3368,  3368,  3496,  3496,  3496,  3496,  3624,  3624,  3624,
     4136,  4136,  4136,  4264,  4264,  4264,  4392,  4392,  4392,  4584,
     5352,  5352,  5352,  5544,  5544,  5544,  5736,  5736,  5736,  5736,
     6456,  6456,  6712,  6712,  6712,  6968,  6968,  6968,  6968,  7224,
     7992,  7992,  8248,  8248,  8248,  8504,  8504,  8760,  8760,  8760,
     9528,  9528,  9528,  9912,  9912,  9912, 10296, 10296, 10296, 10296,
    11064, 11448, 11448, 11448, 11448, 11832, 11832, 11832, 12216, 12216,
    12576, 12960, 12960, 12960, 13536, 13536, 13536, 13536, 14112, 14112,
    14112, 14688, 14688, 14688, 15264, 15264, 15264, 15264, 15840, 15840,
    15840, 16416, 16416, 16416, 16992, 16992, 16992, 16992, 17568, 17568,
    18336, 18336, 19080, 19080, 19080, 19080, 19848, 19848, 19848, 19848,
    20616, 21384, 21384, 21384, 21384, 22152, 22152, 22152, 22920, 22920,
    23688, 23688, 23688, 24496, 24496, 24496, 25456, 25456, 25456, 25456,
    26416, 26416, 26416, 27376, 27376, 27376, 28336, 28336, 28336, 28336,
    28336, 28336, 28336, 29296, 29296, 29296, 29296, 30576, 30576, 30576,
    29296, 30576, 30576, 30576, 30576, 31704, 31704, 31704, 31704, 32856,
    32856, 32856, 34008, 34008, 34008, 35160, 35160, 35160, 35160, 36696,
    36696, 36696, 36696, 37888, 37888, 37888, 37888, 39232, 39232, 39232,
    39232, 39232, 40576, 40576, 40576, 40576, 42368, 42368, 42368, 43816,
    42368, 42368, 43816, 43816, 43816, 45352, 45352, 45352, 46888, 46888,
    45352, 46888, 46888, 46888, 46888, 48936, 48936, 48936, 48936, 51024,
    48936, 48936, 51024, 51024, 51024, 51024, 52752, 52752, 52752, 55056,
    52752, 52752, 52752, 55056, 55056, 55056, 55056, 57336, 57336, 57336,
    55056, 57336, 57336, 57336, 57336, 59256, 59256, 59256, 61664, 61664,
    57336, 59256, 59256, 59256, 61664, 61664, 61664, 61664, 63776, 63776,
    66592, 68808, 68808, 68808, 71112, 71112, 71112, 73712, 73712, 75376,
    
    // Nprb: 101 to 110
     2792,  2856,  2856,  2856,  2984,  2984,  2984,  2984,  2984,  3112,
     3752,  3752,  3752,  3752,  3880,  3880,  3880,  4008,  4008,  4008,
     4584,  4584,  4584,  4584,  4776,  4776,  4776,  4776,  4968,  4968,
     5992,  5992,  5992,  5992,  6200,  6200,  6200,  6200,  6456,  6456,
     7224,  7224,  7480,  7480,  7480,  7480,  7736,  7736,  7736,  7992,
     8760,  9144,  9144,  9144,  9144,  9528,  9528,  9528,  9528,  9528,
    10680, 10680, 10680, 10680, 11064, 11064, 11064, 11448, 11448, 11448,
    12216, 12576, 12576, 12576, 12960, 12960, 12960, 12960, 13536, 13536,
    14112, 14112, 14688, 14688, 14688, 14688, 15264, 15264, 15264, 15264,
    15840, 16416, 16416, 16416, 16416, 16992, 16992, 16992, 16992, 17568,
    17568, 18336, 18336, 18336, 18336, 18336, 19080, 19080, 19080, 19080,
    20616, 20616, 20616, 21384, 21384, 21384, 21384, 22152, 22152, 22152,
    22920, 23688, 23688, 23688, 23688, 24496, 24496, 24496, 24496, 25456,
    26416, 26416, 26416, 26416, 27376, 27376, 27376, 27376, 28336, 28336,
    29296, 29296, 29296, 29296, 30576, 30576, 30576, 30576, 31704, 31704,
    30576, 31704, 31704, 31704, 31704, 32856, 32856, 32856, 34008, 34008,
    32856, 32856, 34008, 34008, 34008, 34008, 35160, 35160, 35160, 35160,
    36696, 36696, 36696, 37888, 37888, 37888, 39232, 39232, 39232, 39232,
    40576, 40576, 40576, 40576, 42368, 42368, 42368, 42368, 43816, 43816,
    43816, 43816, 43816, 45352, 45352, 45352, 46888, 46888, 46888, 46888,
    46888, 46888, 48936, 48936, 48936, 48936, 48936, 51024, 51024, 51024,
    51024, 51024, 51024, 52752, 52752, 52752, 52752, 55056, 55056, 55056,
    55056, 55056, 55056, 57336, 57336, 57336, 57336, 59256, 59256, 59256,
    57336, 59256, 59256, 59256, 59256, 61664, 61664, 61664, 61664, 63776,
    61664, 61664, 63776, 63776, 63776, 63776, 66592, 66592, 66592, 66592,
    63776, 63776, 66592, 66592, 66592, 66592, 68808, 68808, 68808, 71112,
    75376, 75376, 75376, 75376, 75376, 75376, 75376, 75376, 75376, 75376
};
int tb_size_num = (sizeof( tb_size ) / sizeof( int ));

/*
* TS 36.212 Table 5.1.3-3: Turbo code internal interleaver parameters
*/
int cb_size[] = 
{
      40,    48,    56,    64,
      72,    80,    88,    96,
     104,   112,   120,   128,
     136,   144,   152,   160,
     168,   176,   184,   192,
     200,   208,   216,   224,
     232,   240,   248,   256,
     264,   272,   280,   288,
     296,   304,   312,   320,
     328,   336,   344,   352,
     360,   368,   376,   384,
     392,   400,   408,   416,
     424,   432,   440,   448,
     456,   464,   472,   480,
     488,   496,   504,   512,
     528,   544,   560,   576,
     592,   608,   624,   640,
     656,   672,   688,   704,
     720,   736,   752,   768,
     784,   800,   816,   832,
     848,   864,   880,   896,
     912,   928,   944,   960,
     976,   992,  1008,  1024,
    1056,  1088,  1120,  1152,
    1184,  1216,  1248,  1280,
    1312,  1344,  1376,  1408,
    1440,  1472,  1504,  1536,
    1568,  1600,  1632,  1664,
    1696,  1728,  1760,  1792,
    1824,  1856,  1888,  1920,
    1952,  1984,  2016,  2048,
    2112,  2176,  2240,  2304,
    2368,  2432,  2496,  2560,
    2624,  2688,  2752,  2816,
    2880,  2944,  3008,  3072,
    3136,  3200,  3264,  3328,
    3392,  3456,  3520,  3584,
    3648,  3712,  3776,  3840,
    3904,  3968,  4032,  4096,
    4160,  4224,  4288,  4352,
    4416,  4480,  4544,  4608,
    4672,  4736,  4800,  4864,
    4928,  4992,  5056,  5120,
    5184,  5248,  5312,  5376,
    5440,  5504,  5568,  5632,
    5696,  5760,  5824,  5888,
    5952,  6016,  6080,  6144
};
int cb_size_num = (sizeof( cb_size ) / sizeof( int ));


int lookup_K1(int C, int BB)
{
    int K = 0;
    int i;

    for (i=0; i<cb_size_num; i++)
    {
        if ((C * cb_size[i]) >= BB)
        {
            K = cb_size[i];
            break;
        }
    }

    if (0 == K)
    {
        printf("ERR: cannot find (C * K1) >= %d\n\n", BB);
    }

    return K;
}

int lookup_K2(int K1)
{
    int K = 0;
    int i, j;

    for (i=0, j=(cb_size_num-1); i<cb_size_num; i++, j--)
    {
        if (cb_size[j] < K1)
        {
            K = cb_size[j];
            break;
        }
    }

    if (0 == K)
    {
        printf("ERR: cannot find K2 < %d\n\n", K1);
    }

    return K;
}


void help(void)
{
    printf("Usage: cbs [OPTION]...\n");
    printf("\n");
    printf("  -a        Show all TB size.\n");
    printf("  -s size   Assign a TB size in bits.\n");
    printf("  -h        Show the help message.\n");
    printf("\n");
}

/*
 * R10 36.212 Section 5.1.2 Code block segmentation and code block CRC attachment
 */
int main(int argc, char *argv[])
{
    int tb_single = 0;
    int display = 1;
    int ch;

    int A;
    int B, BB;
    int C, C1, C2;
    int K1, K2;
    int L;
    int F;
    int i;


    opterr = 0;
    while ((ch=getopt(argc, argv, "as:h")) != -1)
    {
        switch ( ch )
        {
            case 'a':
                display = 1;
                break;
            case 's':
                display = 0;
                tb_single = 1;
                tb_size_num = 1;
                tb_size[0]  = atoi( optarg );
                break;
            case 'h':
            default:
                help();
                return 0;
        }
    }

    if ( display )
    {
        printf("  NO     TB   C1    K1   C2    K2   F\n");
        printf("--------------------------------------\n");
    }

    for (i=0; i<tb_size_num; i++)
    {
        A = tb_size[i];
        B = (A + 24);

        if ( tb_single )
        {
            printf("A   %d\n", A);
            printf("B   %d\n", B);
        }

        if (B <= Z)
        {
            L  = 0;
            C  = 1;
            BB = B;
        }
        else
        {
            L  = 24;
            C  = CEIL(B, (Z - L));
            BB = (B + (C * L));
        }

        if ( tb_single )
        {
            printf("L   %d\n", L);
            printf("C   %d\n", C);
            printf("BB  %d\n", BB);
        }

        if (0 == C)
        {
            printf("ERR: C is 0 (TB %d)\n\n", A);
            exit(-1);
        }
        else if (1 == C)
        {
            K1 = lookup_K1(C, BB);
            K2 = 0;
            C1 = 1;
            C2 = 0;
        }
        else
        {
            K1 = lookup_K1(C, BB);
            K2 = lookup_K2( K1 );
            C2 = FLOOR(((C * K1) - BB), (K1 - K2));
            C1 = (C - C2);
        }

        F  = ((C1 * K1) + (C2 * K2) - BB);

        if ( tb_single )
        {
            printf("K1  %d\n", K1);
            printf("K2  %d\n", K2);
            printf("C1  %d\n", C1);
            printf("C2  %d\n", C2);
            printf("F   %d\n", F);
            printf("\n");
        }

        if ( display )
        {
            printf(
                "%4d  %5d   %2d  %4d    %d  %4d   %d\n",
                i,
                A,
                C1,
                K1,
                C2,
                K2,
                F
            );
        }

        if ((F != 0 ) || (C2 != 0) || (C1 == 0))
        {
            printf(
                "\033[1;31mTB( %4d ), C2( %d ), K2( %4d ), F( %4d )\033[m\n",
                A,
                C2,
                K2,
                F
            );
        }
    }

    if ( display )
    {
        printf("\n");
    }

    return 0;
}
