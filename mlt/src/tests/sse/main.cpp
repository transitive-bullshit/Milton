/**<!-------------------------------------------------------------------->
   @file   main.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Tests stability and accuracy of SSE linear algebra library
   <!-------------------------------------------------------------------->**/

#include <common/math/algebra.h>
#include <iostream>
using namespace std;
using namespace milton;

void test_alignment();
void test_math();

#define TEST_EQ(a, b)   (EQ((create_real(a)), (create_real(b))))

int main(int argc, char** argv) {
#if MILTON_ENABLE_SEE
   test_alignment();
   test_math();
   
   return 0;
#else
   cerr << "SSE was not enabled in this build!" << endl;
   
   return 1;
#endif
}

struct Test : public SSEAligned {
   Vector3 a;
   Vector3 b;
   Vector3 c;
   Vector3 d;
   Vector3 e;
   Vector3 f;
};

void test_alignment() {
   // test alignment of overridden new / delete
   for(unsigned i = 0; i < 99999; ++i) {
      Vector3 *v = new Vector3(i, i, i);
      
      ASSERT((((unsigned long)v) & 15) == 0);
      safeDelete(v);
   }
   
   { // test alignment (128-bit SSE registers must be aligned on 16-byte boundaries)
      Vector3 a(0, 0, 0);
      Vector3 b(1, 0, 0);
      Vector3 c(0, 1, 0);
      Vector3 d(0, 0, 1);
      Vector3 e(a + b);
      Vector3 f(c + d);
      
      cerr << a << ", " << b << ", " << c << ", " << d << ", " << e << ", " << f << endl;
      
      ASSERT((((unsigned long)&a) & 15) == 0);
      ASSERT((((unsigned long)&b) & 15) == 0);
      ASSERT((((unsigned long)&c) & 15) == 0);
      ASSERT((((unsigned long)&d) & 15) == 0);
      ASSERT((((unsigned long)&e) & 15) == 0);
      ASSERT((((unsigned long)&f) & 15) == 0);
   }
   
   { // test compound alignment
      Test t0;
      Test t1;
      Test t2;
      Test t3;
      Test t4;
   }
   
   // test compound alignment with new / delete
   for(unsigned i = 0; i < 99999; ++i) {
      Test *t = new Test();
      
      ASSERT((((unsigned long)t) & 15) == 0);
      safeDelete(t);
   }
}

void test_math() {
   // constructors
   real_t d0[] = { 0.1, 2.1, -3.9999 };
   
   Vector3 v0(d0);
   for(unsigned i = 3; i--;) ASSERT(TEST_EQ(v0[i], d0[i]));
   
   Vector3 v1;
   for(unsigned i = 3; i--;) ASSERT(TEST_EQ(v1[i], 0));
   ASSERT(v1.isZero());
   
   real_t d2[] = { -77, 0.003, 40 };
   Vector3 v2(d2[0], d2[1], d2[2]);
   for(unsigned i = 3; i--;) ASSERT(TEST_EQ(v2[i], d2[i]));
   
   Vector3 v3(v2);
   for(unsigned i = 3; i--;) ASSERT(TEST_EQ(v3[i], v2[i]));
   ASSERT(v2 == v3);
   
   { // equality / assignment operators
      Vector3 v4 = v2;
      for(unsigned i = 3; i--;) ASSERT(TEST_EQ(v4[i], v2[i]));
      ASSERT(v4 == v2);
      
      ASSERT(v0 != v1);
      v4[2] = 41;
      ASSERT(v4 != v2);
      v4[2] = 40;
      ASSERT(v4 == v2);
   }
   
   { // test static factory constructors
      Vector3 v4 = Vector3::zero();
      for(unsigned i = 3; i--;) ASSERT(TEST_EQ(v4[i], 0));
      ASSERT(v4.isZero());
      
      Vector3 v5 = Vector3::ones();
      for(unsigned i = 3; i--;) ASSERT(TEST_EQ(v5[i], 1));
      
      Vector3 v6 = Vector3::min(v2, v2);
      ASSERT(v6 == v2);
      
      Vector3 v7 = Vector3::min(v4, v5);
      ASSERT(v7 == v4);
      
      Vector3 v8 = Vector3::max(v4, v5);
      ASSERT(v8 == v5);
   }
   
   { // test arithmetic assignment operators
      Vector3 v4 = v0;
      v4 += Vector3::fill(1);
      ASSERT(v4 - Vector3::fill(1) == v0);
      
      v4 -= Vector3::fill(1);
      ASSERT(v4 == v0);
      
      v4 *= -1;
      ASSERT(v4 == -v0);
      
      v4 /= -1;
      ASSERT(v4 == v0);
      
      v4 /= 2;
      v0 *= 0.5;
      
      ASSERT(v4 == v0);
   }
   
   { // test arithmetic operators
      Vector3 v4 = v0 + v2;
      for(unsigned i = 3; i--;) ASSERT(TEST_EQ(v4[i], v0[i] + v2[i]));
      
      Vector3 v5 = v0 - v2;
      for(unsigned i = 3; i--;) ASSERT(TEST_EQ(v5[i], v0[i] - v2[i]));
      
      Vector3 v6 = v0 * v2;
      for(unsigned i = 3; i--;) ASSERT(TEST_EQ(v6[i], v0[i] * v2[i]));
      
      Vector3 v7 = v0 * M_PI;
      for(unsigned i = 3; i--;) ASSERT(TEST_EQ(v7[i], v0[i] * M_PI));
      
      Vector3 v8 = v0 * -9999;
      for(unsigned i = 3; i--;) {
         const real_t d = v0[i] * -9999;
         ASSERT(TEST_EQ(v8[i], d));
      }
   }
   
   { // test misc functionality
      Vector3 v4 = v2;
      
      real_t s = 0;
      for(unsigned i = 3; i--;) s += v4[i] * v4[i];
      s = sqrt(s);
      
      real_t mag2 = v4.getMagnitude2();
      real_t mag  = v4.getMagnitude();
      
      ASSERT(TEST_EQ(sqrt(mag2), mag));
      ASSERT(TEST_EQ(s, mag));
      
      const real_t t0 = v4.normalize();
      ASSERT(v4.isUnit());
      ASSERT(TEST_EQ(t0, mag));
      
      const real_t t1 = v4.getMagnitude();
      ASSERT(TEST_EQ(t1, 1));
      
      Vector3 v3 = v0.getNormalized();
      ASSERT(v3.isUnit());
   }
}

