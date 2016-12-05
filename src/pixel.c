/* The MIT License (MIT)
 *
 * Copyright (c) 2016 Stefano Trettel
 *
 * Software repository: MoonGL, https://github.com/stetre/moongl
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "internal.h"

#define SrcEnum enumBuffer()
#define CheckSrc(L, arg) enumCheck((L), (arg), SrcEnum)
#define PushSrc(L, code) enumPush((L), (code), SrcEnum)

#define MaskBitfield bitfieldBuffer()
#define CheckMask(L, arg, mand) bitfieldCheck((L), (arg), (mand), MaskBitfield)
#define PushMask(L, code) bitfieldPush((L), (code), MaskBitfield)

ENUM_STRINGS(FilterStrings) = {
    "nearest",
    "linear",
    NULL
};
ENUM_CODES(FilterCodes) = {
    GL_NEAREST,
    GL_LINEAR
};
ENUM_T(FilterEnum, FilterStrings, FilterCodes)
#define CheckFilter(L, arg) enumCheck((L), (arg), &FilterEnum)
#define PushFilter(L, code) enumPush((L), (code), &FilterEnum)


ENUM_STRINGS(TargetStrings) = {
    "clamp read color",
    NULL
};
ENUM_CODES(TargetCodes) = {
    GL_CLAMP_READ_COLOR,
};
ENUM_T(TargetEnum, TargetStrings, TargetCodes)
#define CheckTarget(L, arg) enumCheck((L), (arg), &TargetEnum)
#define PushTarget(L, code) enumPush((L), (code), &TargetEnum)

ENUM_STRINGS(ClampStrings) = {
    "true",
    "false",
    "fixed only",
    NULL
};
ENUM_CODES(ClampCodes) = {
    GL_TRUE,
    GL_FALSE,
    GL_FIXED_ONLY,
};
ENUM_T(ClampEnum, ClampStrings, ClampCodes)
#define CheckClamp(L, arg) enumCheck((L), (arg), &ClampEnum)
#define PushClamp(L, code) enumPush((L), (code), &ClampEnum)

enum_t *enumClamp(void)
    { return &ClampEnum; }


ENUM_STRINGS(ImageTargetStrings) = {
    "renderbuffer",
    "1d",
    "2d",
    "3d",
    "1d array",
    "2d array",
    "rectangle",
    "cube map",
    "cube map array",
    "buffer", 
    "2d multisample",
    "2d multisample array",
    NULL
};
ENUM_CODES(ImageTargetCodes) = {
    GL_RENDERBUFFER,
    GL_TEXTURE_1D,
    GL_TEXTURE_2D,
    GL_TEXTURE_3D,
    GL_TEXTURE_1D_ARRAY,
    GL_TEXTURE_2D_ARRAY,
    GL_TEXTURE_RECTANGLE,
    GL_TEXTURE_CUBE_MAP,
    GL_TEXTURE_CUBE_MAP_ARRAY,
    GL_TEXTURE_BUFFER, 
    GL_TEXTURE_2D_MULTISAMPLE,
    GL_TEXTURE_2D_MULTISAMPLE_ARRAY
};
ENUM_T(ImageTargetEnum,ImageTargetStrings, ImageTargetCodes)
#define CheckImageTarget(L, arg) enumCheck((L), (arg), &ImageTargetEnum)

ENUM_STRINGS(PnameStrings) = {
    "unpack swap bytes", 
    "unpack lsb first", 
    "unpack row length", 
    "unpack skip rows", 
    "unpack skip pixels", 
    "unpack alignment",
    "unpack image height",
    "unpack skip images", 
    "unpack compressed block width", 
    "unpack compressed block height", 
    "unpack compressed block depth", 
    "unpack compressed block size", 
    "pack swap bytes", 
    "pack lsb first", 
    "pack row length", 
    "pack skip rows", 
    "pack skip pixels", 
    "pack alignment",
    "pack image height", 
    "pack skip images", 
    "pack compressed block width", 
    "pack compressed block height", 
    "pack compressed block depth", 
    "pack compressed block size", 
    NULL
};
ENUM_CODES(PnameCodes) = {
    GL_UNPACK_SWAP_BYTES, 
    GL_UNPACK_LSB_FIRST, 
    GL_UNPACK_ROW_LENGTH, 
    GL_UNPACK_SKIP_ROWS, 
    GL_UNPACK_SKIP_PIXELS, 
    GL_UNPACK_ALIGNMENT,
    GL_UNPACK_IMAGE_HEIGHT,
    GL_UNPACK_SKIP_IMAGES, 
    GL_UNPACK_COMPRESSED_BLOCK_WIDTH, 
    GL_UNPACK_COMPRESSED_BLOCK_HEIGHT, 
    GL_UNPACK_COMPRESSED_BLOCK_DEPTH, 
    GL_UNPACK_COMPRESSED_BLOCK_SIZE, 
    GL_PACK_SWAP_BYTES, 
    GL_PACK_LSB_FIRST, 
    GL_PACK_ROW_LENGTH, 
    GL_PACK_SKIP_ROWS, 
    GL_PACK_SKIP_PIXELS, 
    GL_PACK_ALIGNMENT,
    GL_PACK_IMAGE_HEIGHT, 
    GL_PACK_SKIP_IMAGES, 
    GL_PACK_COMPRESSED_BLOCK_WIDTH, 
    GL_PACK_COMPRESSED_BLOCK_HEIGHT, 
    GL_PACK_COMPRESSED_BLOCK_DEPTH, 
    GL_PACK_COMPRESSED_BLOCK_SIZE, 
};
ENUM_T(PnameEnum, PnameStrings, PnameCodes)
#define CheckPname(L, arg) enumCheck((L), (arg), &PnameEnum)
#define PushPname(L, code) enumPush((L), (code), &PnameEnum)


static int ReadBuffer(lua_State *L)
/* read_buffer(src)
 * read_buffer(framebuffer, src)
 */
    {
    GLuint framebuffer;
    GLenum src;
    if(lua_isstring(L, 1))
        {
        src = CheckSrc(L, 1);
        glReadBuffer(src);
        }
    else
        {
        framebuffer = luaL_checkinteger(L, 1);
        src = CheckSrc(L, 2);
        glNamedFramebufferReadBuffer(framebuffer, src);
        }
    CheckError(L);
    return 0;
    }

