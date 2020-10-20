#ifndef _EDITORTOYSCALETOOL_H_
#define _EDITORTOYSCALETOOL_H_

#include "2d/editorToy/EditorToyTool.h"

#include "2d/editorToy/EditorToyScene.h"

#include "console/consoleTypes.h"

class EditorToyScaleTool : public EditorToyTool
{
protected:

   typedef EditorToyTool Parent;

   enum ScaleState
   {
      ScaleNone = 0,
      ScaleLeft = 1,
      ScaleRight = 2,
      ScaleTop = 4,
      ScaleBottom = 8
   };

   U32 mScaleState;

   virtual void scale(EditorToyScene* scene, Vector2 size, 
                     Vector2 pos, Vector2 mPoint, 
                     bool uniform, bool keepAr, F32 ar, 
                     Vector2& newSize, Vector2& newPos, 
                     bool& flipX, bool& flipY);

public:
   EditorToyScaleTool();
   virtual ~EditorToyScaleTool();

   virtual void drawScaleNuts(EditorToySceneWindow* sceneWindow, const RectF& rect);
   virtual S32 getScaleState(EditorToySceneWindow* sceneWindow, const Point2I& pt, const RectF& rect);

   DECLARE_CONOBJECT(EditorToyScaleTool);

};



#endif // !_EDITORTOYSCALETOOL_H_
