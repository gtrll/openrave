#define IKFAST_NAMESPACE ik_pr2_head
#include "plugindefs.h"

/// autogenerated analytical inverse kinematics code from ikfast program part of OpenRAVE
/// \author Rosen Diankov
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///     http://www.apache.org/licenses/LICENSE-2.0
/// 
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// ikfast version 61 generated on 2012-10-08 02:44:05.655596
/// To compile with gcc:
///     gcc -lstdc++ ik.cpp
/// To compile without any main function as a shared object (might need -llapack):
///     gcc -fPIC -lstdc++ -DIKFAST_NO_MAIN -DIKFAST_CLIBRARY -shared -Wl,-soname,libik.so -o libik.so ik.cpp
#define IKFAST_HAS_LIBRARY
#include "ikfast.h" // found inside share/openrave-X.Y/python/ikfast.h
using namespace ikfast;

// check if the included ikfast version matches what this file was compiled with
#define IKFAST_COMPILE_ASSERT(x) extern int __dummy[(int)x]
IKFAST_COMPILE_ASSERT(IKFAST_VERSION==61);

#include <cmath>
#include <vector>
#include <limits>
#include <algorithm>
#include <complex>

#define IKFAST_STRINGIZE2(s) #s
#define IKFAST_STRINGIZE(s) IKFAST_STRINGIZE2(s)

#ifndef IKFAST_ASSERT
#include <stdexcept>
#include <sstream>
#include <iostream>

#ifdef _MSC_VER
#ifndef __PRETTY_FUNCTION__
#define __PRETTY_FUNCTION__ __FUNCDNAME__
#endif
#endif

#ifndef __PRETTY_FUNCTION__
#define __PRETTY_FUNCTION__ __func__
#endif

#define IKFAST_ASSERT(b) { if( !(b) ) { std::stringstream ss; ss << "ikfast exception: " << __FILE__ << ":" << __LINE__ << ": " <<__PRETTY_FUNCTION__ << ": Assertion '" << #b << "' failed"; throw std::runtime_error(ss.str()); } }

#endif

#if defined(_MSC_VER)
#define IKFAST_ALIGNED16(x) __declspec(align(16)) x
#else
#define IKFAST_ALIGNED16(x) x __attribute((aligned(16)))
#endif

#define IK2PI  ((IkReal)6.28318530717959)
#define IKPI  ((IkReal)3.14159265358979)
#define IKPI_2  ((IkReal)1.57079632679490)

#ifdef _MSC_VER
#ifndef isnan
#define isnan _isnan
#endif
#endif // _MSC_VER

// lapack routines
extern "C" {
  void dgetrf_ (const int* m, const int* n, double* a, const int* lda, int* ipiv, int* info);
  void zgetrf_ (const int* m, const int* n, std::complex<double>* a, const int* lda, int* ipiv, int* info);
  void dgetri_(const int* n, const double* a, const int* lda, int* ipiv, double* work, const int* lwork, int* info);
  void dgesv_ (const int* n, const int* nrhs, double* a, const int* lda, int* ipiv, double* b, const int* ldb, int* info);
  void dgetrs_(const char *trans, const int *n, const int *nrhs, double *a, const int *lda, int *ipiv, double *b, const int *ldb, int *info);
  void dgeev_(const char *jobvl, const char *jobvr, const int *n, double *a, const int *lda, double *wr, double *wi,double *vl, const int *ldvl, double *vr, const int *ldvr, double *work, const int *lwork, int *info);
}

using namespace std; // necessary to get std math routines

