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

#include "graphics/dgl.h"
#include "2d/scene/DebugDraw.h"

// Debug Profiling.
#include "debug/profiler.h"

//-----------------------------------------------------------------------------

void DebugDraw::DrawAABB( const b2AABB& aabb, const ColorF& color )
{
    // Debug Profiling.
    PROFILE_SCOPE(DebugDraw_DrawAABB);

    // Calculate AABB vertices.
    b2Vec2 aabbVertex[4];
    aabbVertex[0].Set(aabb.lowerBound.x, aabb.lowerBound.y);
    aabbVertex[1].Set(aabb.upperBound.x, aabb.lowerBound.y);
    aabbVertex[2].Set(aabb.upperBound.x, aabb.upperBound.y);
    aabbVertex[3].Set(aabb.lowerBound.x, aabb.upperBound.y);

    DrawPolygon( aabbVertex, 4, color );
}

//-----------------------------------------------------------------------------

void DebugDraw::DrawOOBB( const b2Vec2* pOOBB, const ColorF& color )
{
    // Debug Profiling.
    PROFILE_SCOPE(DebugDraw_DrawOOBB);

    DrawPolygon( pOOBB, 4, color );
}

//-----------------------------------------------------------------------------

void DebugDraw::DrawAsleep( const b2Vec2* pOOBB, const ColorF& color )
{
    // Debug Profiling.
    PROFILE_SCOPE(DebugDraw_DrawAsleep);

    DrawSegment( pOOBB[0], pOOBB[2], color );
    DrawSegment( pOOBB[1], pOOBB[3], color );
}

//-----------------------------------------------------------------------------

void DebugDraw::DrawCollisionShapes( const b2Transform& xf, b2Body* pBody )
{
    // Debug Profiling.
    PROFILE_SCOPE(DebugDraw_DrawCollisionShapes);

    // Iterate fixtures.
    for ( b2Fixture* pFixture = pBody->GetFixtureList(); pFixture; pFixture = pFixture->GetNext() )
    {
        // Inactive fixture.
        if ( pBody->IsActive() == false )
        {
            DrawShape(pFixture, xf, ColorF(0.5f, 0.5f, 0.5f));
        }
        // Active static fixture.
        else if ( pBody->GetType() == b2_staticBody )
        {
            DrawShape(pFixture, xf, ColorF(0.5f, 0.9f, 0.5f));
        }
        // Active kinematic fixture.
        else if ( pBody->GetType() == b2_kinematicBody )
        {
            DrawShape(pFixture, xf, ColorF(0.5f, 0.5f, 0.9f));
        }
        // Active, asleep dynamic fixture.
        else if ( pBody->IsAwake() == false )
        {
            DrawShape(pFixture, xf, ColorF(0.6f, 0.6f, 0.2f));
        }
        // Active, awake dynamic fixture.
        else
        {
            DrawShape(pFixture, xf, ColorF(0.9f, 0.9f, 0.2f));
        }
    }
}

//-----------------------------------------------------------------------------

void DebugDraw::DrawSortPoint( const b2Vec2& worldPosition, const b2Vec2& size, const b2Vec2& localSortPoint )
{
    // Debug Profiling.
    PROFILE_SCOPE(DebugDraw_DrawSortPoint);

    // Calculate sort point marker size based upon the object size.
    const b2Vec2 halfSize( size.x * 0.5f, size.y * 0.5f );
    const b2Vec2 objectSize( size.x * 0.2f, size.y * 0.2f );
    const F32 markerSize = getMin(objectSize.x, objectSize.y);

    // Calculate world sort point.
    b2Vec2 worldPoint( worldPosition.x + (localSortPoint.x  * halfSize.x), worldPosition.y + (localSortPoint.y  * halfSize.y) );

    // Calculate The Sort Point "X". 
    b2Vec2 worldSortPoint[4];
    worldSortPoint[0].Set( worldPoint.x - markerSize, worldPoint.y - markerSize );
    worldSortPoint[1].Set( worldPoint.x + markerSize, worldPoint.y + markerSize );
    worldSortPoint[2].Set( worldPoint.x - markerSize, worldPoint.y + markerSize );
    worldSortPoint[3].Set( worldPoint.x + markerSize, worldPoint.y - markerSize );

    DrawSegment( worldSortPoint[0], worldSortPoint[1], ColorF( 0.0f, 1.0f, 0.8f ) );
    DrawSegment( worldSortPoint[2], worldSortPoint[3], ColorF( 0.0f, 1.0f, 0.8f ) );
}

