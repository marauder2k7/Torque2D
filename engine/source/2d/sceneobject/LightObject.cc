#include "graphics/dgl.h"
#include "console/consoleTypes.h"
#include "2d/core/Utility.h"
#include "2d/sceneobject/LightObject.h"

// Script bindings.
#include "LightObject_ScriptBinding.h"

IMPLEMENT_CONOBJECT(LightObject);

LightObject::LightObject():
   mLightRadius(10.0f),
   mLightSegments(15)
{
   mSrcBlendFactor = DGLBlendSrcAlpha;
   mDstBlendFactor = DGLBlendOne;
   // Use a static body by default.
   mBodyDefinition.type = b2_staticBody;
}

LightObject::~LightObject()
{

}

bool LightObject::onAdd()
{
   // Call Parent.
   if (!Parent::onAdd())
      return false;

   // Return Okay.
   return true;
}

//----------------------------------------------------------------------------

void LightObject::onRemove()
{
   // Call Parent.
   Parent::onRemove();
}

void LightObject::initPersistFields()
{
   Parent::initPersistFields();

   /// Light settings.
   addProtectedField("LightRadius", TypeF32, Offset(mLightRadius, LightObject), &setLightRadius, &defaultProtectedGetFn, &writeLightRadius, "");
   addProtectedField("LightSegments", TypeS32, Offset(mLightSegments, LightObject), &setLightSegments, &defaultProtectedGetFn, &writeLightSegments, "");

}

void LightObject::safeDelete(void)
{
   Parent::safeDelete();
}

