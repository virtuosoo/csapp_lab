/* 
 * CS:APP Data Lab 
 * 
 * <Please put your name and userid here>
 * 
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting if the shift amount
     is less than 0 or greater than 31.


EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implement floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants. You can use any arithmetic,
logical, or comparison operations on int or unsigned data.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operations (integer, logical,
     or comparison) that you are allowed to use for your implementation
     of the function.  The max operator count is checked by dlc.
     Note that assignment ('=') is not counted; you may use as many of
     these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */


#endif
//1
/* 
 * bitXor - x^y using only ~ and & 
 *   Example: bitXor(4, 5) = 1
 *   Legal ops: ~ &
 *   Max ops: 14
 *   Rating: 1
 */
int bitXor(int x, int y) {
  int var1 = (~x) & y;  //var1的某位为1，当且仅当对应x的位为0，y的位为1
  int var2 = x & (~y);
  return ~((~var1) & (~var2)); //相当于一次或运算
}
/* 
 * tmin - return minimum two's complement integer 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 
 *   Rating: 1
 */
int tmin(void) {
  int res = 0x80 << 24;
  return res;
}
//2
/*
 * isTmax - returns 1 if x is the maximum, two's complement number,
 *     and 0 otherwise 
 *   Legal ops: ! ~ & ^ | +
 *   Max ops: 10
 *   Rating: 1
 */
int isTmax(int x) {
  int all1 = (~1) ^ (1); //其实就是-1
  int var1 = x ^ all1;  //这个解法的原理是：在补码范围内， x + 1 == x ^ (-1)只有两个解
  return !((x + 1) ^ var1) & !!(x ^ all1); //即Tmax和-1，按这个条件，排除-1即可。注意 x == y可以写成!(x ^ y)
}
/* 
 * allOddBits - return 1 if all odd-numbered bits in word set to 1
 *   where bits are numbered from 0 (least significant) to 31 (most significant)
 *   Examples allOddBits(0xFFFFFFFD) = 0, allOddBits(0xAAAAAAAA) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 */
int allOddBits(int x) {
  int allA = (((((0xAA << 8) + 0xAA) << 8) + 0xAA) << 8) + 0xAA; //0xAAAAAAAA
  return !((x & allA) ^ allA);
}
/* 
 * negate - return -x 
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x) {
  return (~x) + 1; //补码的性质
}
//3
/* 
 * isAsciiDigit - return 1 if 0x30 <= x <= 0x39 (ASCII codes for characters '0' to '9')
 *   Example: isAsciiDigit(0x35) = 1.
 *            isAsciiDigit(0x3a) = 0.
 *            isAsciiDigit(0x05) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 3
 */
int isAsciiDigit(int x) {
  int ret = 1, lowx = x & 0xF;;
  ret = ret & !((x >> 4) ^ 0x3); //x右移4位后，必须等于0x3
  ret = ret & !!((lowx & 0xC) ^ 0xC); //x的后4位，不能形如11xx，这样排除了4个
  ret = ret & !!((lowx & 0xA) ^ 0xA); //x的后4位，也不能形如101x，排除2个
  return ret;
}
/* 
 * conditional - same as x ? y : z 
 *   Example: conditional(2,4,5) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */
int conditional(int x, int y, int z) {
  int neg1 = ~1 + 1; //-1
  int c = !x + neg1; //当x=0时，c = 0；当x != 0时， c = 0xffffffff，即-1
  return (z & (~c)) + (y & c);
}
/* 
 * isLessOrEqual - if x <= y  then return 1, else return 0 
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLessOrEqual(int x, int y) {
  int negy = (~y) + 1, Tmin = 1 << 31;
  int isxNeg = !!(x & Tmin), isyPos = !(y & Tmin);
  int sum = x + negy;   
  return (isxNeg & isyPos) | (!(x ^ y)) | //x为负，但y>=0，或者x == y
  (!!(sum & Tmin) & !(!isxNeg & !isyPos)); //或者x - y < 0，但要排除x为正，y为负的情况，此时可能溢出，导致判断错误
}
//4
/* 
 * logicalNeg - implement the ! operator, using all of 
 *              the legal operators except !
 *   Examples: logicalNeg(3) = 0, logicalNeg(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4 
 */

