
#include "console/console.h"
#include "console/consoleTypes.h"
#include "graphics/dgl.h"

#include "2d/editorToy/EditorToySelectTool.h"

EditorToySelectTool::EditorToySelectTool() : EditorToyTool(),
mHover(NULL),
mHoverFillCol(100,0,50,100),
mHoverStrokeCol(100,0,50,255),
mHoverStrokeSize(2.0f),
mSelectState(0),
mDragRect(0,0,-1,-1),
mMouseAr(0.5f)

{
}

EditorToySelectTool::~EditorToySelectTool()
{
}
