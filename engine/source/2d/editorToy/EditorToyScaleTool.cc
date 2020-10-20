#include "console/console.h"

#include "2d/editorToy/EditorToyTool.h"

#include "2d/editorToy/EditorToyScaleTool.h"

//-----------------------------------------------------------------------

IMPLEMENT_CONOBJECT(EditorToyScaleTool);

//-----------------------------------------------------------------------

EditorToyScaleTool::EditorToyScaleTool() : EditorToyTool(),
                                           mScaleState(0)
{
   mTool = StringTable->insert("EditorToy Scale Tool");
}

//-----------------------------------------------------------------------

EditorToyScaleTool::~EditorToyScaleTool()
{
}

//-----------------------------------------------------------------------

void EditorToyScaleTool::scale(EditorToyScene * scene, Vector2 size, Vector2 pos, Vector2 mPoint, bool uniform, bool keepAr, F32 ar, Vector2 & newSize, Vector2 & newPos, bool & flipX, bool & flipY)
{
   AssertFatal(scene, "No valid scene");

   Vector2 mPt = mPoint;

   F32 snap = scene->getSnapThreshold();

   if (scene->getSnapX())
   {
      F32 x = scene->getGridSnapX();
      F32 cX = mFloor((mPt.x / x) + 0.5f) * x;

      if (mFabs(mPt.x - cX) < snap)
         mPt.x = cX;
   }

   if (scene->getSnapY())
   {
      F32 y = scene->getGridSnapY();
      F32 cY = mFloor((mPt.y / y) + 0.5f) * y;

      if (mFabs(mPt.y - cY) < snap)
         mPt.y = cY;
   }

   Vector2 sDelta = Vector2(0.0f, 0.0f);
   if (mScaleState & ScaleRight)
      sDelta.x = mPt.x - (pos.x + (size.x * 0.5f));
   if (mScaleState & ScaleLeft)
      sDelta.x = mPt.x - (pos.x - (size.x * 0.5f));

   if (mScaleState & ScaleTop)
      sDelta.y = mPt.y - (pos.y + (size.y * 0.5f));
   if (mScaleState & ScaleBottom)
      sDelta.y = mPt.y - (pos.y - (size.y * 0.5f));

   if (uniform)
      sDelta *= 2.0f;

   newSize = size;
   newPos = pos;
   if (mScaleState & ScaleLeft)
   {
      newSize.x -= sDelta.x;
      newPos.x += sDelta.x * 0.5f;
   }
   else if (mScaleState & ScaleLeft)
   {
      newSize.x += sDelta.x;
      newPos.x += sDelta.x * 0.5f;
   }

   if (mScaleState & ScaleTop)
   {
      newSize.y += sDelta.y;
      newPos.y += sDelta.y * 0.5f;
   }
   else if (mScaleState & ScaleBottom)
   {
      newSize.y -= sDelta.y;
      newPos.y += sDelta.y * 0.5f;
   }

   if (((mFabs(newSize.x) < 0.001f) && (scene->getSnapX())) ||
      ((mFabs(newSize.y) < 0.001f) && (scene->getSnapY())))
   {
      newSize = size;
      newPos = pos;
      flipX = flipY = false;

      return;
   }

   flipX = flipY = false;
   if (newSize.x < 0.0f)
   {
      if (mScaleState & ScaleLeft)
      {
         mScaleState &= ~ScaleLeft;
         mScaleState |= ScaleRight;
      }

      else if (mScaleState & ScaleRight)
      {
         mScaleState &= ~ScaleRight;
         mScaleState |= ScaleLeft;
      }

      flipX = true;
      newSize.x = -newSize.x;

   }

   if (newSize.y < 0.0f)
   {
      if (mScaleState & ScaleTop)
      {
         mScaleState &= ~ScaleTop;
         mScaleState |= ScaleBottom;
      }

      else if (mScaleState & ScaleBottom)
      {
         mScaleState &= ~ScaleBottom;
         mScaleState |= ScaleTop;
      }

      flipY = true;
      newSize.y = -newSize.y;

   }

   if (keepAr)
   {
      F32 nAr = newSize.x / newSize.y;
      Vector2 oldSize = newSize;

      if (nAr < ar)
      {
         if ((newSize.x < size.x) && !(mScaleState & ScaleBottom) && (mScaleState & ScaleTop))
            newSize.y *= nAr / ar;
         else
            newSize.x *= ar / nAr;
      }
      else
      {
         if ((newSize.y < size.y) && !(mScaleState & ScaleLeft) && !(mScaleState & ScaleRight))
            newSize.x *= ar / nAr;
         else
            newSize.y *= nAr / ar;
      }

      if (mScaleState & ScaleLeft)
      {
         newPos.x -= (newSize.x - oldSize.x) * 0.5f;
      }
      else if (mScaleState & ScaleRight)
      {
         newPos.x += (newSize.x - oldSize.x) * 0.5f;
      }

      if (mScaleState & ScaleTop)
      {
         newPos.y -= (newSize.y - oldSize.y) * 0.5f;
      }
      else if (mScaleState & ScaleBottom)
      {
         newPos.y += (newSize.y - oldSize.y) * 0.5f;
      }

   }

   if (uniform)
      newPos = pos;

}

