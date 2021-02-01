#ifndef _FRUSTUM_H_
#define _FRUSTUM_H_

#ifndef _MBOX_H_
#include "math/mBox.h"
#endif // !_MBOX_H_

#ifndef _MMATRIX_H_
#include "math/mMatrix.h"
#endif // !_MMATRIX_H_

namespace ProjUtil
{
   void makeProjection(MatrixF *outMatrix,
      F32 left,
      F32 right,
      F32 bottom,
      F32 top,
      F32 nearPlane,
      F32 farPlane);

   void makeOrthoProjection(MatrixF *outMatrix,
      F32 left,
      F32 right,
      F32 bottom,
      F32 top,
      F32 nearPlane,
      F32 farPlane);
} //ProjUtilEnd

//This is a super simple frustum
class Frustum
{
protected:

   F32   mLeft;
   F32   mRight;
   F32   mBottom;
   F32   mTop;
   F32   mNearPlane;
   F32   mFarPlane;
   bool  mOrtho;

public:

   Frustum(F32 left  = -1.0f,
      F32 right      = 1.0f,
      F32 bottom     = -1.0f,
      F32 top        = 1.0f,
      F32 nearPlane  = 0.01f,
      F32 farPlane   = 1.0f,
      bool ortho     = false );

   void set(F32 left,
            F32 right,
            F32 bottom,
            F32 top,
            F32 nearPlane,
            F32 farPlane,
            bool ortho);

   void getProjectionMatrix(MatrixF *proj) const;

   //Getters
   F32 getLeft() const        { return mLeft; }
   F32 getRight() const       { return mRight; }
   F32 getBottom() const      { return mBottom; }
   F32 getTop() const         { return mTop; }
   F32 getNearPlane() const   { return mNearPlane; }
   F32 getFarPlane() const    { return mFarPlane; }

};

#endif