#ifdef IKFAST_NAMESPACE
namespace IKFAST_NAMESPACE {
#endif

inline float IKabs(float f) { return fabsf(f); }
inline double IKabs(double f) { return fabs(f); }

inline float IKsqr(float f) { return f*f; }
inline double IKsqr(double f) { return f*f; }

inline float IKlog(float f) { return logf(f); }
inline double IKlog(double f) { return log(f); }

// allows asin and acos to exceed 1
#ifndef IKFAST_SINCOS_THRESH
#define IKFAST_SINCOS_THRESH ((IkReal)0.000001)
#endif

// used to check input to atan2 for degenerate cases
#ifndef IKFAST_ATAN2_MAGTHRESH
#define IKFAST_ATAN2_MAGTHRESH ((IkReal)2e-6)
#endif

// minimum distance of separate solutions
#ifndef IKFAST_SOLUTION_THRESH
#define IKFAST_SOLUTION_THRESH ((IkReal)1e-6)
#endif

inline float IKasin(float f)
{
IKFAST_ASSERT( f > -1-IKFAST_SINCOS_THRESH && f < 1+IKFAST_SINCOS_THRESH ); // any more error implies something is wrong with the solver
if( f <= -1 ) return float(-IKPI_2);
else if( f >= 1 ) return float(IKPI_2);
return asinf(f);
}
inline double IKasin(double f)
{
IKFAST_ASSERT( f > -1-IKFAST_SINCOS_THRESH && f < 1+IKFAST_SINCOS_THRESH ); // any more error implies something is wrong with the solver
if( f <= -1 ) return -IKPI_2;
else if( f >= 1 ) return IKPI_2;
return asin(f);
}

// return positive value in [0,y)
inline float IKfmod(float x, float y)
{
    while(x < 0) {
        x += y;
    }
    return fmodf(x,y);
}

// return positive value in [0,y)
inline double IKfmod(double x, double y)
{
    while(x < 0) {
        x += y;
    }
    return fmod(x,y);
}

inline float IKacos(float f)
{
IKFAST_ASSERT( f > -1-IKFAST_SINCOS_THRESH && f < 1+IKFAST_SINCOS_THRESH ); // any more error implies something is wrong with the solver
if( f <= -1 ) return float(IKPI);
else if( f >= 1 ) return float(0);
return acosf(f);
}
inline double IKacos(double f)
{
IKFAST_ASSERT( f > -1-IKFAST_SINCOS_THRESH && f < 1+IKFAST_SINCOS_THRESH ); // any more error implies something is wrong with the solver
if( f <= -1 ) return IKPI;
else if( f >= 1 ) return 0;
return acos(f);
}
inline float IKsin(float f) { return sinf(f); }
inline double IKsin(double f) { return sin(f); }
inline float IKcos(float f) { return cosf(f); }
inline double IKcos(double f) { return cos(f); }
inline float IKtan(float f) { return tanf(f); }
inline double IKtan(double f) { return tan(f); }
inline float IKsqrt(float f) { if( f <= 0.0f ) return 0.0f; return sqrtf(f); }
inline double IKsqrt(double f) { if( f <= 0.0 ) return 0.0; return sqrt(f); }
inline float IKatan2(float fy, float fx) {
    if( isnan(fy) ) {
        IKFAST_ASSERT(!isnan(fx)); // if both are nan, probably wrong value will be returned
        return float(IKPI_2);
    }
    else if( isnan(fx) ) {
        return 0;
    }
    return atan2f(fy,fx);
}
inline double IKatan2(double fy, double fx) {
    if( isnan(fy) ) {
        IKFAST_ASSERT(!isnan(fx)); // if both are nan, probably wrong value will be returned
        return IKPI_2;
    }
    else if( isnan(fx) ) {
        return 0;
    }
    return atan2(fy,fx);
}

inline float IKsign(float f) {
    if( f > 0 ) {
        return float(1);
    }
    else if( f < 0 ) {
        return float(-1);
    }
    return 0;
}

inline double IKsign(double f) {
    if( f > 0 ) {
        return 1.0;
    }
    else if( f < 0 ) {
        return -1.0;
    }
    return 0;
}

/// solves the forward kinematics equations.
/// \param pfree is an array specifying the free joints of the chain.
IKFAST_API void ComputeFk(const IkReal* j, IkReal* eetrans, IkReal* eerot) {
IkReal x0,x1,x2,x3,x4,x5,x6;
x0=IKcos(j[0]);
x1=IKcos(j[1]);
x2=IKsin(j[0]);
x3=IKsin(j[1]);
x4=((IkReal(0.0980000000000000))*(x3));
x5=((x1)*(x2));
x6=((x0)*(x1));
eetrans[0]=((IkReal(-0.0170700000000000))+(((x0)*(x4)))+(((IkReal(0.0680000000000000))*(x0)))+(((IkReal(-0.0300000000000000))*(x2)))+(((IkReal(0.0232000000000000))*(x6))));
eetrans[1]=((((IkReal(0.0680000000000000))*(x2)))+(((IkReal(0.0300000000000000))*(x0)))+(((x2)*(x4)))+(((IkReal(0.0232000000000000))*(x5))));
eetrans[2]=((IkReal(0.381450000000000))+(((IkReal(0.0980000000000000))*(x1)))+(((IkReal(-0.0232000000000000))*(x3))));
eerot[0]=x6;
eerot[1]=x5;
eerot[2]=((IkReal(-1.00000000000000))*(x3));
}

IKFAST_API int GetNumFreeParameters() { return 0; }
IKFAST_API int* GetFreeParameters() { return NULL; }
IKFAST_API int GetNumJoints() { return 2; }

IKFAST_API int GetIkRealSize() { return sizeof(IkReal); }

IKFAST_API int GetIkType() { return 0x23000006; }

class IKSolver {
public:
IkReal j13,cj13,sj13,htj13,j14,cj14,sj14,htj14,new_px,px,npx,new_py,py,npy,new_pz,pz,npz,pp;
unsigned char _ij13[2], _nj13,_ij14[2], _nj14;

bool ComputeIk(const IkReal* eetrans, const IkReal* eerot, const IkReal* pfree, IkSolutionListBase<IkReal>& solutions) {
j13=numeric_limits<IkReal>::quiet_NaN(); _ij13[0] = -1; _ij13[1] = -1; _nj13 = -1; j14=numeric_limits<IkReal>::quiet_NaN(); _ij14[0] = -1; _ij14[1] = -1; _nj14 = -1; 
for(int dummyiter = 0; dummyiter < 1; ++dummyiter) {
    solutions.Clear();
px = eetrans[0]; py = eetrans[1]; pz = eetrans[2];

new_px=((IkReal(0.0170700000000000))+(px));
new_py=py;
new_pz=((IkReal(-0.381450000000000))+(pz));
px = new_px; py = new_py; pz = new_pz;
pp=(((px)*(px))+((py)*(py))+((pz)*(pz)));
{
IkReal dummyeval[1];
dummyeval[0]=((IkReal(-1.00000000000000))+(((IkReal(-33.3333333333333))*(py))));
if( IKabs(dummyeval[0]) < 0.0000010000000000  )
{
continue;

} else
{
{
IkReal j13array[2], cj13array[2], sj13array[2];
bool j13valid[2]={false};
_nj13 = 2;
IkReal x7=((IKabs(((IkReal(-0.0600000000000000))+(((IkReal(-2.00000000000000))*(py))))) != 0)?((IkReal)1/(((IkReal(-0.0600000000000000))+(((IkReal(-2.00000000000000))*(py)))))):(IkReal)1.0e30);
IkReal x8=((IkReal(2.00000000000000))*(px)*(x7));
if( (((IkReal(-0.00360000000000000))+(((IkReal(4.00000000000000))*((py)*(py))))+(((IkReal(4.00000000000000))*((px)*(px)))))) < (IkReal)-0.00001 )
    continue;
IkReal x9=((x7)*(IKsqrt(((IkReal(-0.00360000000000000))+(((IkReal(4.00000000000000))*((py)*(py))))+(((IkReal(4.00000000000000))*((px)*(px))))))));
j13array[0]=((IkReal(-2.00000000000000))*(atan(((x9)+(((IkReal(-1.00000000000000))*(x8)))))));
sj13array[0]=IKsin(j13array[0]);
cj13array[0]=IKcos(j13array[0]);
j13array[1]=((IkReal(2.00000000000000))*(atan(((x9)+(x8)))));
sj13array[1]=IKsin(j13array[1]);
cj13array[1]=IKcos(j13array[1]);
if( j13array[0] > IKPI )
{
    j13array[0]-=IK2PI;
}
else if( j13array[0] < -IKPI )
{    j13array[0]+=IK2PI;
}
j13valid[0] = true;
if( j13array[1] > IKPI )
{
    j13array[1]-=IK2PI;
}
else if( j13array[1] < -IKPI )
{    j13array[1]+=IK2PI;
}
j13valid[1] = true;
for(int ij13 = 0; ij13 < 2; ++ij13)
{
if( !j13valid[ij13] )
{
    continue;
}
_ij13[0] = ij13; _ij13[1] = -1;
for(int iij13 = ij13+1; iij13 < 2; ++iij13)
{
if( j13valid[iij13] && IKabs(cj13array[ij13]-cj13array[iij13]) < IKFAST_SOLUTION_THRESH && IKabs(sj13array[ij13]-sj13array[iij13]) < IKFAST_SOLUTION_THRESH )
{
    j13valid[iij13]=false; _ij13[1] = iij13; break; 
}
}
j13 = j13array[ij13]; cj13 = cj13array[ij13]; sj13 = sj13array[ij13];

{
IkReal dummyeval[1];
IkReal x10=(sj13)*(sj13);
dummyeval[0]=((((IkReal(4.53333333333333))*(cj13)*(sj13)))+(((IkReal(1111.11111111111))*(x10)*((pz)*(pz))))+(((IkReal(-151.111111111111))*(py)*(sj13)))+((cj13)*(cj13))+(((IkReal(-66.6666666666667))*(cj13)*(py)))+(((IkReal(5.13777777777778))*(x10)))+(((IkReal(1111.11111111111))*((py)*(py)))));
if( IKabs(dummyeval[0]) < 0.0000010000000000  )
{
{
IkReal dummyeval[1];
IkReal x11=(cj13)*(cj13);
IkReal x12=(px)*(px);
IkReal x13=((IkReal(1111.11111111111))*(x12));
dummyeval[0]=((IkReal(-1.00000000000000))+(((IkReal(1111.11111111111))*(x11)*((py)*(py))))+(x13)+(((IkReal(-1.00000000000000))*(x11)*(x13)))+(((IkReal(-2222.22222222222))*(cj13)*(px)*(py)*(sj13))));
if( IKabs(dummyeval[0]) < 0.0000010000000000  )
{
continue;

} else
{
{
IkReal j14array[4], cj14array[4], sj14array[4];
bool j14valid[4]={false};
_nj14 = 4;
IkReal x14=(cj13)*(cj13);
IkReal x15=(py)*(py);
IkReal x16=(px)*(px);
IkReal x17=((x14)*(x15));
IkReal x18=((IkReal(2.00000000000000))*(cj13)*(px)*(py)*(sj13));
IkReal x19=((IkReal(1.00000000000000))*(x14)*(x16));
IkReal x20=((IKabs(((IkReal(-0.000900000000000000))+(((IkReal(-1.00000000000000))*(x18)))+(((IkReal(-1.00000000000000))*(x19)))+(x17)+(x16))) != 0)?((IkReal)1/(((IkReal(-0.000900000000000000))+(((IkReal(-1.00000000000000))*(x18)))+(((IkReal(-1.00000000000000))*(x19)))+(x17)+(x16)))):(IkReal)1.0e30);
if( (((((IkReal(-1.00000000000000))*(x19)*(((IKabs(((IkReal(-0.000900000000000000))+(((IkReal(-1.00000000000000))*(x18)))+(((IkReal(-1.00000000000000))*(x14)*(x16)))+(x17)+(x16))) != 0)?((IkReal)1/(((IkReal(-0.000900000000000000))+(((IkReal(-1.00000000000000))*(x18)))+(((IkReal(-1.00000000000000))*(x14)*(x16)))+(x17)+(x16)))):(IkReal)1.0e30))))+(((x16)*(x20)))+(((x17)*(((IKabs(((IkReal(-0.000900000000000000))+(((IkReal(-1.00000000000000))*(x18)))+(((IkReal(-1.00000000000000))*(x19)))+(((x14)*(x15)))+(x16))) != 0)?((IkReal)1/(((IkReal(-0.000900000000000000))+(((IkReal(-1.00000000000000))*(x18)))+(((IkReal(-1.00000000000000))*(x19)))+(((x14)*(x15)))+(x16)))):(IkReal)1.0e30))))+(((IkReal(-1.00000000000000))*(x18)*(((IKabs(((IkReal(-0.000900000000000000))+(((IkReal(-1.00000000000000))*(x19)))+(((IkReal(-2.00000000000000))*(cj13)*(px)*(py)*(sj13)))+(x17)+(x16))) != 0)?((IkReal)1/(((IkReal(-0.000900000000000000))+(((IkReal(-1.00000000000000))*(x19)))+(((IkReal(-2.00000000000000))*(cj13)*(px)*(py)*(sj13)))+(x17)+(x16)))):(IkReal)1.0e30))))+(((IkReal(-0.000900000000000000))*(x20))))) < (IkReal)-0.00001 )
    continue;
IkReal x21=IKsqrt(((((IkReal(-1.00000000000000))*(x19)*(((IKabs(((IkReal(-0.000900000000000000))+(((IkReal(-1.00000000000000))*(x18)))+(((IkReal(-1.00000000000000))*(x14)*(x16)))+(x17)+(x16))) != 0)?((IkReal)1/(((IkReal(-0.000900000000000000))+(((IkReal(-1.00000000000000))*(x18)))+(((IkReal(-1.00000000000000))*(x14)*(x16)))+(x17)+(x16)))):(IkReal)1.0e30))))+(((x16)*(x20)))+(((x17)*(((IKabs(((IkReal(-0.000900000000000000))+(((IkReal(-1.00000000000000))*(x18)))+(((IkReal(-1.00000000000000))*(x19)))+(((x14)*(x15)))+(x16))) != 0)?((IkReal)1/(((IkReal(-0.000900000000000000))+(((IkReal(-1.00000000000000))*(x18)))+(((IkReal(-1.00000000000000))*(x19)))+(((x14)*(x15)))+(x16)))):(IkReal)1.0e30))))+(((IkReal(-1.00000000000000))*(x18)*(((IKabs(((IkReal(-0.000900000000000000))+(((IkReal(-1.00000000000000))*(x19)))+(((IkReal(-2.00000000000000))*(cj13)*(px)*(py)*(sj13)))+(x17)+(x16))) != 0)?((IkReal)1/(((IkReal(-0.000900000000000000))+(((IkReal(-1.00000000000000))*(x19)))+(((IkReal(-2.00000000000000))*(cj13)*(px)*(py)*(sj13)))+(x17)+(x16)))):(IkReal)1.0e30))))+(((IkReal(-0.000900000000000000))*(x20)))));
cj14array[0]=x21;
cj14array[2]=((IkReal(-1.00000000000000))*(x21));
if( cj14array[0] >= -1-IKFAST_SINCOS_THRESH && cj14array[0] <= 1+IKFAST_SINCOS_THRESH )
{
    j14valid[0] = j14valid[1] = true;
    j14array[0] = IKacos(cj14array[0]);
    sj14array[0] = IKsin(j14array[0]);
    cj14array[1] = cj14array[0];
    j14array[1] = -j14array[0];
    sj14array[1] = -sj14array[0];
}
else if( isnan(cj14array[0]) )
{
    // probably any value will work
    j14valid[0] = true;
    cj14array[0] = 1; sj14array[0] = 0; j14array[0] = 0;
}
if( cj14array[2] >= -1-IKFAST_SINCOS_THRESH && cj14array[2] <= 1+IKFAST_SINCOS_THRESH )
{
    j14valid[2] = j14valid[3] = true;
    j14array[2] = IKacos(cj14array[2]);
    sj14array[2] = IKsin(j14array[2]);
    cj14array[3] = cj14array[2];
    j14array[3] = -j14array[2];
    sj14array[3] = -sj14array[2];
}
else if( isnan(cj14array[2]) )
{
    // probably any value will work
    j14valid[2] = true;
    cj14array[2] = 1; sj14array[2] = 0; j14array[2] = 0;
}
for(int ij14 = 0; ij14 < 4; ++ij14)
{
if( !j14valid[ij14] )
{
    continue;
}
_ij14[0] = ij14; _ij14[1] = -1;
for(int iij14 = ij14+1; iij14 < 4; ++iij14)
{
if( j14valid[iij14] && IKabs(cj14array[ij14]-cj14array[iij14]) < IKFAST_SOLUTION_THRESH && IKabs(sj14array[ij14]-sj14array[iij14]) < IKFAST_SOLUTION_THRESH )
{
    j14valid[iij14]=false; _ij14[1] = iij14; break; 
}
}
j14 = j14array[ij14]; cj14 = cj14array[ij14]; sj14 = sj14array[ij14];
{
IkReal evalcond[3];
IkReal x22=IKsin(j14);
IkReal x23=IKcos(j14);
IkReal x24=(cj13)*(cj13);
IkReal x25=(pz)*(pz);
IkReal x26=(py)*(py);
IkReal x27=((cj13)*(sj13));
IkReal x28=(x23)*(x23);
IkReal x29=(x22)*(x22)*(x22);
IkReal x30=((py)*(x22));
IkReal x31=((IkReal(0.0133280000000000))*(x24));
IkReal x32=((IkReal(0.0300000000000000))*(x22));
IkReal x33=((sj13)*(x22));
IkReal x34=((pz)*(sj13)*(x23));
IkReal x35=((x25)*(x28));
IkReal x36=((x22)*(x28));
evalcond[0]=((((IkReal(-1.00000000000000))*(x32)))+(((cj13)*(x30)))+(((IkReal(-1.00000000000000))*(px)*(x33))));
evalcond[1]=((((IkReal(-0.0980000000000000))*(sj13)))+(x30)+(x34)+(((IkReal(-1.00000000000000))*(cj13)*(x32)))+(((IkReal(-0.0680000000000000))*(x33))));
evalcond[2]=((IkReal(0.0142280000000000))+(((IkReal(-1.00000000000000))*(x35)))+(((IkReal(-1.00000000000000))*(x29)*(x31)))+(((IkReal(0.00588000000000000))*(x27)*(x36)))+(((IkReal(0.0133280000000000))*(x36)))+(((IkReal(-1.00000000000000))*(x26)))+(((IkReal(0.0133280000000000))*(x29)))+(((IkReal(-1.00000000000000))*(x31)))+(((IkReal(-1.00000000000000))*(x31)*(x36)))+(((IkReal(-2.00000000000000))*(x30)*(x34)))+(((IkReal(-0.00462400000000000))*(x28)))+(((IkReal(0.00408000000000000))*(x27)))+(((x24)*(x35)))+(((x26)*(x28)))+(((IkReal(0.00588000000000000))*(x27)*(x29)))+(((IkReal(0.00372400000000000))*(x24)*(x28)))+(((IkReal(-0.00408000000000000))*(x27)*(x28))));
if( IKabs(evalcond[0]) > 0.000001  || IKabs(evalcond[1]) > 0.000001  || IKabs(evalcond[2]) > 0.000001  )
{
continue;
}
}

IkReal soleval[1];
soleval[0]=((IkReal(-0.0232000000000000))+(((cj14)*(((IkReal(-0.0680000000000000))+(((cj13)*(px)))+(((py)*(sj13)))))))+(((IkReal(-1.00000000000000))*(pz)*(sj14))));
if( soleval[0] > 0.0000000000000000  )
{
std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(2);
vinfos[0].jointtype = 1;
vinfos[0].foffset = j13;
vinfos[0].indices[0] = _ij13[0];
vinfos[0].indices[1] = _ij13[1];
vinfos[0].maxsolutions = _nj13;
vinfos[1].jointtype = 1;
vinfos[1].foffset = j14;
vinfos[1].indices[0] = _ij14[0];
vinfos[1].indices[1] = _ij14[1];
vinfos[1].maxsolutions = _nj14;
std::vector<int> vfree(0);
solutions.AddSolution(vinfos,vfree);
}
}
}

}

}

} else
{
{
IkReal j14array[2], cj14array[2], sj14array[2];
bool j14valid[2]={false};
_nj14 = 2;
IkReal x37=((((IkReal(-0.0680000000000000))*(sj13)))+(py)+(((IkReal(-0.0300000000000000))*(cj13))));
if( IKabs(((pz)*(sj13))) < IKFAST_ATAN2_MAGTHRESH && IKabs(x37) < IKFAST_ATAN2_MAGTHRESH )
    continue;
IkReal x38=((IkReal(1.00000000000000))*(IKatan2(((pz)*(sj13)), x37)));
if( ((((((pz)*(pz))*((sj13)*(sj13))))+((x37)*(x37)))) < (IkReal)-0.00001 )
    continue;
if( (((IkReal(0.0980000000000000))*(sj13)*(((IKabs(IKabs(IKsqrt((((((pz)*(pz))*((sj13)*(sj13))))+((x37)*(x37)))))) != 0)?((IkReal)1/(IKabs(IKsqrt((((((pz)*(pz))*((sj13)*(sj13))))+((x37)*(x37))))))):(IkReal)1.0e30)))) < -1-IKFAST_SINCOS_THRESH || (((IkReal(0.0980000000000000))*(sj13)*(((IKabs(IKabs(IKsqrt((((((pz)*(pz))*((sj13)*(sj13))))+((x37)*(x37)))))) != 0)?((IkReal)1/(IKabs(IKsqrt((((((pz)*(pz))*((sj13)*(sj13))))+((x37)*(x37))))))):(IkReal)1.0e30)))) > 1+IKFAST_SINCOS_THRESH )
    continue;
IkReal x39=IKasin(((IkReal(0.0980000000000000))*(sj13)*(((IKabs(IKabs(IKsqrt((((((pz)*(pz))*((sj13)*(sj13))))+((x37)*(x37)))))) != 0)?((IkReal)1/(IKabs(IKsqrt((((((pz)*(pz))*((sj13)*(sj13))))+((x37)*(x37))))))):(IkReal)1.0e30))));
j14array[0]=((((IkReal(-1.00000000000000))*(x38)))+(x39));
sj14array[0]=IKsin(j14array[0]);
cj14array[0]=IKcos(j14array[0]);
j14array[1]=((IkReal(3.14159265358979))+(((IkReal(-1.00000000000000))*(x39)))+(((IkReal(-1.00000000000000))*(x38))));
sj14array[1]=IKsin(j14array[1]);
cj14array[1]=IKcos(j14array[1]);
if( j14array[0] > IKPI )
{
    j14array[0]-=IK2PI;
}
else if( j14array[0] < -IKPI )
{    j14array[0]+=IK2PI;
}
j14valid[0] = true;
if( j14array[1] > IKPI )
{
    j14array[1]-=IK2PI;
}
else if( j14array[1] < -IKPI )
{    j14array[1]+=IK2PI;
}
j14valid[1] = true;
for(int ij14 = 0; ij14 < 2; ++ij14)
{
if( !j14valid[ij14] )
{
    continue;
}
_ij14[0] = ij14; _ij14[1] = -1;
for(int iij14 = ij14+1; iij14 < 2; ++iij14)
{
if( j14valid[iij14] && IKabs(cj14array[ij14]-cj14array[iij14]) < IKFAST_SOLUTION_THRESH && IKabs(sj14array[ij14]-sj14array[iij14]) < IKFAST_SOLUTION_THRESH )
{
    j14valid[iij14]=false; _ij14[1] = iij14; break; 
}
}
j14 = j14array[ij14]; cj14 = cj14array[ij14]; sj14 = sj14array[ij14];
{
IkReal evalcond[3];
IkReal x40=IKsin(j14);
IkReal x41=(px)*(px);
IkReal x42=(cj13)*(cj13);
IkReal x43=(py)*(py);
IkReal x44=(pz)*(pz);
IkReal x45=IKcos(j14);
IkReal x46=((cj13)*(py));
IkReal x47=((IkReal(2.00000000000000))*(sj13));
IkReal x48=((IkReal(0.00408000000000000))*(sj13));
IkReal x49=(x45)*(x45);
IkReal x50=(x40)*(x40)*(x40);
IkReal x51=((IkReal(1.00000000000000))*(x41));
IkReal x52=((IkReal(0.0133280000000000))*(x40));
IkReal x53=((sj13)*(x40));
IkReal x54=((IkReal(0.0133280000000000))*(x42));
IkReal x55=((IkReal(1.00000000000000))*(x43));
IkReal x56=((cj13)*(x49));
IkReal x57=((x44)*(x49));
IkReal x58=((x42)*(x49));
evalcond[0]=((((x40)*(x46)))+(((IkReal(-0.0300000000000000))*(x40)))+(((IkReal(-1.00000000000000))*(px)*(x53))));
evalcond[1]=((IkReal(0.000900000000000000))+(((IkReal(-0.000900000000000000))*(x49)))+(((x41)*(x49)))+(((x41)*(x42)))+(((IkReal(-1.00000000000000))*(x51)*(x58)))+(((px)*(x46)*(x47)))+(((IkReal(-1.00000000000000))*(x42)*(x55)))+(((x43)*(x58)))+(((IkReal(-1.00000000000000))*(px)*(x46)*(x47)*(x49)))+(((IkReal(-1.00000000000000))*(x51))));
evalcond[2]=((IkReal(0.0142280000000000))+(((IkReal(0.00588000000000000))*(x53)*(x56)))+(((IkReal(-1.00000000000000))*(x52)*(x58)))+(((x43)*(x49)))+(((IkReal(-1.00000000000000))*(x48)*(x56)))+(((IkReal(0.00588000000000000))*(cj13)*(sj13)*(x50)))+(((IkReal(0.00372400000000000))*(x58)))+(((x42)*(x57)))+(((IkReal(-1.00000000000000))*(py)*(pz)*(x40)*(x45)*(x47)))+(((cj13)*(x48)))+(((IkReal(0.0133280000000000))*(x50)))+(((x49)*(x52)))+(((IkReal(-0.00462400000000000))*(x49)))+(((IkReal(-1.00000000000000))*(x54)))+(((IkReal(-1.00000000000000))*(x55)))+(((IkReal(-1.00000000000000))*(x50)*(x54)))+(((IkReal(-1.00000000000000))*(x57))));
if( IKabs(evalcond[0]) > 0.000001  || IKabs(evalcond[1]) > 0.000001  || IKabs(evalcond[2]) > 0.000001  )
{
continue;
}
}

IkReal soleval[1];
soleval[0]=((IkReal(-0.0232000000000000))+(((cj14)*(((IkReal(-0.0680000000000000))+(((cj13)*(px)))+(((py)*(sj13)))))))+(((IkReal(-1.00000000000000))*(pz)*(sj14))));
if( soleval[0] > 0.0000000000000000  )
{
std::vector<IkSingleDOFSolutionBase<IkReal> > vinfos(2);
vinfos[0].jointtype = 1;
vinfos[0].foffset = j13;
vinfos[0].indices[0] = _ij13[0];
vinfos[0].indices[1] = _ij13[1];
vinfos[0].maxsolutions = _nj13;
vinfos[1].jointtype = 1;
vinfos[1].foffset = j14;
vinfos[1].indices[0] = _ij14[0];
vinfos[1].indices[1] = _ij14[1];
vinfos[1].maxsolutions = _nj14;
std::vector<int> vfree(0);
solutions.AddSolution(vinfos,vfree);
}
}
}

}

}
}
}

}

}
}
return solutions.GetNumSolutions()>0;
}

};


/// solves the inverse kinematics equations.
/// \param pfree is an array specifying the free joints of the chain.
IKFAST_API bool ComputeIk(const IkReal* eetrans, const IkReal* eerot, const IkReal* pfree, IkSolutionListBase<IkReal>& solutions) {
IKSolver solver;
return solver.ComputeIk(eetrans,eerot,pfree,solutions);
}

IKFAST_API const char* GetKinematicsHash() { return "80f514166e15c34bd64294fc1fdd5ddd"; }

IKFAST_API const char* GetIkFastVersion() { return IKFAST_STRINGIZE(IKFAST_VERSION); }

#ifdef IKFAST_NAMESPACE
} // end namespace
#endif