//-----------------------------------------------------------------------------

void DebugDraw::DrawShape( b2Fixture* fixture, const b2Transform& xf, const ColorF& color )
{
    // Debug Profiling.
    PROFILE_SCOPE(DebugDraw_DrawShape);

    switch (fixture->GetType())
    {
    case b2Shape::e_circle:
        {
            b2CircleShape* circle = (b2CircleShape*)fixture->GetShape();

            b2Vec2 center = b2Mul(xf, circle->m_p);
            float32 radius = circle->m_radius;
            b2Vec2 axis = b2Mul(xf.q, b2Vec2(0.0f, 1.0f));

            DrawSolidCircle(center, radius, axis, color);
        }
        break;

    case b2Shape::e_edge:
        {
            b2EdgeShape* edge = (b2EdgeShape*)fixture->GetShape();
            b2Vec2 v1 = b2Mul(xf, edge->m_vertex1);
            b2Vec2 v2 = b2Mul(xf, edge->m_vertex2);
            DrawSegment(v1, v2, color);
        }
        break;

    case b2Shape::e_chain:
        {
            b2ChainShape* chain = (b2ChainShape*)fixture->GetShape();
            int32 count = chain->m_count;
            const b2Vec2* vertices = chain->m_vertices;

            b2Vec2 v1 = b2Mul(xf, vertices[0]);
            for (int32 i = 1; i < count; ++i)
            {
                b2Vec2 v2 = b2Mul(xf, vertices[i]);
                DrawSegment(v1, v2, color);
                DrawCircle(v1, 0.05f, color);
                v1 = v2;
            }
        }
        break;

    case b2Shape::e_polygon:
        {
            b2PolygonShape* poly = (b2PolygonShape*)fixture->GetShape();
            int32 vertexCount = poly->m_count;
            b2Assert(vertexCount <= b2_maxPolygonVertices);
            b2Vec2 vertices[b2_maxPolygonVertices];

            for (int32 i = 0; i < vertexCount; ++i)
            {
                vertices[i] = b2Mul(xf, poly->m_vertices[i]);
            }

            DrawSolidPolygon(vertices, vertexCount, color);
        }
        break;
            
    default:
        break;
    }
}

//-----------------------------------------------------------------------------

void DebugDraw::DrawJoints( b2World* pWorld )
{
    // Debug Profiling.
    PROFILE_SCOPE(DebugDraw_Joints);

    // Draw all joints in world.
    for ( b2Joint* pJoint = pWorld->GetJointList(); pJoint; pJoint = pJoint->GetNext() )
    {
        // Fetch joint configuration.
        b2Body* pBodyA = pJoint->GetBodyA();
        b2Body* pBodyB = pJoint->GetBodyB();
        const b2Transform& xf1 = pBodyA->GetTransform();
        const b2Transform& xf2 = pBodyB->GetTransform();
        b2Vec2 x1 = xf1.p;
        b2Vec2 x2 = xf2.p;
        b2Vec2 p1 = pJoint->GetAnchorA();
        b2Vec2 p2 = pJoint->GetAnchorB();

        ColorF color( 0.5f, 0.8f, 0.8f );

        switch ( pJoint->GetType() )
        {
            // Distance joint.
            case e_distanceJoint:
                DrawSegment(p1, p2, color);
                break;

            // Pullet joint.
            case e_pulleyJoint:
                {
                    b2PulleyJoint* pPulley = (b2PulleyJoint*)pJoint;
                    b2Vec2 s1 = pPulley->GetGroundAnchorA();
                    b2Vec2 s2 = pPulley->GetGroundAnchorB();
                    DrawSegment( s1, p1, color );
                    DrawSegment( s2, p2, color );
                    DrawSegment( s1, s2, color );
                }
                break;
                
            case e_mouseJoint:
                {
                    b2MouseJoint* pMouseJoint = (b2MouseJoint*)pJoint;
                    
                    b2Vec2 p1 = pBodyB->GetPosition();
                    p1 = pMouseJoint->GetAnchorB();
                    b2Vec2 p2 = pMouseJoint->GetTarget();
                    DrawSegment( p1, p2, color );
                }
                break;

            // All other joints.
            default:
                DrawSegment( x1, p1, color );
                DrawSegment( p1, p2, color );
                DrawSegment( x2, p2, color );
        }
    }
}

