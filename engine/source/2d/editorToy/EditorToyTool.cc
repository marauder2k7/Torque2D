#include "console/consoleInternal.h"
#include "console/console.h"
#include "graphics/dgl.h"
#include "2d/editorToy/EditorToyTool.h"

#include "2d/editorToy/EditorToyTool_ScriptBinding.h"

//-----------------------------------------------------------------------

IMPLEMENT_CONOBJECT(EditorToyTool);

//-----------------------------------------------------------------------

EditorToyTool::EditorToyTool() : SimObject(),
                                 mActive(false),
                                 mNutCol(128, 128, 128, 128),
                                 mNutOutCol(128, 128, 128, 255),
                                 mNutSize(5)
{
   VECTOR_SET_ASSOCIATION(mNuts);
   mTool = StringTable->insert("EditorToy Tool");
}

EditorToyTool::~EditorToyTool()
{

}

bool EditorToyTool::onAdd()
{
   if (!Parent::onAdd())
      return false;

   StringTableEntry objName = getName();
   if (objName && objName[0] && getClassRep())
   {
      Namespace *parent = getClassRep()->getNameSpace();
      if (Con::linkNamespaces(parent->mName, objName))
         mNameSpace = Con::lookupNamespace(objName);
   }

   return true;
}

bool EditorToyTool::onActivate(EditorToySceneWindow* sceneWin)
{
   if (mActive)
      return false;

   if (!sceneWin || !sceneWin->getEditScene())
      return false;

   mActive = true;
   return true;
}

void EditorToyTool::onDeactivate()
{
   if (!mActive)
      return;

   mActive = false;
}

bool EditorToyTool::onAcquireObj(SceneObject* obj)
{
   if (!mActive)
      return false;

   return true;
}

void EditorToyTool::onRelinquishObj(SceneObject * obj)
{
}

void EditorToyTool::drawNut(Point2I pos)
{
   RectI rect(pos.x - mNutSize, pos.y - mNutSize, 2 * mNutSize + 1, 2 * mNutSize + 1);
   rect.inset(1, 1);
   dglDrawRectFill(rect, mNutCol);
   rect.inset(-1, -1);
   dglDrawRect(rect, mNutOutCol);
}

void EditorToyTool::drawArrow(Point2I pos)
{
   RectI rect((F32)(pos.x - (F32)mNutSize), (F32)(pos.y - (F32)mNutSize), (F32)(2.0f * mNutSize), (F32)(2.0f * mNutSize));

   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glDisable(GL_TEXTURE_2D);

   glColor4ubv(&mNutCol.blue);

   glBegin(GL_TRIANGLE_STRIP);
   glVertex2f(rect.point.x + 0.5f, rect.point.y + 0.5f);
   glVertex2f(rect.point.x + rect.extent.x + 0.5f, rect.point.y + 0.5f);
   glVertex2f(rect.point.x + 0.5f, rect.point.y + rect.extent.y + 0.5f);
   glVertex2f(rect.point.x + rect.extent.x + 0.5f, rect.point.y + rect.extent.y + 0.5f);
   glVertex2f(rect.point.x + rect.extent.x - (rect.extent.x / 2.0f) + 0.5f, rect.point.y + rect.extent.y + (rect.extent.y / 2.0f) + 0.5f);
   glEnd();

   glColor4ubv(&mNutOutCol.blue);

   glBegin(GL_LINE_STRIP);
   glVertex2f(rect.point.x + 0.5f, rect.point.y + 0.5f);
   glVertex2f(rect.point.x + rect.extent.x + 0.5f, rect.point.y + 0.5f);
   glVertex2f(rect.point.x + rect.extent.x + 0.5f, rect.point.y + rect.extent.y + 0.5f);
   glVertex2f(rect.point.x + rect.extent.x - (rect.extent.x / 2.0f) + 0.5f, rect.point.y + rect.extent.y + (rect.extent.y / 2.0f) + 0.5f);
   glVertex2f(rect.point.x + 0.5f, rect.point.y + rect.extent.y + 0.5f);
   glVertex2f(rect.point.x + 0.5f, rect.point.y + 0.5f);
   glEnd();

}

bool EditorToyTool::inNut(Point2I pt, S32 x, S32 y)
{
   S32 size = mNutSize << 1;

   S32 px = pt.x - x;
   S32 py = pt.y - y;

   return px <= size && px >= -size && py <= size && py >= -size;

}