static int ReadPixels(lua_State *L)
    {
    GLint x = luaL_checkinteger(L, 1);
    GLint y = luaL_checkinteger(L, 2);
    GLsizei width = luaL_checkinteger(L, 3);
    GLsizei height = luaL_checkinteger(L, 4);
    GLenum format = checkformat(L, 5);
    GLenum type = checktype(L, 6);
    GLsizei bufsz = width*height*sizeoftype(L, type);
    void *data = Malloc(L, bufsz);
#if 0 /* #ifdef GL_VERSION_4_5 */
    glReadnPixels(x, y, width, height, format, type, bufsz, data);
#else
    glReadPixels(x, y, width, height, format, type, data);
#endif
    CheckErrorFree(L, data);
    lua_pushlstring(L, (char*)data, bufsz);
    Free(L, data);
    return 1;
    }

static int ClampColor(lua_State *L)
    {
    GLenum clamp;
    GLenum target = CheckTarget(L, 1);
    if(lua_isboolean(L, 2))
        clamp = lua_toboolean(L, 2) ? GL_TRUE : GL_FALSE;
    else
        clamp = CheckClamp(L, 2);
    glClampColor(target, clamp);
    CheckError(L);
    return 0;
    }

static int BlitFramebuffer(lua_State *L)
/* blit_framebuffer(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, filter [,bit1, ..])
 * blit_framebuffer(readfb, drawfb, srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, filter [,bit1, ..])
 */

    {
    GLuint rfb, dfb;
    GLint srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1;
    GLbitfield mask;
    GLenum filter;
    int named, arg = 1;
    if(lua_isstring(L, 9)) /* 'filter' */
        named = 0;
    else
        {
        named = 1;
        rfb = luaL_checkinteger(L, arg++);
        dfb = luaL_checkinteger(L, arg++);
        }
    srcX0 = luaL_checkinteger(L, arg++);
    srcY0 = luaL_checkinteger(L, arg++);
    srcX1 = luaL_checkinteger(L, arg++);
    srcY1 = luaL_checkinteger(L, arg++);
    dstX0 = luaL_checkinteger(L, arg++);
    dstY0 = luaL_checkinteger(L, arg++);
    dstX1 = luaL_checkinteger(L, arg++);
    dstY1 = luaL_checkinteger(L, arg++);
    filter = CheckFilter(L, arg++); 
    mask = CheckMask(L, arg, 0);
    if(named==0)
        glBlitFramebuffer(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter);
    else
        glBlitNamedFramebuffer
            (rfb, dfb, srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter);
    CheckError(L);
    return 0;
    }

