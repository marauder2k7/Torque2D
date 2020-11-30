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

#ifndef _SPRITE_BATCH_QUERY_RESULT_H_
#define _SPRITE_BATCH_QUERY_RESULT_H_

#ifndef _VECTOR2_H_
#include "2d/core/Vector2.h"
#endif

///-----------------------------------------------------------------------------

class SpriteBatchItem;

///-----------------------------------------------------------------------------

struct SpriteBatchQueryResult
{
    SpriteBatchQueryResult() :
        mpSpriteBatchItem( NULL ),
        mPoint( 0.0f, 0.0f ),
        mNormal( 0.0f, 0.0f ),
        mFraction( 0.0f ),
        mShapeIndex(0)
    {
    }

    /// Initialize a non-ray-cast result.
    SpriteBatchQueryResult( SpriteBatchItem* mpSpriteBatchItem ) :
        mpSpriteBatchItem( mpSpriteBatchItem ),
        mPoint( 0.0f, 0.0f ),
        mNormal( 0.0f, 0.0f ),
        mFraction( 0.0f ),
        mShapeIndex(0)
    {
    }

    /// Initialize a ray-cast result.    
    SpriteBatchQueryResult( SpriteBatchItem* mpSpriteBatchItem, const b2Vec2& point, const b2Vec2& normal, const F32 fraction, const U32 shapeIndex ) :
        mpSpriteBatchItem( mpSpriteBatchItem ),
        mPoint(point),
        mNormal(normal),
        mFraction(fraction),
        mShapeIndex( shapeIndex )
    {
    }

    SpriteBatchItem* mpSpriteBatchItem;
    b2Vec2          mPoint;
    b2Vec2          mNormal;
    F32             mFraction;
    U32             mShapeIndex;
};

///-----------------------------------------------------------------------------

typedef Vector<SpriteBatchQueryResult> typeSpriteBatchQueryResultVector;

#endif // _SPRITE_BATCH_QUERY_RESULT_H_