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

#define MagFilterEnum enumMagFilter()
#define MinFilterEnum enumMinFilter()
#define WrapEnum enumWrap()
#define CompareFuncEnum enumCompareFunc()
#define CompareModeEnum enumCompareMode()

ENUM_STRINGS(PnameStrings) = {
    "wrap s", 
    "wrap t", 
    "wrap r", 
    "min filter", 
    "mag filter", 
    "border color", 
    "min lod", 
    "max lod", 
    "lod bias",
    "compare mode", 
    "compare func",
    NULL
};
ENUM_CODES(PnameCodes) = {
    GL_TEXTURE_WRAP_S, 
    GL_TEXTURE_WRAP_T, 
    GL_TEXTURE_WRAP_R, 
    GL_TEXTURE_MIN_FILTER, 
    GL_TEXTURE_MAG_FILTER, 
    GL_TEXTURE_BORDER_COLOR, 
    GL_TEXTURE_MIN_LOD, 
    GL_TEXTURE_MAX_LOD, 
    GL_TEXTURE_LOD_BIAS,
    GL_TEXTURE_COMPARE_MODE, 
    GL_TEXTURE_COMPARE_FUNC,
};
ENUM_T(PnameEnum, PnameStrings, PnameCodes)
#define CheckPname(L, arg) enumCheck((L), (arg), &PnameEnum)
#define PushPname(L, code) enumPush((L), (code), &PnameEnum)


GEN_FUNC(Sampler)
CREATE_FUNC(Sampler)
DELETE_FUNC(Sampler)
IS_FUNC(Sampler)
BINDN_FUNC(Sampler)


static int BindSampler(lua_State *L)
    {
    GLuint unit = luaL_optinteger(L, 1, 0);
    GLuint sampler = luaL_optinteger(L, 2, 0);
    glBindSampler(unit, sampler);
    CheckError(L);
    return 0;
    }

static int NewSampler(lua_State *L)
    {
    GLuint sampler;
    GLuint unit = luaL_optinteger(L, 1, 0);
    check_init_called(L);
    glGenSamplers(1, &sampler);
    CheckError(L);
    glBindSampler(unit, sampler);
    CheckError(L);
    lua_pushinteger(L, sampler);
    return 1;
    }

/*------------------------------------------------------------------------------*
 | Set                                                                          |
 *------------------------------------------------------------------------------*/


static int SetFloat(lua_State *L, GLuint sampler, GLenum pname, int arg)
    {
    GLfloat param = luaL_checknumber(L, arg);
    glSamplerParameterf(sampler, pname, param);
    CheckError(L);
    return 0;
    }

static int SetFloat4(lua_State *L, GLuint sampler, GLenum pname, int arg)
    {
    GLfloat params[4];
    params[0] = luaL_checknumber(L, arg++);
    params[1] = luaL_checknumber(L, arg++);
    params[2] = luaL_checknumber(L, arg++);
    params[3] = luaL_checknumber(L, arg++);
    glSamplerParameterfv(sampler, pname, params);
    CheckError(L);
    return 0;
    }

static int SetEnum(lua_State *L, GLuint sampler, GLenum pname, int arg, enum_t *e)
    {
    GLenum param = enumCheck(L, arg, e);
    glSamplerParameteri(sampler, pname, param);
    CheckError(L);
    return 0;
    }


static int SamplerParameter(lua_State *L)
    {
    GLuint sampler = luaL_checkinteger(L, 1);
    GLenum pname = CheckPname(L, 2);
    switch(pname)
        {
        case GL_TEXTURE_WRAP_S:
        case GL_TEXTURE_WRAP_T: 
        case GL_TEXTURE_WRAP_R: return SetEnum(L, sampler, pname, 3, WrapEnum);
        case GL_TEXTURE_MIN_FILTER:  return SetEnum(L, sampler, pname, 3, MinFilterEnum);
        case GL_TEXTURE_MAG_FILTER:  return SetEnum(L, sampler, pname, 3, MagFilterEnum);
        case GL_TEXTURE_BORDER_COLOR:  return SetFloat4(L, sampler, pname, 3);
        case GL_TEXTURE_MIN_LOD:  return SetFloat(L, sampler, pname, 3);
        case GL_TEXTURE_MAX_LOD:  return SetFloat(L, sampler, pname, 3);
        case GL_TEXTURE_LOD_BIAS: return SetFloat(L, sampler, pname, 3);
        case GL_TEXTURE_COMPARE_MODE:  return SetEnum(L, sampler, pname, 3, CompareModeEnum);
        case GL_TEXTURE_COMPARE_FUNC:  return SetEnum(L, sampler, pname, 3, CompareFuncEnum);
        default:
            return luaL_error(L, UNEXPECTED_ERROR);
        }
    CheckError(L);
    return 0;
    }

/*------------------------------------------------------------------------------*
 | Get                                                                          |
 *------------------------------------------------------------------------------*/

static int GetEnum(lua_State *L, GLuint sampler, GLenum pname, enum_t *e)
    {
    GLint param;
    glGetSamplerParameteriv(sampler, pname, &param);
    CheckError(L);
    enumPush(L, param, e);
    return 1;
    }

static int GetFloat(lua_State *L, GLuint sampler, GLenum pname)
    {
    GLfloat param;
    CheckError(L);
    glGetSamplerParameterfv(sampler, pname, &param);
    lua_pushnumber(L, param);
    return 1;
    }

static int GetFloat4(lua_State *L, GLuint sampler, GLenum pname)
    {
    GLfloat params[4];
    CheckError(L);
    glGetSamplerParameterfv(sampler, pname, params);
    lua_pushnumber(L, params[0]);
    lua_pushnumber(L, params[1]);
    lua_pushnumber(L, params[2]);
    lua_pushnumber(L, params[3]);
    return 4;
    }

static int GetSamplerParameter(lua_State *L)
    {
    GLuint sampler = luaL_checkinteger(L, 1);
    GLenum pname = CheckPname(L, 2);
    switch(pname)
        {
        case GL_TEXTURE_WRAP_S:
        case GL_TEXTURE_WRAP_T:
        case GL_TEXTURE_WRAP_R: return GetEnum(L, sampler, pname, WrapEnum);
        case GL_TEXTURE_MIN_FILTER:  return GetEnum(L, sampler, pname, MinFilterEnum);
        case GL_TEXTURE_MAG_FILTER:  return GetEnum(L, sampler, pname, MagFilterEnum);
        case GL_TEXTURE_BORDER_COLOR:  return GetFloat4(L, sampler, pname);
        case GL_TEXTURE_MIN_LOD:  return GetFloat(L, sampler, pname);
        case GL_TEXTURE_MAX_LOD:  return GetFloat(L, sampler, pname);
        case GL_TEXTURE_LOD_BIAS: return GetFloat(L, sampler, pname);
        case GL_TEXTURE_COMPARE_MODE:  return GetEnum(L, sampler, pname, CompareModeEnum);
        case GL_TEXTURE_COMPARE_FUNC: return GetEnum(L, sampler, pname, CompareFuncEnum);
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
        { "new_sampler", NewSampler },
        { "gen_samplers", GenSamplers },
        { "create_samplers", CreateSamplers },
        { "delete_samplers", DeleteSamplers },
        { "bind_sampler", BindSampler },
        { "bind_samplers", BindSamplers },
        { "is_sampler", IsSampler },
        { "sampler_parameter", SamplerParameter },
        { "get_sampler_parameter", GetSamplerParameter },
        { NULL, NULL } /* sentinel */
    };

void moongl_open_sampler(lua_State *L)
    {
    luaL_setfuncs(L, Functions, 0);
    }
