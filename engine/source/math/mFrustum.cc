#include "platform/platform.h"
#include "math/mMathFn.h"
#include "math/mFrustum.h"

//ProjUtils
namespace ProjUtil
{
   void makeProjection(MatrixF *outMatrix,
      F32 left,
      F32 right,
      F32 bottom,
      F32 top,
      F32 nearPlane,
      F32 farPlane)
   {
      Point4F row;
      row.x = 2.0f * nearPlane / (right - left);
      row.y = 0.0f;
      row.z = 0.0f;
      row.w = 0.0f;
      outMatrix->setRow(0, row);

      row.x = 0.0f;
      row.y = 2.0f * nearPlane / (top - bottom);
      row.z = 0.0f;
      row.w = 0.0f;
      outMatrix->setRow(1, row);

      row.x = (left + right) / (right - left);
      row.y = (top + bottom) / (top - bottom);
      row.z = farPlane / (nearPlane - farPlane);
      row.w = -1.0f;
      outMatrix->setRow(2, row);

      row.x = 0.0f;
      row.y = 0.0f;
      row.z = farPlane * nearPlane / (nearPlane - farPlane);
      row.w = 0.0f;
      outMatrix->setRow(3, row);

      outMatrix->transpose();

   }

   void makeOrthoProjection(MatrixF *outMatrix,
      F32 left,
      F32 right,
      F32 bottom,
      F32 top,
      F32 nearPlane,
      F32 farPlane)
   {
      Point4F row;
      row.x = 2.0f / (right - left);
      row.y = 0.0f;
      row.z = 0.0f;
      row.w = 0.0f;
      outMatrix->setRow(0, row);

      row.x = 0.0f;
      row.y = 2.0f / (top - bottom);
      row.z = 0.0f;
      row.w = 0.0f;
      outMatrix->setRow(1, row);

      row.x = 0.0f;
      row.y = 0.0f;
      row.z = 1.0f / (nearPlane - farPlane);
      row.w = 0.0f;

      outMatrix->setRow(2, row);

      row.x = (left + right) / (left - right);
      row.y = (top + bottom) / (bottom - top);
      row.z = nearPlane / (nearPlane - farPlane);
      row.w = 1.0f;
      outMatrix->setRow(3, row);

      outMatrix->transpose();
   }
}
//ProjUtils END

Frustum::Frustum(F32 left, F32 right, F32 bottom, F32 top, F32 nearPlane, F32 farPlane, bool ortho)
{
   mLeft       = left;
   mRight      = right;
   mBottom     = bottom;
   mTop        = top;
   mNearPlane  = nearPlane;
   mFarPlane   = farPlane;
   mOrtho      = ortho;
}

void Frustum::set(F32 left, F32 right, F32 bottom, F32 top, F32 nearPlane, F32 farPlane, bool ortho)
{
   mLeft       = left;
   mRight      = right;
   mBottom     = bottom;
   mTop        = top;
   mNearPlane  = nearPlane;
   mFarPlane   = farPlane;
   mOrtho      = ortho;
}

void Frustum::getProjectionMatrix(MatrixF *proj) const
{
   if (mOrtho)
   {
      ProjUtil::makeOrthoProjection(proj, mLeft, mRight, mBottom, mTop, mNearPlane, mFarPlane);
   }
   else
   {
      ProjUtil::makeProjection(proj, mLeft, mRight, mBottom, mTop, mNearPlane, mFarPlane);
   }
}
