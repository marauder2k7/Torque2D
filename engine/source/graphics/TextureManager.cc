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

#include "graphics/TextureManager.h"

#include "platform/platformAssert.h"
#include "graphics/dgl.h"
#include "platform/platform.h"
#include "collection/vector.h"
#include "io/resource/resourceManager.h"
#include "graphics/gBitmap.h"
#include "console/console.h"
#include "console/consoleInternal.h"
#include "console/consoleTypes.h"
#include "memory/safeDelete.h"
#include "math/mMath.h"

#include "TextureManager_ScriptBinding.h"

//---------------------------------------------------------------------------------------------------------------------

S32 TextureManager::mMasterTextureKeyIndex = 0;
TextureManager::ManagerState TextureManager::mManagerState = TextureManager::NotInitialized; 
bool TextureManager::mDGLRender = true;
bool TextureManager::mForce16BitTexture = false;
bool TextureManager::mAllowTextureCompression = false;
bool TextureManager::mDisableTextureSubImageUpdates = false;
//GLenum TextureManager::mTextureCompressionHint = GL_FASTEST;
S32 TextureManager::mBitmapResidentSize = 0;
S32 TextureManager::mTextureResidentSize = 0;
S32 TextureManager::mTextureResidentWasteSize = 0;
S32 TextureManager::mTextureResidentCount = 0;

//---------------------------------------------------------------------------------------------------------------------

#ifdef TORQUE_OS_IOS
#define EXT_ARRAY_SIZE 4
    static const char* extArray[EXT_ARRAY_SIZE] = { "", ".pvr", ".jpg", ".png"};
#else
struct Forced16BitMapping
{
   DGLFormat wanted;
   DGLFormat forced;
   bool   end;
};

Forced16BitMapping sg16BitMappings[] =
{
    { DGLFormatR8G8B8,  DGLFormatR5G6B5,  false },
    { DGLFormatR8G8B8A8, DGLFormatR4G4B4A4, false },
    { DGLFormat_FIRST, DGLFormat_FIRST, true }
};
#define EXT_ARRAY_SIZE 3
static const char* extArray[EXT_ARRAY_SIZE] = { "", ".jpg", ".png"};
#endif

//---------------------------------------------------------------------------------------------------------------------

struct EventCallbackEntry
{
    TextureManager::TextureEventCallback callback;
    void *               userData;
    U32                  key;
};

static U32                        sgCurrCallbackKey = 0;

static Vector<EventCallbackEntry> sgEventCallbacks(__FILE__, __LINE__);

//--------------------------------------------------------------------------------------------------------------------

U32 TextureManager::registerEventCallback(TextureEventCallback callback, void *userData)
{
    sgEventCallbacks.increment();
    sgEventCallbacks.last().callback = callback;
    sgEventCallbacks.last().userData = userData;
    sgEventCallbacks.last().key      = sgCurrCallbackKey++;

    return sgEventCallbacks.last().key;
}

//--------------------------------------------------------------------------------------------------------------------