#ifndef IKFAST_NO_MAIN
#include <stdio.h>
#include <stdlib.h>
#ifdef IKFAST_NAMESPACE
using namespace IKFAST_NAMESPACE;
#endif
int main(int argc, char** argv)
{
    if( argc != 12+GetNumFreeParameters()+1 ) {
        printf("\nUsage: ./ik r00 r01 r02 t0 r10 r11 r12 t1 r20 r21 r22 t2 free0 ...\n\n"
               "Returns the ik solutions given the transformation of the end effector specified by\n"
               "a 3x3 rotation R (rXX), and a 3x1 translation (tX).\n"
               "There are %d free parameters that have to be specified.\n\n",GetNumFreeParameters());
        return 1;
    }

    IkSolutionList<IkReal> solutions;
    std::vector<IkReal> vfree(GetNumFreeParameters());
    IkReal eerot[9],eetrans[3];
    eerot[0] = atof(argv[1]); eerot[1] = atof(argv[2]); eerot[2] = atof(argv[3]); eetrans[0] = atof(argv[4]);
    eerot[3] = atof(argv[5]); eerot[4] = atof(argv[6]); eerot[5] = atof(argv[7]); eetrans[1] = atof(argv[8]);
    eerot[6] = atof(argv[9]); eerot[7] = atof(argv[10]); eerot[8] = atof(argv[11]); eetrans[2] = atof(argv[12]);
    for(std::size_t i = 0; i < vfree.size(); ++i)
        vfree[i] = atof(argv[13+i]);
    bool bSuccess = ComputeIk(eetrans, eerot, vfree.size() > 0 ? &vfree[0] : NULL, solutions);

    if( !bSuccess ) {
        fprintf(stderr,"Failed to get ik solution\n");
        return -1;
    }

    printf("Found %d ik solutions:\n", (int)solutions.GetNumSolutions());
    std::vector<IkReal> solvalues(GetNumJoints());
    for(std::size_t i = 0; i < solutions.GetNumSolutions(); ++i) {
        const IkSolutionBase<IkReal>& sol = solutions.GetSolution(i);
        printf("sol%d (free=%d): ", (int)i, (int)sol.GetFree().size());
        std::vector<IkReal> vsolfree(sol.GetFree().size());
        sol.GetSolution(&solvalues[0],vsolfree.size()>0?&vsolfree[0]:NULL);
        for( std::size_t j = 0; j < solvalues.size(); ++j)
            printf("%.15f, ", solvalues[j]);
        printf("\n");
    }
    return 0;
}

