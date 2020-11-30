//-----------------------------------------------------------------------------
// Copyright (c) 2013 GarageGames, LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------

#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#include "platform/platform.h"
#include "platform/threads/mutex.h"
#include "platform/threads/thread.h"
#include "memory/safeDelete.h"

// TODO: examine & dump errno if pthread_* funcs fail. ( only in debug build )

struct PlatformMutexData
{
   pthread_mutex_t   mMutex;
};

Mutex::Mutex(void)
{
   mData = new PlatformMutexData;
   pthread_mutexattr_t attr;

   pthread_mutexattr_init(&attr);
   pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);

   pthread_mutex_init(&mData->mMutex, &attr);
}

Mutex::~Mutex()
{
   AssertFatal(mData, "Mutex::destroyMutex: invalid mutex");
   pthread_mutex_destroy(&mData->mMutex);
   SAFE_DELETE(mData);
}
 
bool Mutex::lock( bool block)
{
   if (mData == NULL)
      return false;
   if (block)
   {
      return pthread_mutex_lock(&mData->mMutex) == 0;
   }
   else
   {
      return pthread_mutex_trylock(&mData->mMutex) == 0;
   }

}

void Mutex::unlock()
{
   if (mData == NULL)
      return;
   pthread_mutex_unlock( &mData->mMutex);
}
