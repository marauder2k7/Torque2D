#ifndef _OBJECTTYPES_H_
#define _OBJECTTYPES_H_

#include "platform/types.h"

// Types used for SimObject type masks (SimObject::mTypeMask)
//

/* NB!  If a new object type is added, don't forget to add it to the
 *      consoleInit function in simBase.cc
 */

enum SimObjectTypes
{
   /// @name Types used by the SceneObject class
   /// @{
   DefaultObjectType = 0,
   StaticObjectType = BIT(0),
   GameBaseObjectType = BIT(1),
   ShapeBaseObjectType = BIT(2),
   CameraObjectType = BIT(3),
   StaticShapeObjectType = BIT(4),
   PlayerObjectType = BIT(5),
   ItemObjectType = BIT(6),
};
#endif