//-----------------------------------------------------------------------------

void DebugDraw::DrawPolygon( const b2Vec2* vertices, int32 vertexCount, const ColorF& color )
{
    // Debug Profiling.
    PROFILE_SCOPE(DebugDraw_DrawPolygon);


    DGL->SetColorF(color.red, color.green, color.blue, 1.0f);
    Vector<F32> verts;
    for (U32 i = 0; i < vertexCount; i++)
    {
       verts.push_back(vertices[i].x);
       verts.push_back(vertices[i].y);
    }
    DGL->SetVertexPoint(2, 0, verts.address());
    DGL->EnableClientState(DGLCSVertexArray);
    DGL->DrawArrays(DGLLineLoop, 0, vertexCount);
    DGL->DisableClientState(DGLCSVertexArray);

}

//-----------------------------------------------------------------------------

void DebugDraw::DrawSolidPolygon( const b2Vec2* vertices, int32 vertexCount, const ColorF& color )
{
    // Debug Profiling.
    PROFILE_SCOPE(DebugDraw_DrawSolidPolygon);

    DGL->EnableState(DGLRSBlend);
    DGL->setBlendFunc(DGLBlendSrcAlpha, DGLBlendInvSrcAlpha);
    DGL->SetColorF(0.5f * color.red, 0.5f * color.green, 0.5f * color.blue, 0.15f);
    Vector<F32> verts;
    for (U32 i = 0; i < vertexCount; i++)
    {
       verts.push_back(vertices[i].x);
       verts.push_back(vertices[i].y);
    }
    DGL->SetVertexPoint(2, 0, verts.address());
    DGL->EnableClientState(DGLCSVertexArray);
    DGL->DrawArrays(DGLTriangleFan, 0, vertexCount);
    DGL->DisableState(DGLRSBlend);

    DGL->SetColorF(color.red, color.green, color.blue, 1.0f);
    DGL->SetVertexPoint(2, 0, verts.address());
    DGL->DrawArrays(DGLLineLoop, 0, vertexCount);
    DGL->DisableClientState(DGLCSVertexArray);
}

//-----------------------------------------------------------------------------

void DebugDraw::DrawCircle( const b2Vec2& center, float32 radius, const ColorF& color )
{
    // Debug Profiling.
    PROFILE_SCOPE(DebugDraw_DrawCircle);

    const float32 k_segments = 16.0f;
    const float32 k_increment = 2.0f * b2_pi / k_segments;
    float32 theta = 0.0f;
    DGL->SetColorF(color.red, color.green, color.blue, 1.0);
    Vector<F32> verts;
    for (U32 n = 0; n < k_segments; n++)
    {
       b2Vec2 v = center + radius * b2Vec2(mCos(theta), mSin(theta));

       verts.push_back(v.x);
       verts.push_back(v.y);

       theta += k_increment;
    }
    DGL->EnableClientState(DGLCSVertexArray);
    DGL->SetVertexPoint(2, 0, verts.address());
    DGL->DrawArrays(DGLLineLoop, 0, k_segments);
    DGL->DisableClientState(DGLCSVertexArray);
}
    
