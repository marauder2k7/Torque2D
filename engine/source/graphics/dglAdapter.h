#ifndef _DGLADAPTER_H_
#define _DGLADAPTER_H_

#ifndef _DGLSTRUCTS_H_
#include "graphics/dglStructs.h"
#endif // !_DGLSTRUCTS_H_

struct DGLAdapter
{
public:

   enum
   {
      MaxAdapterNameLen = 512,
   };

   char mName[MaxAdapterNameLen];
   char mOutputName[MaxAdapterNameLen];

   Vector<DGLVideoMode> mAvailableModes;

   const char * getName() const { return mName; }
   const char * getOutputName() const { return mOutputName; }

   U32            mIndex;

   DGLAdapter()
   {
      VECTOR_SET_ASSOCIATION(mAvailableModes);

      mName[0] = 0;
      mOutputName[0] = 0;
      mIndex = 0;
   }

   ~DGLAdapter()
   {
      mAvailableModes.clear();
   }

};

#endif // !_DGLADAPTER_H_