void LightObject::sceneRender(const SceneRenderState * sceneRenderState, const SceneRenderRequest * sceneRenderRequest, BatchRender * batchRender)
{
   Vector2 worldPos = getPosition();
   Vector<Vector2> verts;
   Vector<RayList> bList;
   S32 mSrcLightBlend = getSrcBlendFactor();
   S32 mDstLightBlend = getDstBlendFactor();
   ColorF mLightColor = getBlendColor();

   F32 radius = getLightRadius();
   Scene* scene = getScene();
   b2World* mWorld = scene->getWorld();

   Vector<F32> lightVerts;
   Vector<F32> colorPoint;

   DGL->EnableState(DGLRSBlend);
   DGL->DisableState(DGLRSTexture2D);
   DGL->PushMatrix();
   DGL->SetTranslate(worldPos.x, worldPos.y, 0);
   DGL->SetPolygonMode(DGLFront, DGLFill);
   
   DGL->setBlendFunc((DGLBlend)mSrcLightBlend, (DGLBlend)mDstLightBlend);
   // Creates the fading dark region.
   colorPoint.push_back(mLightColor.red);
   colorPoint.push_back(mLightColor.green);
   colorPoint.push_back(mLightColor.blue);
   colorPoint.push_back(mLightColor.alpha);
   lightVerts.push_back(0.0f);
   lightVerts.push_back(0.0f);
   //check scene objects
   U32 objCount = scene->getSceneObjectCount();
   for (U32 i = 0; i < objCount; i++)
   {
      SceneObject *tObj = scene->getSceneObject(i);
      Vector2 dist = worldPos - tObj->getPosition();
      const F32 distSqr = dist.LengthSquared();
      const F32 radSqr = radius * radius;
      //within radius?
      if (distSqr < radSqr || distSqr == radSqr)
      {
         U32 shapeCount = tObj->getCollisionShapeCount();
         for (U32 j = 0; j < shapeCount; j++)
         {
            //All vertices from polygon collision shape
            if (tObj->getCollisionShapeType(j) == b2Shape::e_polygon)
            {
               U32 pCount = tObj->getPolygonCollisionShapePointCount(j);

               for (U32 k = 0; k < pCount; k++)
               {
                  Vector2 locPoint = tObj->getPolygonCollisionShapeLocalPoint(j, k);
                  Vector2 wPoint = tObj->getWorldPoint(locPoint);
                  verts.push_back(wPoint);
               }
            }
         }
      }
   }
   /*    \ | /
        -  +  -
         / | \
   */

   U32 lightSeg = getLightSegments();
   F32 segAng = 360.0f / lightSeg;

   for (U32 i = 0; i <= lightSeg; i++)
   {
      b2Vec2 segStrt = worldPos;
      b2Vec2 segEnd = segStrt + radius * b2Vec2(mCos(mDegToRad((F32)segAng * i)), mSin(mDegToRad((F32)segAng * i)));
      verts.push_back(segEnd);
   }

   //cast ray to vertices
   for (S32 l = 0; l < verts.size(); l++)
   {
      F32 rayLength = radius;
      Vector2 p1 = worldPos;
      Vector2 p2 = verts[l];
      F32 baseAng = mAtan(p2.x, p2.y);
      F32 cAng = 0;
      for (int m = 0; m < 2; m++)
      {
         if (m == 0)cAng = baseAng - 0.0001f;
         //if (m == 1)cAng = baseAng;
         if (m == 1)cAng = baseAng + 0.0001f;

         p2.x = rayLength * mCos(cAng);
         p2.y = rayLength * mSin(cAng);

         RaysCastCallback callback;
         mWorld->RayCast(&callback, p1, p2);
         if (callback.m_fixture)
         {

            F32 ang = mAtan(callback.m_point.x - p1.x, callback.m_point.y - p1.y);
            Vector2 intersection = p1 + callback.m_fraction * (p2 - p1);
            
            RayList intersectionPoint;
            intersectionPoint.ang = ang;
            intersectionPoint.x = intersection.x;
            intersectionPoint.y = intersection.y;
            intersectionPoint.l = callback.m_fraction;


            bList.push_back_unique(intersectionPoint);

         }
         else
         {
            F32 ang = mAtan(p2.x - p1.x, p2.y - p1.y);

            RayList intersectionPoint;
            intersectionPoint.ang = ang;
            intersectionPoint.x = p2.x;
            intersectionPoint.y = p2.y;
            intersectionPoint.l = 1.0;

            bList.push_back_unique(intersectionPoint);
         }
      }
   }

   //Con::printf("Rays cast: %i", bList.size());
   //sort the list
   if (bList.size() > 1)
   {
      dQsort(bList.address(), bList.size(), sizeof(RayList), sortRays);
   }

   //triangle fan
   for (S32 m = 0; m < bList.size(); m++)
   {
      colorPoint.push_back((mLightColor.green - (mLightColor.green * bList[m].l)));
      colorPoint.push_back((mLightColor.green - (mLightColor.green * bList[m].l)));
      colorPoint.push_back((mLightColor.blue - (mLightColor.blue * bList[m].l)));
      colorPoint.push_back((mLightColor.alpha - (mLightColor.alpha * bList[m].l)));
      lightVerts.push_back(bList[m].x);
      lightVerts.push_back(bList[m].y);
   }
   //close off the circle
   colorPoint.push_back((mLightColor.green - (mLightColor.green * bList[0].l)));
   colorPoint.push_back((mLightColor.green - (mLightColor.green * bList[0].l)));
   colorPoint.push_back((mLightColor.blue - (mLightColor.blue * bList[0].l)));
   colorPoint.push_back((mLightColor.alpha - (mLightColor.alpha * bList[0].l)));
   lightVerts.push_back(bList[0].x);
   lightVerts.push_back(bList[0].y);
   
   DGL->SetVertexPoint(2, 0, lightVerts.address());
   DGL->EnableClientState(DGLCSVertexArray);
   DGL->SetColorPoint(4, 0, colorPoint.address());
   DGL->EnableClientState(DGLCSColorArray);
   DGL->DrawArrays(DGLTriangleFan, 0, bList.size() + 2);
   DGL->DisableClientState(DGLCSVertexArray);
   DGL->DisableClientState(DGLCSColorArray);
   DGL->DisableState(DGLRSBlend);
   DGL->PopMatrix();
}

void LightObject::OnRegisterScene(Scene* mScene)
{
   Parent::OnRegisterScene(mScene);
   mScene->getWorldQuery()->addAlwaysInScope(this);

}

void LightObject::OnUnregisterScene(Scene* mScene)
{
   mScene->getWorldQuery()->removeAlwaysInScope(this);
   Parent::OnUnregisterScene(mScene);
}

S32 QSORT_CALLBACK sortRays(const void* a, const void* b)
{
   RayList* ray_a = (RayList*) a;
   RayList* ray_b = (RayList*) b;

   if (ray_a->ang < ray_b->ang)
   {
      return -1;
   }
   else if (ray_a->ang > ray_b->ang)
   {
      return 1;
   }
  
   return 0;

}