//-----------------------------------------------------------------------------

void DebugDraw::DrawSolidCircle( const b2Vec2& center, float32 radius, const b2Vec2& axis, const ColorF& color )
{
    // Debug Profiling.
    PROFILE_SCOPE(DebugDraw_DrawSolidCircle);

    const float32 k_segments = 12.0f;
    const float32 k_increment = 2.0f * b2_pi / k_segments;
    float32 theta = 0.0f;
    DGL->EnableState(DGLRSBlend);
    DGL->setBlendFunc(DGLBlendSrcAlpha, DGLBlendInvSrcAlpha);
    DGL->SetColorF(0.5f * color.red, 0.5f * color.green, 0.5f * color.blue, 0.15f);
    
    Vector<F32> verts;
    for (U32 n = 0; n < k_segments; n++)
    {
       b2Vec2 v = center + radius * b2Vec2(mCos(theta), mSin(theta));

       verts.push_back(v.x);
       verts.push_back(v.y);

       theta += k_increment;
    }

    DGL->SetVertexPoint(2, 0, verts.address());
    DGL->EnableClientState(DGLCSVertexArray);
    DGL->DrawArrays(DGLTriangleFan, 0, k_segments);
    DGL->DisableState(DGLRSBlend);

    DGL->SetColorF(color.red, color.green, color.blue, 1.0f);
    DGL->SetVertexPoint(2, 0, verts.address());
    DGL->DrawArrays(DGLLineLoop, 0, k_segments);
    DGL->DisableClientState(DGLCSVertexArray);
}
    
//-----------------------------------------------------------------------------

void DebugDraw::DrawSegment( const b2Vec2& p1, const b2Vec2& p2, const ColorF& color )
{

    DGL->SetColorF(color.red, color.green, color.blue, 1.0f);

    F32 verts[]{
       p1.x,p1.y,
       p2.x,p2.y,
    };

    DGL->EnableClientState(DGLCSVertexArray);
    DGL->SetVertexPoint(2, 0, verts);
    DGL->DrawArrays(DGLLineList, 0, 4);
    DGL->DisableClientState(DGLCSVertexArray);

}

//-----------------------------------------------------------------------------

void DebugDraw::DrawTransform( const b2Transform& xf )
{
    b2Vec2 p1 = xf.p, p2;
    const float32 k_axisScale = 0.4f;

    DGL->SetColorF(1.0f, 0.0f, 0.0f, 1.0f);
    p2 = p1 + k_axisScale * xf.q.GetXAxis();
    F32 verts[]{
       p1.x,p1.y,
       p2.x,p2.y,
    };

    DGL->EnableClientState(DGLCSVertexArray);
    DGL->SetVertexPoint(2, 0, verts);
    DGL->DrawArrays(DGLLineList, 0, 4);
    DGL->SetColorF(0.0f, 1.0f, 0.0f, 1.0f);
    p2 = p1 + k_axisScale * xf.q.GetXAxis();
    F32 verts2[]{
       p1.x,p1.y,
       p2.x,p2.y,
    };
    DGL->SetVertexPoint(2, 0, verts2);
    DGL->DrawArrays(DGLLineList, 0, 4);
    DGL->DisableClientState(DGLCSVertexArray);
}

//-----------------------------------------------------------------------------

void DebugDraw::DrawPoint( const b2Vec2& p, float32 size, const ColorF& color )
{
    DGL->SetColorF(color.red, color.green, color.blue, 1.0f);
    F32 verts[]{
       p.x,p.y,
    };
    DGL->EnableClientState(DGLCSVertexArray);
    DGL->SetVertexPoint(2, 0, verts);
    DGL->DrawArrays(DGLPointList, 0, 2);
    DGL->DisableClientState(DGLCSVertexArray);
}