static int CopyImageSubData(lua_State *L)
    {
    int arg = 1;
    GLuint srcName = luaL_checkinteger(L, arg++);
    GLenum srcTarget = CheckImageTarget(L, arg++);
    GLint srcLevel = luaL_checkinteger(L, arg++);
    GLint srcX = luaL_checkinteger(L, arg++);
    GLint srcY = luaL_checkinteger(L, arg++);
    GLint srcZ = luaL_checkinteger(L, arg++);
    GLuint dstName = luaL_checkinteger(L, arg++);
    GLenum dstTarget = CheckImageTarget(L, arg++);
    GLint dstLevel = luaL_checkinteger(L, arg++);
    GLint dstX = luaL_checkinteger(L, arg++);
    GLint dstY = luaL_checkinteger(L, arg++);
    GLint dstZ = luaL_checkinteger(L, arg++);
    GLsizei srcWidth = luaL_checkinteger(L, arg++);
    GLsizei srcHeight = luaL_checkinteger(L, arg++);
    GLsizei srcDepth = luaL_checkinteger(L, arg++);
    glCopyImageSubData(srcName, srcTarget, srcLevel, srcX, srcY, srcZ, 
                        dstName, dstTarget, dstLevel, dstX, dstY, dstZ, 
                            srcWidth, srcHeight, srcDepth);
    CheckError(L);
    return 0;
    }

static int Storei_(lua_State *L, GLenum pname, int arg, int boolean)
    {
    GLint param;
    if(boolean)
        param = checkboolean(L, arg);
    else
        param = luaL_checkinteger(L, arg);
    glPixelStorei(pname, param);
    CheckError(L);
    return 0;
    }

#define StoreInt(L, pname, arg) Storei_((L),(pname),(arg), 0)
#define StoreBoolean(L, pname, arg) Storei_((L),(pname),(arg), 1)

static int PixelStore(lua_State *L)
    {
    GLenum pname = CheckPname(L, 1);
    switch(pname)
        {
        case GL_UNPACK_SWAP_BYTES: 
        case GL_UNPACK_LSB_FIRST: return StoreBoolean(L, pname, 2);
        case GL_UNPACK_ROW_LENGTH: 
        case GL_UNPACK_SKIP_ROWS: 
        case GL_UNPACK_SKIP_PIXELS: 
        case GL_UNPACK_ALIGNMENT:
        case GL_UNPACK_IMAGE_HEIGHT:
        case GL_UNPACK_SKIP_IMAGES: 
        case GL_UNPACK_COMPRESSED_BLOCK_WIDTH: 
        case GL_UNPACK_COMPRESSED_BLOCK_HEIGHT: 
        case GL_UNPACK_COMPRESSED_BLOCK_DEPTH: 
        case GL_UNPACK_COMPRESSED_BLOCK_SIZE: return StoreInt(L, pname, 2);
        case GL_PACK_SWAP_BYTES: 
        case GL_PACK_LSB_FIRST: return StoreBoolean(L, pname, 2);
        case GL_PACK_ROW_LENGTH: 
        case GL_PACK_SKIP_ROWS: 
        case GL_PACK_SKIP_PIXELS: 
        case GL_PACK_ALIGNMENT:
        case GL_PACK_IMAGE_HEIGHT: 
        case GL_PACK_SKIP_IMAGES: 
        case GL_PACK_COMPRESSED_BLOCK_WIDTH: 
        case GL_PACK_COMPRESSED_BLOCK_HEIGHT: 
        case GL_PACK_COMPRESSED_BLOCK_DEPTH: 
        case GL_PACK_COMPRESSED_BLOCK_SIZE:     return StoreInt(L, pname, 2);
        default:
            return luaL_error(L, UNEXPECTED_ERROR);
        }
    CheckError(L);
    return 0;
    }


/*------------------------------------------------------------------------------*
 | Registration                                                                 |
 *------------------------------------------------------------------------------*/

static const struct luaL_Reg Functions[] = 
    {
        { "read_buffer", ReadBuffer },
        { "read_pixels", ReadPixels },
        { "clamp_color", ClampColor },
        { "blit_framebuffer", BlitFramebuffer },
        { "copy_image_sub_data", CopyImageSubData },
        { "pixel_store", PixelStore },
        { NULL, NULL } /* sentinel */
    };

void moongl_open_pixel(lua_State *L)
    {
    luaL_setfuncs(L, Functions, 0);
    }