// 注意到0x00..0 + 0x01 = true; oxff..f + 0x01 = false
// 所以当x为0时，需要构造出全0，当x不为0时，需要构造出全1
// 注意到((~x) + 1) = -x,则x不为0时，(x | ((~x) + 1))一定是负的
// 此时将其算术左移31位，得到了全1
int logicalNeg(int x) {
  return ((x | ((~x) + 1)) >> 31) + 0x01;
}
/* howManyBits - return the minimum number of bits required to represent x in
 *             two's complement
 *  Examples: howManyBits(12) = 5
 *            howManyBits(298) = 10
 *            howManyBits(-5) = 4
 *            howManyBits(0)  = 1
 *            howManyBits(-1) = 1
 *            howManyBits(0x80000000) = 32
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 90
 *  Rating: 4
 */

// 对于一个正数，想找到多少位能表示，只需找到它最高位的1所在的位置，然后再+1即可，
// 因为前面需要一个0，表示正负。负数同理，只需找到最高位的0，
// 为了方便，当x为负数时，将x按位取反。
int howManyBits(int x) {
  int b16, b8, b4, b2, b1, b0;
  int sign = x >> 31;
  x = (sign & (~x)) | ((~sign) & x); //如果x为正数，则不变，否则x按位取反

  b16 = !!(x >> 16) << 4; //如果x的高16位有1，则b16值为16，否则值为0，下面的同理
  x = x >> b16;
  b8 = !!(x >> 8) << 3;
  x = x >> b8;
  b4 = !!(x >> 4) << 2;
  x = x >> b4;
  b2 = !!(x >> 2) << 1;
  x = x >> b2;
  b1 = !!(x >> 1);
  x = x >> b1;
  b0 = x;

  return b16 + b8 + b4 + b2 + b1 + b0 + 1;
}
//float
/* 
 * floatScale2 - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned floatScale2(unsigned uf) {
  unsigned s = uf >> 31;
  unsigned exp = (uf >> 23) & 0xff;
  unsigned frac = (uf << 9) >> 9;
  if (exp == 0xff) { //inf or NaN
    return uf;
  }

  if (exp == 0) { //denorm 特殊处理
    if (frac == 0) {
      return s << 31;
    }
    if (((frac >> 22) & 1) == 0) { //直接将frac部分*2即可
      return (s << 31) + (exp << 23) + (frac << 1);
    } else { //需要从denorm的形式过度到norm形式
      unsigned newFrac = frac << 1;
      newFrac = newFrac << 9 >> 9; //只保留23位
      return (s << 31) + (0x01 << 23) + (newFrac);
    }
  }

  return (s << 31) + ((exp + 1) << 23) + frac; //普通情况，将指数+1即可
}
/* 
 * floatFloat2Int - Return bit-level equivalent of expression (int) f
 *   for floating point argument f.
 *   Argument is passed as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point value.
 *   Anything out of range (including NaN and infinity) should return
 *   0x80000000u.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
int floatFloat2Int(unsigned uf) {
  int Tmin = 1 << 31;
  int s = uf >> 31;
  int exp = (uf >> 23) & 0xff;
  int frac = (uf << 9) >> 9;

  if (exp == 0xff) { //inf or NaN
    return Tmin;
  }

  if (exp < 127) { //太小，就是0
    //printf("return 0 because too small\n");
    return 0;
  }

  int E = exp - 127;
  if (E > 31) {
    return Tmin;
  }

  int sign = (s == 1) ? -1 : 1;
  int res = frac | (1 << 23); //把隐含的1填进去
  //printf("res %x\n", res);
  if (E < 24) {
    res = res >> (23 - E);
  } else {
    res = res << (E - 23);
  }
  //printf("normal return\n");
  return res * sign;
}
/* 
 * floatPower2 - Return bit-level equivalent of the expression 2.0^x
 *   (2.0 raised to the power x) for any 32-bit integer x.
 *
 *   The unsigned value that is returned should have the identical bit
 *   representation as the single-precision floating-point number 2.0^x.
 *   If the result is too small to be represented as a denorm, return
 *   0. If too large, return +INF.
 * 
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. Also if, while 
 *   Max ops: 30 
 *   Rating: 4
 */
unsigned floatPower2(int x) {
  unsigned INF = 0xff << 23;
  if (x < -149) { //too small
    return 0;
  }
  if (x > 127) {
    return INF;
  }
  if (x <= -127 && x >= -149) { //denorm，测试用例里面好像没有这种情况？
    return 1 << (x + 149); //当x位-127时，左移22位，当x为-149时，左移0位。
  }
  return (x + 127) << 23;
}