#endif

#include "plugindefs.h" 
namespace IKFAST_NAMESPACE {
IkSolverBasePtr CreateIkSolver(EnvironmentBasePtr penv, std::istream& sinput, const std::vector<dReal>& vfreeinc) {
    boost::shared_ptr<ikfast::IkFastFunctions<IkReal> > ikfunctions(new ikfast::IkFastFunctions<IkReal>());
    ikfunctions->_ComputeIk = IKFAST_NAMESPACE::ComputeIk;
    ikfunctions->_ComputeFk = IKFAST_NAMESPACE::ComputeFk;
    ikfunctions->_GetNumFreeParameters = IKFAST_NAMESPACE::GetNumFreeParameters;
    ikfunctions->_GetFreeParameters = IKFAST_NAMESPACE::GetFreeParameters;
    ikfunctions->_GetNumJoints = IKFAST_NAMESPACE::GetNumJoints;
    ikfunctions->_GetIkRealSize = IKFAST_NAMESPACE::GetIkRealSize;
    ikfunctions->_GetIkFastVersion = IKFAST_NAMESPACE::GetIkFastVersion;
    ikfunctions->_GetIkType = IKFAST_NAMESPACE::GetIkType;
    ikfunctions->_GetKinematicsHash = IKFAST_NAMESPACE::GetKinematicsHash;
    return CreateIkFastSolver(penv,sinput,ikfunctions,vfreeinc);
}
} // end namespace