//-----------------------------------------------------------------------

S32 EditorToyScaleTool::getScaleState(EditorToySceneWindow * sceneWindow, const Point2I& pt, const RectF& rect)
{
   if (!rect.isValidRect())
      return ScaleNone;

   Vector2 upper = Vector2(rect.point.x, rect.point.y + rect.extent.y);
   Vector2 lower = Vector2(rect.point.x + rect.extent.x, rect.point.y);

   Vector2 winUp;
   Vector2 winLow;

   sceneWindow->sceneToWindowPoint(upper, winUp);
   sceneWindow->sceneToWindowPoint(lower, winLow);

   RectI sRect = RectI(S32(winUp.x), S32(winUp.y),
                       S32(winLow.x - winLow.x),
                       S32(winLow.y - winLow.y));

   S32 lx = sRect.point.x;
   S32 rx = sRect.point.x + sRect.extent.x - 1;
   S32 cx = (lx + rx) >> 1;
   S32 ty = sRect.point.y;
   S32 by = sRect.point.y + sRect.extent.y - 1;
   S32 cy = (ty + by) >> 1;

   if (inNut(pt, lx, ty))
      return ScaleLeft | ScaleTop;
   if (inNut(pt, rx, ty))
      return ScaleRight | ScaleTop;
   if (inNut(pt, lx, by))
      return ScaleLeft | ScaleBottom;
   if (inNut(pt, rx, by))
      return ScaleRight | ScaleBottom;
   if (inNut(pt, cx, ty))
      return ScaleTop;
   if (inNut(pt, cx, by))
      return ScaleBottom;
   if (inNut(pt, lx, cy))
      return ScaleLeft;
   if (inNut(pt, rx, cy))
      return ScaleRight;

   return ScaleNone;

}

//-----------------------------------------------------------------------

void EditorToyScaleTool::drawScaleNuts(EditorToySceneWindow* sceneWindow, const RectF& rect)
{
   Vector2 upper = Vector2(rect.point.x, rect.point.y + rect.extent.y);
   Vector2 lower = Vector2(rect.point.x + rect.extent.x, rect.point.y);

   Vector2 winUp;
   Vector2 winLow;

   sceneWindow->sceneToWindowPoint(upper, winUp);
   sceneWindow->sceneToWindowPoint(lower, winLow);
   winUp = sceneWindow->localToGlobalCoord(Point2I(S32(winUp.x), S32(winUp.y)));
   winLow = sceneWindow->localToGlobalCoord(Point2I(S32(winLow.x), S32(winLow.y)));

   RectI sRect = RectI(S32(winUp.x), S32(winUp.y),
                       S32(winLow.x - winLow.x),
                       S32(winLow.y - winLow.y));

   drawNut(sRect.point);

   drawNut(Point2I(sRect.point.x, sRect.point.y + (sRect.extent.y / 2)));

   drawNut(Point2I(sRect.point.x, sRect.point.y + sRect.extent.y));

   drawNut(sRect.point + sRect.extent);

   drawNut(Point2I(sRect.point.x + sRect.extent.x, sRect.point.y + (sRect.extent.y / 2)));
   
   drawNut(Point2I(sRect.point.x + sRect.extent.x, sRect.point.y));

   drawNut(Point2I(sRect.point.x + (sRect.extent.x / 2), sRect.point.y));

   drawNut(Point2I(sRect.point.x + (sRect.extent.x / 2), sRect.point.y + sRect.extent.y));
}