void TextureManager::unregisterEventCallback(const U32 callbackKey)
{
    for (S32 i = 0; i < sgEventCallbacks.size(); i++)
    {
        if (sgEventCallbacks[i].key == callbackKey)
        {
            sgEventCallbacks.erase(i);
            return;
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------

void TextureManager::postTextureEvent(const TextureEventCode eventCode)
{
    for (S32 i = 0; i < sgEventCallbacks.size(); i++)
    {
        (sgEventCallbacks[i].callback)(eventCode, sgEventCallbacks[i].userData);
    }
}

//--------------------------------------------------------------------------------------------------------------------

U8 *getLuminanceAlphaBits(GBitmap *bmp)
{
   U8 *data = new U8[bmp->getWidth() * bmp->getHeight() * 2];
   
   S32 w = bmp->getWidth();
   S32 h = bmp->getHeight();
   U8 *src = bmp->getAddress(0, 0);
   U8 *dest = data;
   for (int y=0; y<h; y++)
   {
      for (int x=0; x<w; x++)
      {
         *dest++ = 255;
         *dest++ = *src++;
      }
   }
   
   return data;
}

//--------------------------------------------------------------------------------------------------------------------

void TextureManager::create()
{
    AssertISV(mManagerState == NotInitialized, "TextureManager::create() - already created!");

    TextureDictionary::create();

    Con::addVariable("$pref::OpenGL::force16BitTexture", TypeBool, &TextureManager::mForce16BitTexture);
    Con::addVariable("$pref::OpenGL::allowTextureCompression", TypeBool, &TextureManager::mAllowTextureCompression);
    Con::addVariable("$pref::OpenGL::disableTextureSubImageUpdates", TypeBool, &TextureManager::mDisableTextureSubImageUpdates);

    // Flag as alive.
    mManagerState = Alive;
}

//--------------------------------------------------------------------------------------------------------------------

void TextureManager::destroy()
{
    AssertISV(mManagerState != NotInitialized, "TextureManager::destroy - nothing to destroy!");

    // Destroy the texture dictionary.
    TextureDictionary::destroy();

    // Reset state.
    mBitmapResidentSize = 0;
    mTextureResidentSize = 0;
    mTextureResidentWasteSize = 0;
    mTextureResidentCount = 0;
    mMasterTextureKeyIndex = 0;

    // Flag as not initialized.
    mManagerState = NotInitialized;
}

//--------------------------------------------------------------------------------------------------------------------

void TextureManager::killManager()
{
    // Finish if already dead.
    if ( mManagerState == Dead )
        return;

    // Flag as dead.
    mManagerState = Dead;

    // Post zombie event.
    postTextureEvent(BeginZombification);

    Vector<U32> deleteNames(4096);

    TextureObject* probe = TextureDictionary::TextureObjectChain;
    while (probe) 
    {
        if (probe->mGLTextureName != 0)
        {
            deleteNames.push_back(probe->mGLTextureName);
        }
        probe->mGLTextureName = 0;
        
        // Adjust metrics.
        mTextureResidentCount--;
        mTextureResidentSize -= probe->mTextureResidentSize;
        probe->mTextureResidentSize = 0;
        mTextureResidentWasteSize -= probe->mTextureResidentWasteSize;
        probe->mTextureResidentWasteSize = 0;

        probe = probe->next;
    }

    // Delete all textures.
    DGL->DeleteTextures(deleteNames.size(), deleteNames.address());
}

//--------------------------------------------------------------------------------------------------------------------

void TextureManager::resurrectManager( void )
{
    // Finish if not dead.
    if (mManagerState != Dead)
        return;

    // Flag as resurrecting.
    mManagerState = Resurrecting;

    // Post begin resurrection event.
    postTextureEvent(BeginResurrection);

    // Resurrect textures.
    TextureObject* probe = TextureDictionary::TextureObjectChain;
    while (probe) 
    {
        switch( probe->mHandleType )
        {
            case TextureHandle::BitmapTexture:
                {
                    // Sanity!
                    AssertISV( probe->mTextureKey != NULL && probe->mTextureKey != StringTable->EmptyString, "Encountered a bitmap texture that didn't specify its bitmap." );

                    // Load the bitmap.
                    GBitmap* pBitmap = loadBitmap( probe->mTextureKey );

                    // Sanity!
                    AssertISV(pBitmap != NULL, "Error resurrecting the texture cache.\n""Possible cause: a bitmap was deleted during the course of gameplay.");

                    // Register texture.
                    TextureObject* pTextureObject;
                    pTextureObject = registerTexture(probe->mTextureKey, pBitmap, probe->mHandleType, probe->mClamp);

                    // Sanity!
                    AssertFatal(pTextureObject == probe, "A new texture was returned during resurrection.");

                } break;

            case TextureHandle::BitmapKeepTexture:
                {
                    // Sanity!
                    AssertISV( probe->mpBitmap != NULL, "Encountered no bitmap for a texture that should keep it." );

                    // Create texture.
                    createGLName(probe);

                } break;

            default:
                // Sanity!
                AssertISV( false, "Unknown texture type encountered during texture resurrection." );
        }

        // Next texture.
        probe = probe->next;
    }

    // Post end resurrection event.
    postTextureEvent(EndResurrection);

    // Flag as alive.
    mManagerState = Alive;
}

//--------------------------------------------------------------------------------------------------------------------

void TextureManager::flush()
{
    killManager();
    resurrectManager();
}

//--------------------------------------------------------------------------------------------------------------------

StringTableEntry TextureManager::getUniqueTextureKey( void )
{
    char textureKeyBuffer[32];
    dSprintf( textureKeyBuffer, sizeof(textureKeyBuffer), "GeneratedKey_%d", mMasterTextureKeyIndex++ );
    return StringTable->insert( textureKeyBuffer );
}

//--------------------------------------------------------------------------------------------------------------------

GBitmap* TextureManager::createPowerOfTwoBitmap(GBitmap* pBitmap)
{    

    // Finish if already a power-of-two in dimension.
    if (isPow2(pBitmap->getWidth()) && isPow2(pBitmap->getHeight()))
        return pBitmap;


    U32 width = pBitmap->getWidth();
    U32 height = pBitmap->getHeight();

    U32 newWidth  = getNextPow2(pBitmap->getWidth());
    U32 newHeight = getNextPow2(pBitmap->getHeight());

    GBitmap* pReturn = new GBitmap(newWidth, newHeight, false, pBitmap->getFormat());

    for (U32 i = 0; i < height; i++) 
    {
        U8*       pDest = (U8*)pReturn->getAddress(0, i);
        const U8* pSrc  = (const U8*)pBitmap->getAddress(0, i);

        dMemcpy(pDest, pSrc, width * pBitmap->bytesPerPixel);

        pDest += width * pBitmap->bytesPerPixel;
        // set the src pixel to the last pixel in the row
        const U8 *pSrcPixel = pDest - pBitmap->bytesPerPixel;

        for(U32 j = width; j < newWidth; j++)
            for(U32 k = 0; k < pBitmap->bytesPerPixel; k++)
                *pDest++ = pSrcPixel[k];
    }

    for(U32 i = height; i < newHeight; i++)
    {
        U8* pDest = (U8*)pReturn->getAddress(0, i);
        U8* pSrc = (U8*)pReturn->getAddress(0, height-1);
        dMemcpy(pDest, pSrc, newWidth * pBitmap->bytesPerPixel);
    }

    return pReturn;
}

//---------------------------------------------------------------------------------------------------------------------

void TextureManager::freeTexture( TextureObject* pTextureObject )
{
    if((mDGLRender || mManagerState == Resurrecting) && pTextureObject->mGLTextureName)
    {
        DGL->DeleteTextures(1, (const U32*)&pTextureObject->mGLTextureName);

        // Adjust metrics.
        mTextureResidentCount--;
        mTextureResidentSize -= pTextureObject->mTextureResidentSize;
        pTextureObject->mTextureResidentSize = 0;
        mTextureResidentWasteSize -= pTextureObject->mTextureResidentWasteSize;
        pTextureObject->mTextureResidentWasteSize = 0;
    }

    if ( pTextureObject->mpBitmap != NULL )
    {
        SAFE_DELETE( pTextureObject->mpBitmap );
        mBitmapResidentSize -= pTextureObject->mBitmapResidentSize;
        pTextureObject->mBitmapResidentSize = 0;
    }

    TextureDictionary::remove(pTextureObject);
    SAFE_DELETE( pTextureObject );
}

//---------------------------------------------------------------------------------------------------------------------

void TextureManager::getSourceDestByteFormat(GBitmap *pBitmap, DGLFormat *sourceFormat )
{
    U32 byteSize = 1;
    switch (pBitmap->getFormat())
    {
#if defined(TORQUE_OS_IOS)
    case GBitmap::PVR2:
        *sourceFormat = GL_RGB;
        *byteFormat = GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG;
        byteSize = 1; // Incorrect but assume worst case.
        break;
    case GBitmap::PVR2A:
        *sourceFormat = GL_RGBA;
        *byteFormat = GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;
        byteSize = 1; // Incorrect but assume worst case.
        break;
    case GBitmap::PVR4:
        *sourceFormat = GL_RGB;
        *byteFormat = GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG;
        byteSize = 1; // Incorrect but assume worst case.
        break;
    case GBitmap::PVR4A:
        *sourceFormat = GL_RGBA;
        *byteFormat = GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
        byteSize = 1; // Incorrect but assume worst case.
        break;
    }
    *destFormat = *sourceFormat;
    *texelSize = byteSize;
    return;
#else   
    case DGLFormatA8:
       *sourceFormat = DGLFormatA8;
       break;
    case DGLFormatL8:
        *sourceFormat = DGLFormatL8;
        break;
    case DGLFormatA4L4:
        *sourceFormat = DGLFormatA4L4;
        break;
    case DGLFormatR8G8B8:
        *sourceFormat = DGLFormatR8G8B8;
        break;
    case DGLFormatR8G8B8A8:
        *sourceFormat = DGLFormatR8G8B8A8;
        break;
    

    case DGLFormatR5G5B5A1:
    case DGLFormatR5G5B5X1:
#if defined(TORQUE_BIG_ENDIAN)
        *sourceFormat = GL_BGRA_EXT;
#else
        *sourceFormat = DGLFormatR5G5B5A1;
#endif
        break;
    };

    if (TextureManager::mForce16BitTexture)
    {
        for (U32 i = 0; sg16BitMappings[i].end != true; i++)
        {
            if (*sourceFormat == sg16BitMappings[i].wanted)
            {
                *sourceFormat = sg16BitMappings[i].forced;
                return;
            }
        }
    }
#endif // defined(TORQUE_OS_IOS)
}

//--------------------------------------------------------------------------------------------------------------------

U16* TextureManager::create16BitBitmap( GBitmap *pDL, U8 *in_source8, DGLFormat alpha_info, DGLFormat *GLFormat, U32 width, U32 height )
{
    //PUAP -Mat make 16 bit
    U16 *texture_data = new U16[width * height];
    U16 *dest = texture_data;
    U32 *source = (U32*)in_source8;
    //since the pointer is 4 bytes, multiply by the number of bytes per pixel over 4
    U32 spanInBytes = (U32)((width * height) * (pDL->bytesPerPixel / 4.0f));
    U32 *source_end = source + spanInBytes;

    switch (alpha_info) {
        case DGLFormatA8: //ALPHA_TRANSPARENT:
            while (source != source_end) {
                U32 color = *source++;
                *dest++ = ((color & 0xF8) << 8) | ((color & 0xF800) >> 5) | ((color & 0xF80000) >> 18) | (color >> 31);
            }
            *GLFormat = DGLFormatR5G5B5A1;
            break;
            case DGLFormatB8G8R8A8://ALPHA_BLEND
            while (source != source_end) {
                U32 color = *source++;
                *dest++ = ((color & 0xF0) << 8) | ((color & 0xF000) >> 4) | ((color & 0xF00000) >> 16) | ((color & 0xF0000000) >> 28);
            }
            *GLFormat = DGLFormatR4G4B4A4;
        break;

        default://ALPHA_NONE
            U8 *source8 = (U8*)source;
            //32 bytes per address, snce we are casting to U8 we need 4 times as many
            U8 *end8 = source8 + (U32)(spanInBytes*4);
            while (source8 < end8) {
                U16 red = (U16)*source8;
                source8++;
                U16 green = (U16)*source8;
                source8++;
                U16 blue = (U16)*source8;
                source8++;
                //now color should be == RR GG BB 00
                *dest = ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | ((blue & 0xF8) >> 3);
                dest++;
            }
            *GLFormat = DGLFormatR5G6B5;
        break;
    }
    return texture_data;
}


//-----------------------------------------------------------------------------

void TextureManager::refresh( TextureObject* pTextureObject )
{
    // Finish if refresh not appropriate.
    if (!(mDGLRender || mManagerState == Resurrecting))
        return;

    // Sanity!
    AssertISV( pTextureObject->mGLTextureName != 0, "Refreshing texture but no texture created." );
    AssertISV( pTextureObject->mpBitmap != 0, "Refreshing texture but no bitmap available." );

    // Fetch bitmaps.
    GBitmap* pSourceBitmap = pTextureObject->mpBitmap;
    GBitmap* pNewBitmap = createPowerOfTwoBitmap(pSourceBitmap);
   
    U8 *bits = (U8*)pNewBitmap->getBits();
    U8 *lumBits = NULL;

    // Fetch source/dest formats.
    DGLFormat sourceFormat;
   
#if defined(TORQUE_OS_EMSCRIPTEN)
    if (pSourceBitmap->getFormat() == GBitmap::Alpha)
    {
        // special case: alpha should be converted to luminancealpha
        bits = lumBits = getLuminanceAlphaBits(pNewBitmap);
        sourceFormat = destFormat = GL_LUMINANCE_ALPHA;
        byteFormat = GL_UNSIGNED_BYTE;
        texelSize = 2;
    }
    else
#endif
    {
        getSourceDestByteFormat(pSourceBitmap, &sourceFormat);
    }

#if defined(TORQUE_OS_IOS)
    bool isCompressed = (pNewBitmap->getFormat() >= GBitmap::PVR2) && (pNewBitmap->getFormat() <= GBitmap::PVR4A);
#endif

#if defined(TORQUE_OS_IOS)
    if (isCompressed) {
        switch (pNewBitmap->getFormat()) {
            case GBitmap::PVR2:
            case GBitmap::PVR2A:
                glCompressedTexImage2D(GL_TEXTURE_2D, 0, byteFormat,
                    pNewBitmap->getWidth(), pNewBitmap->getHeight(), 0, (getMax((int)pNewBitmap->getWidth(),16) * getMax((int)pNewBitmap->getHeight(), 8) * 2 + 7) / 8, pNewBitmap->getBits() );
                break;
            case GBitmap::PVR4:
            case GBitmap::PVR4A:
                glCompressedTexImage2D(GL_TEXTURE_2D, 0, byteFormat,
                    pNewBitmap->getWidth(), pNewBitmap->getHeight(), 0, (getMax((int)pNewBitmap->getWidth(),8) * getMax((int)pNewBitmap->getHeight(), 8) * 4 + 7) / 8, pNewBitmap->getBits() );
                break;
            default:
            // already tested for range of values, so default is just to keep the compiler happy!
            break;
        }
    } else 
#endif

    // Bind texture.
    //glBindTexture( GL_TEXTURE_2D, pTextureObject->mGLTextureName );

    DGL->BindTexture(pTextureObject->mGLTextureName);

    // Are we forcing to 16-bit?
    if( pSourceBitmap->mForce16Bit )
    {
        // Yes, so generate a 16-bit texture.
        DGLFormat GLformat;

        U16* pBitmap16 = create16BitBitmap( pNewBitmap, pNewBitmap->getWritableBits(), pNewBitmap->getFormat(), 
                                                &GLformat,
                                                pNewBitmap->getWidth(), pNewBitmap->getHeight() );

        /*glTexImage2D(GL_TEXTURE_2D, 
                        0,
                        GLformat,
                        pNewBitmap->getWidth(), pNewBitmap->getHeight(), 
                        0,
                        GLformat, 
                        GLdata_type,
                        pBitmap16
                    );*/

        DGL->UploadTexture16(0,
           GLformat,
           pNewBitmap->getWidth(), pNewBitmap->getHeight(),
           0,
           pBitmap16);

        //copy new texture_data into pBits
        delete [] pBitmap16;
    }
    else
    {
        // No, so upload as-is.
        DGL->UploadTexture(0,
            sourceFormat,
            pNewBitmap->getWidth(), pNewBitmap->getHeight(),
            0,
            bits);
    }

    const DGLTextureFilter filter = pTextureObject->getFilter();
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);

    DGL->SetTextureParam(DGLTextureParamMagFilter, filter);
    DGL->SetTextureParam(DGLTextureParamMinFilter, filter);

    DGLTextureFilter glClamp;
    if ( pTextureObject->getClamp() )
        glClamp = DGL->smEdgeClamp ? DGLClampEdge : DGLClampDef;
    else
        glClamp = DGLClampRepeat;

    //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glClamp );
    //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glClamp );
    DGL->SetTextureParam(DGLTextureParamWrapS, glClamp);
    DGL->SetTextureParam(DGLTextureParamWrapT, glClamp);

    if(pNewBitmap != pSourceBitmap)
    {
        delete pNewBitmap;
    }
   
    if (lumBits)
        delete[] lumBits;
}

//--------------------------------------------------------------------------------------------------------------------

void TextureManager::refresh( const char *textureName )
{
    // Finish if no texture name specified.
    AssertFatal( textureName != NULL, "Texture Manager:  Cannot refresh a NULL texture name." );

    // Find the texture object.
    TextureObject* pTextureObject = TextureDictionary::find( textureName );

    // Finish if no texture for this texture name.
    if ( pTextureObject == NULL )
        return;

    // Finish if the texture object is a kept bitmap.
    if ( pTextureObject->getHandleType() == TextureHandle::BitmapKeepTexture )
        return;

    // Load the bitmap.
    GBitmap* pBitmap = loadBitmap( pTextureObject->mTextureKey );

    // Finish if bitmap could not be loaded.
    if ( pBitmap == NULL )
        return;

    // Register texture.
    TextureObject* pNewTextureObject;
    pNewTextureObject = registerTexture(pTextureObject->mTextureKey, pBitmap, pTextureObject->mHandleType, pTextureObject->mClamp);

    // Sanity!
    AssertFatal(pNewTextureObject == pTextureObject, "A new texture was returned during refresh.");
}

//--------------------------------------------------------------------------------------------------------------------

void TextureManager::createGLName( TextureObject* pTextureObject )
{
    // Finish if not appropriate.
    if (!(mDGLRender || mManagerState == Resurrecting))
        return;

    // Sanity!
    AssertISV( pTextureObject->mHandleType != TextureHandle::InvalidTexture, "Invalid texture type." );
    AssertISV( pTextureObject->mpBitmap != NULL, "Bitmap cannot be NULL." );
    AssertISV( pTextureObject->mGLTextureName == 0, "GL texture name already exists." );

    // Generate texture name.
    DGL->LoadTexture(1, pTextureObject->mGLTextureName);

    // Fetch source/dest formats.
    DGLFormat sourceFormat;
    getSourceDestByteFormat(pTextureObject->mpBitmap, &sourceFormat);

    // Adjust metrics.
    mTextureResidentCount++;
    pTextureObject->mTextureResidentSize = pTextureObject->mTextureWidth * pTextureObject->mTextureHeight;
    mTextureResidentSize += pTextureObject->mTextureResidentSize;
    pTextureObject->mTextureResidentWasteSize = ((pTextureObject->mTextureWidth * pTextureObject->mTextureHeight)-(pTextureObject->mBitmapWidth * pTextureObject->mBitmapHeight));
    mTextureResidentWasteSize += pTextureObject->mTextureResidentWasteSize;

    // Refresh the texture.
    refresh( pTextureObject );
}

//--------------------------------------------------------------------------------------------------------------------

TextureObject* TextureManager::registerTexture(const char* pTextureKey, GBitmap* pNewBitmap, TextureHandle::TextureHandleType type, bool clampToEdge)
{
    // Sanity!
    AssertISV( type != TextureHandle::InvalidTexture, "Invalid texture type." );

    TextureObject* pTextureObject = NULL;

    // Fetch texture key.
    StringTableEntry textureKey = StringTable->insert(pTextureKey);

    if(pTextureKey)
    {
        pTextureObject = TextureDictionary::find(textureKey, type, clampToEdge);
    }

    if( pTextureObject )
    {
        // Remove bitmap if we have a different existing one.
        if ( pTextureObject->mpBitmap != NULL && pTextureObject->mpBitmap != pNewBitmap)
        {
            delete pTextureObject->mpBitmap;
            pTextureObject->mpBitmap = NULL;

            // Adjust metrics.
            mBitmapResidentSize -= pTextureObject->mBitmapResidentSize;
            pTextureObject->mBitmapResidentSize = 0;
        }

        // Remove any texture name.
        if ( pTextureObject->mGLTextureName != 0 )
        {
            DGL->DeleteTextures(1, (const U32*)&pTextureObject->mGLTextureName);
            pTextureObject->mGLTextureName = 0;

            // Adjust metrics.
            mTextureResidentCount--;
            mTextureResidentSize -= pTextureObject->mTextureResidentSize;
            pTextureObject->mTextureResidentSize = 0;
            mTextureResidentWasteSize -= pTextureObject->mTextureResidentWasteSize;
            pTextureObject->mTextureResidentWasteSize = 0;
        }
    }
    else
    {
        // Create new texture object.
        pTextureObject = new TextureObject();
        pTextureObject->mTextureKey = textureKey;
        pTextureObject->mHandleType = type;

        TextureDictionary::insert(pTextureObject);
    }

    if ( pTextureObject->mHandleType == TextureHandle::BitmapKeepTexture && pTextureObject->mpBitmap != pNewBitmap )
    {
        // Adjust metrics.
        pTextureObject->mBitmapResidentSize = pNewBitmap->byteSize;
        mBitmapResidentSize += pTextureObject->mBitmapResidentSize;
    }

    pTextureObject->mpBitmap           = pNewBitmap;
    pTextureObject->mBitmapWidth       = pNewBitmap->getWidth();
    pTextureObject->mBitmapHeight      = pNewBitmap->getHeight();
    pTextureObject->mTextureWidth      = getNextPow2(pNewBitmap->getWidth());
    pTextureObject->mTextureHeight     = getNextPow2(pNewBitmap->getHeight());
    pTextureObject->mClamp             = clampToEdge;

    // Generate a GL texture name if one is not ready.
    if( pTextureObject->mGLTextureName == 0) 
    {
        createGLName(pTextureObject);
    }

    // Delete bitmap if we're not keeping it.
    if ( pTextureObject->mHandleType != TextureHandle::BitmapKeepTexture ) 
    {
        delete pTextureObject->mpBitmap;
        pTextureObject->mpBitmap = NULL;
    }

    return pTextureObject;
}

//--------------------------------------------------------------------------------------------------------------------

TextureObject *TextureManager::loadTexture(const char* pTextureKey, TextureHandle::TextureHandleType type, bool clampToEdge, bool checkOnly, bool force16Bit )
{
    // Sanity!
    AssertISV( type != TextureHandle::InvalidTexture, "Invalid texture type." );

    // Finish if texture key is invalid.
    if( pTextureKey == NULL || *pTextureKey == 0)
        return NULL;

    // Fetch texture key.
    StringTableEntry textureKey = StringTable->insert(pTextureKey);

    TextureObject *ret = TextureDictionary::find(textureKey, type, clampToEdge);

    GBitmap *bmp = NULL;

    if( ret == NULL )
    {
        // Ok, no hit - is it in the current dir? If so then let's grab it
        // and use it.
        bmp = loadBitmap(textureKey, false);

        if(bmp)
        {
            bmp->mForce16Bit = force16Bit;
            return registerTexture(textureKey, bmp, type, clampToEdge);
        }
    }

    if(ret)
        return ret;

    // If we're just checking, fail out so we eventually get around to
    // loading a real bitmap.
    if(checkOnly)
        return NULL;

    // Ok, no success so let's try actually loading a texture.
    bmp = loadBitmap(textureKey);

    if(!bmp)
    {
        Con::warnf("Could not locate texture: %s", textureKey);
        return NULL;
    }
    bmp->mForce16Bit = force16Bit;

    return registerTexture(textureKey, bmp, type, clampToEdge);
}

//--------------------------------------------------------------------------------------------------------------------

GBitmap *TextureManager::loadBitmap( const char* pTextureKey, bool recurse, bool nocompression )
{
    char fileNameBuffer[512];
    Con::expandPath( fileNameBuffer, sizeof(fileNameBuffer), pTextureKey );
    GBitmap *bmp = NULL;

    // Loop through the supported extensions to find the file.
    U32 len = dStrlen(fileNameBuffer);
    for (U32 i = 0; i < EXT_ARRAY_SIZE && bmp == NULL; i++)
    {
#if defined(TORQUE_OS_IOS)
        // check to see if requested no-compression...
        if (nocompression && (dStrncmp( extArray[i], ".pvr", 4 ) == 0)) {
            continue;
        }
#endif
        dStrcpy(fileNameBuffer + len, extArray[i]);

        bmp = (GBitmap*)ResourceManager->loadInstance(fileNameBuffer);

        if ( bmp != NULL && (bmp->getWidth() > MaximumProductSupportedTextureWidth || bmp->getHeight() > MaximumProductSupportedTextureHeight) )
        {
            Con::warnf( "TextureManager::loadBitmap() - Cannot load bitmap '%s' as its dimensions exceed the maximum product-supported texture dimension.", fileNameBuffer );
            delete bmp;
            return NULL;
        }
    }

    return bmp;
}

//--------------------------------------------------------------------------------------------------------------------

void TextureManager::dumpMetrics( void )
{
    S32 textureResidentCount = 0;
    S32 textureResidentSize = 0;
    S32 textureResidentWasteSize = 0;
    S32 bitmapResidentSize = 0;

    Con::printSeparator();
    Con::printBlankLine();
    Con::printf( "Dumping texture manager metrics:" );

    TextureObject* pProbe = TextureDictionary::TextureObjectChain;
    while (pProbe != NULL) 
    {
        bool isTextureResident = false;
        if ( pProbe->mGLTextureName != 0 )
        {
            textureResidentCount++;
            DGL->AreTexturesLoaded( 1, &pProbe->mGLTextureName, &isTextureResident );
        }

        textureResidentSize += pProbe->mTextureResidentSize;
        bitmapResidentSize += pProbe->mBitmapResidentSize;
        textureResidentWasteSize += pProbe->mTextureResidentWasteSize;

        // Info.
        Con::printf( "BitmapArea: (%d-%d), BitmapMemory: %d, TextureArea: (%d-%d), TextureMemory: %d, TextureMemoryWaste=%d, Refs=%d, Resident=%s, Name=%s",
            pProbe->mBitmapWidth,pProbe->mBitmapHeight, pProbe->mBitmapResidentSize,
            pProbe->mTextureWidth, pProbe->mTextureHeight, pProbe->mTextureResidentSize, pProbe->mTextureResidentWasteSize,
            pProbe->mRefCount,
            isTextureResident == 0 ? "NO" : "YES",
            pProbe->mTextureKey );

        pProbe = pProbe->next;
    }

    // Validate metrics.
    const bool textureCountSame = textureResidentCount == mTextureResidentCount;
    const bool textureSizeSame = textureResidentSize == mTextureResidentSize;
    const bool textureWasteSizeSame = textureResidentWasteSize == mTextureResidentWasteSize;
    const bool bitmapSizeSame = bitmapResidentSize == mBitmapResidentSize;
    const bool allMetricsValid = textureCountSame && textureSizeSame && textureWasteSizeSame && bitmapSizeSame;

    // Error if metrics are invalid.
    if ( !allMetricsValid )
        Con::errorf( "Metrics appear to be invalid." );

    // Info.
    Con::printf( "Metrics Totals:" );
    Con::printf( "TextureCount: %d, TextureSize: %d, TextureWasteSize: %d, BitmapSize: %d, ResidentFraction: %g",
        mTextureResidentCount,
        mTextureResidentSize,
        mTextureResidentWasteSize,
        mBitmapResidentSize,
        getResidentFraction() );

    Con::printBlankLine();
    Con::printSeparator();
    Con::printf( "All texture manager metrics are valid." );
    Con::printSeparator();
}

//--------------------------------------------------------------------------------------------------------------------

F32 TextureManager::getResidentFraction()
{
    U32 resident = 0;
    U32 total    = 0;

    Vector<U32> names;

    TextureObject* pProbe = TextureDictionary::TextureObjectChain;
    while (pProbe != NULL) 
    {
        if (pProbe->mGLTextureName != 0) 
        {
            total++;
            names.push_back(pProbe->mGLTextureName);
        }

        pProbe = pProbe->next;
    }

    if (total == 0)
        return 1.0f;

    Vector<bool> isResident;
    isResident.setSize(names.size());

    DGL->AreTexturesLoaded(names.size(), names.address(), isResident.address());
    for (U32 i = 0; i < (U32)names.size(); i++)
        if (isResident[i] == true)
            resident++;

    return (F32(resident) / F32(total));
}
