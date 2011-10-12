/* $Id: gl.h,v 1.1.1.1 2001/08/22 11:45:26 bob Exp $ */

/*********** NOTE THIS FILE HAS BEEN CUSTOMIZED FOR ICVM ***********/

/*
 * Mesa 3-D graphics library
 * Version:  3.2.1
 *
 * Copyright (C) 1999-2000  Brian Paul   All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * BRIAN PAUL BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


#ifndef GL_H
#define GL_H

#include <sys/syscall.h>

#ifndef INTERNETC
#if defined(USE_MGL_NAMESPACE)
#include "gl_mangle.h"
#endif
#endif



#ifdef __cplusplus
extern "C" {
#endif



/*
 * Apps can test for this symbol to do conditional compilation if needed.
 */
#define MESA

#define MESA_MAJOR_VERSION 3
#define MESA_MINOR_VERSION 2


#define GL_VERSION_1_1   1
#define GL_VERSION_1_2   1


/*
 *
 * Enumerations
 *
 */

typedef enum {
	/* Boolean values */
	GL_FALSE			= 0,
	GL_TRUE				= 1,

	/* Data types */
	GL_BYTE				= 0x1400,
	GL_UNSIGNED_BYTE		= 0x1401,
	GL_SHORT			= 0x1402,
	GL_UNSIGNED_SHORT		= 0x1403,
	GL_INT				= 0x1404,
	GL_UNSIGNED_INT			= 0x1405,
	GL_FLOAT			= 0x1406,
	GL_DOUBLE			= 0x140A,
	GL_2_BYTES			= 0x1407,
	GL_3_BYTES			= 0x1408,
	GL_4_BYTES			= 0x1409,

	/* Primitives */
	GL_POINTS			= 0x0000,
	GL_LINES			= 0x0001,
	GL_LINE_LOOP			= 0x0002,
	GL_LINE_STRIP			= 0x0003,
	GL_TRIANGLES			= 0x0004,
	GL_TRIANGLE_STRIP		= 0x0005,
	GL_TRIANGLE_FAN			= 0x0006,
	GL_QUADS			= 0x0007,
	GL_QUAD_STRIP			= 0x0008,
	GL_POLYGON			= 0x0009,

	/* Vertex Arrays */
	GL_VERTEX_ARRAY			= 0x8074,
	GL_NORMAL_ARRAY			= 0x8075,
	GL_COLOR_ARRAY			= 0x8076,
	GL_INDEX_ARRAY			= 0x8077,
	GL_TEXTURE_COORD_ARRAY		= 0x8078,
	GL_EDGE_FLAG_ARRAY		= 0x8079,
	GL_VERTEX_ARRAY_SIZE		= 0x807A,
	GL_VERTEX_ARRAY_TYPE		= 0x807B,
	GL_VERTEX_ARRAY_STRIDE		= 0x807C,
	GL_NORMAL_ARRAY_TYPE		= 0x807E,
	GL_NORMAL_ARRAY_STRIDE		= 0x807F,
	GL_COLOR_ARRAY_SIZE		= 0x8081,
	GL_COLOR_ARRAY_TYPE		= 0x8082,
	GL_COLOR_ARRAY_STRIDE		= 0x8083,
	GL_INDEX_ARRAY_TYPE		= 0x8085,
	GL_INDEX_ARRAY_STRIDE		= 0x8086,
	GL_TEXTURE_COORD_ARRAY_SIZE	= 0x8088,
	GL_TEXTURE_COORD_ARRAY_TYPE	= 0x8089,
	GL_TEXTURE_COORD_ARRAY_STRIDE	= 0x808A,
	GL_EDGE_FLAG_ARRAY_STRIDE	= 0x808C,
	GL_VERTEX_ARRAY_POINTER		= 0x808E,
	GL_NORMAL_ARRAY_POINTER		= 0x808F,
	GL_COLOR_ARRAY_POINTER		= 0x8090,
	GL_INDEX_ARRAY_POINTER		= 0x8091,
	GL_TEXTURE_COORD_ARRAY_POINTER	= 0x8092,
	GL_EDGE_FLAG_ARRAY_POINTER	= 0x8093,
	GL_V2F				= 0x2A20,
	GL_V3F				= 0x2A21,
	GL_C4UB_V2F			= 0x2A22,
	GL_C4UB_V3F			= 0x2A23,
	GL_C3F_V3F			= 0x2A24,
	GL_N3F_V3F			= 0x2A25,
	GL_C4F_N3F_V3F			= 0x2A26,
	GL_T2F_V3F			= 0x2A27,
	GL_T4F_V4F			= 0x2A28,
	GL_T2F_C4UB_V3F			= 0x2A29,
	GL_T2F_C3F_V3F			= 0x2A2A,
	GL_T2F_N3F_V3F			= 0x2A2B,
	GL_T2F_C4F_N3F_V3F		= 0x2A2C,
	GL_T4F_C4F_N3F_V4F		= 0x2A2D,

	/* Matrix Mode */
	GL_MATRIX_MODE			= 0x0BA0,
	GL_MODELVIEW			= 0x1700,
	GL_PROJECTION			= 0x1701,
	GL_TEXTURE			= 0x1702,

	/* Points */
	GL_POINT_SMOOTH			= 0x0B10,
	GL_POINT_SIZE			= 0x0B11,
	GL_POINT_SIZE_GRANULARITY 	= 0x0B13,
	GL_POINT_SIZE_RANGE		= 0x0B12,

	/* Lines */
	GL_LINE_SMOOTH			= 0x0B20,
	GL_LINE_STIPPLE			= 0x0B24,
	GL_LINE_STIPPLE_PATTERN		= 0x0B25,
	GL_LINE_STIPPLE_REPEAT		= 0x0B26,
	GL_LINE_WIDTH			= 0x0B21,
	GL_LINE_WIDTH_GRANULARITY	= 0x0B23,
	GL_LINE_WIDTH_RANGE		= 0x0B22,

	/* Polygons */
	GL_POINT			= 0x1B00,
	GL_LINE				= 0x1B01,
	GL_FILL				= 0x1B02,
	GL_CW				= 0x0900,
	GL_CCW				= 0x0901,
	GL_FRONT			= 0x0404,
	GL_BACK				= 0x0405,
	GL_POLYGON_MODE			= 0x0B40,
	GL_POLYGON_SMOOTH		= 0x0B41,
	GL_POLYGON_STIPPLE		= 0x0B42,
	GL_EDGE_FLAG			= 0x0B43,
	GL_CULL_FACE			= 0x0B44,
	GL_CULL_FACE_MODE		= 0x0B45,
	GL_FRONT_FACE			= 0x0B46,
	GL_POLYGON_OFFSET_FACTOR	= 0x8038,
	GL_POLYGON_OFFSET_UNITS		= 0x2A00,
	GL_POLYGON_OFFSET_POINT		= 0x2A01,
	GL_POLYGON_OFFSET_LINE		= 0x2A02,
	GL_POLYGON_OFFSET_FILL		= 0x8037,

	/* Display Lists */
	GL_COMPILE			= 0x1300,
	GL_COMPILE_AND_EXECUTE		= 0x1301,
	GL_LIST_BASE			= 0x0B32,
	GL_LIST_INDEX			= 0x0B33,
	GL_LIST_MODE			= 0x0B30,

	/* Depth buffer */
	GL_NEVER			= 0x0200,
	GL_LESS				= 0x0201,
	GL_GEQUAL			= 0x0206,
	GL_LEQUAL			= 0x0203,
	GL_GREATER			= 0x0204,
	GL_NOTEQUAL			= 0x0205,
	GL_EQUAL			= 0x0202,
	GL_ALWAYS			= 0x0207,
	GL_DEPTH_TEST			= 0x0B71,
	GL_DEPTH_BITS			= 0x0D56,
	GL_DEPTH_CLEAR_VALUE		= 0x0B73,
	GL_DEPTH_FUNC			= 0x0B74,
	GL_DEPTH_RANGE			= 0x0B70,
	GL_DEPTH_WRITEMASK		= 0x0B72,
	GL_DEPTH_COMPONENT		= 0x1902,

	/* Lighting */
	GL_LIGHTING			= 0x0B50,
	GL_LIGHT0			= 0x4000,
	GL_LIGHT1			= 0x4001,
	GL_LIGHT2			= 0x4002,
	GL_LIGHT3			= 0x4003,
	GL_LIGHT4			= 0x4004,
	GL_LIGHT5			= 0x4005,
	GL_LIGHT6			= 0x4006,
	GL_LIGHT7			= 0x4007,
	GL_SPOT_EXPONENT		= 0x1205,
	GL_SPOT_CUTOFF			= 0x1206,
	GL_CONSTANT_ATTENUATION		= 0x1207,
	GL_LINEAR_ATTENUATION		= 0x1208,
	GL_QUADRATIC_ATTENUATION	= 0x1209,
	GL_AMBIENT			= 0x1200,
	GL_DIFFUSE			= 0x1201,
	GL_SPECULAR			= 0x1202,
	GL_SHININESS			= 0x1601,
	GL_EMISSION			= 0x1600,
	GL_POSITION			= 0x1203,
	GL_SPOT_DIRECTION		= 0x1204,
	GL_AMBIENT_AND_DIFFUSE		= 0x1602,
	GL_COLOR_INDEXES		= 0x1603,
	GL_LIGHT_MODEL_TWO_SIDE		= 0x0B52,
	GL_LIGHT_MODEL_LOCAL_VIEWER	= 0x0B51,
	GL_LIGHT_MODEL_AMBIENT		= 0x0B53,
	GL_FRONT_AND_BACK		= 0x0408,
	GL_SHADE_MODEL			= 0x0B54,
	GL_FLAT				= 0x1D00,
	GL_SMOOTH			= 0x1D01,
	GL_COLOR_MATERIAL		= 0x0B57,
	GL_COLOR_MATERIAL_FACE		= 0x0B55,
	GL_COLOR_MATERIAL_PARAMETER	= 0x0B56,
	GL_NORMALIZE			= 0x0BA1,

	/* User clipping planes */
	GL_CLIP_PLANE0			= 0x3000,
	GL_CLIP_PLANE1			= 0x3001,
	GL_CLIP_PLANE2			= 0x3002,
	GL_CLIP_PLANE3			= 0x3003,
	GL_CLIP_PLANE4			= 0x3004,
	GL_CLIP_PLANE5			= 0x3005,

	/* Accumulation buffer */
	GL_ACCUM_RED_BITS		= 0x0D58,
	GL_ACCUM_GREEN_BITS		= 0x0D59,
	GL_ACCUM_BLUE_BITS		= 0x0D5A,
	GL_ACCUM_ALPHA_BITS		= 0x0D5B,
	GL_ACCUM_CLEAR_VALUE		= 0x0B80,
	GL_ACCUM			= 0x0100,
	GL_ADD				= 0x0104,
	GL_LOAD				= 0x0101,
	GL_MULT				= 0x0103,
	GL_RETURN			= 0x0102,

	/* Alpha testing */
	GL_ALPHA_TEST			= 0x0BC0,
	GL_ALPHA_TEST_REF		= 0x0BC2,
	GL_ALPHA_TEST_FUNC		= 0x0BC1,

	/* Blending */
	GL_BLEND			= 0x0BE2,
	GL_BLEND_SRC			= 0x0BE1,
	GL_BLEND_DST			= 0x0BE0,
	GL_ZERO				= 0,
	GL_ONE				= 1,
	GL_SRC_COLOR			= 0x0300,
	GL_ONE_MINUS_SRC_COLOR		= 0x0301,
	GL_DST_COLOR			= 0x0306,
	GL_ONE_MINUS_DST_COLOR		= 0x0307,
	GL_SRC_ALPHA			= 0x0302,
	GL_ONE_MINUS_SRC_ALPHA		= 0x0303,
	GL_DST_ALPHA			= 0x0304,
	GL_ONE_MINUS_DST_ALPHA		= 0x0305,
	GL_SRC_ALPHA_SATURATE		= 0x0308,
	GL_CONSTANT_COLOR		= 0x8001,
	GL_ONE_MINUS_CONSTANT_COLOR	= 0x8002,
	GL_CONSTANT_ALPHA		= 0x8003,
	GL_ONE_MINUS_CONSTANT_ALPHA	= 0x8004,

	/* Render Mode */
	GL_FEEDBACK			= 0x1C01,
	GL_RENDER			= 0x1C00,
	GL_SELECT			= 0x1C02,

	/* Feedback */
	GL_2D				= 0x0600,
	GL_3D				= 0x0601,
	GL_3D_COLOR			= 0x0602,
	GL_3D_COLOR_TEXTURE		= 0x0603,
	GL_4D_COLOR_TEXTURE		= 0x0604,
	GL_POINT_TOKEN			= 0x0701,
	GL_LINE_TOKEN			= 0x0702,
	GL_LINE_RESET_TOKEN		= 0x0707,
	GL_POLYGON_TOKEN		= 0x0703,
	GL_BITMAP_TOKEN			= 0x0704,
	GL_DRAW_PIXEL_TOKEN		= 0x0705,
	GL_COPY_PIXEL_TOKEN		= 0x0706,
	GL_PASS_THROUGH_TOKEN		= 0x0700,
	GL_FEEDBACK_BUFFER_POINTER	= 0x0DF0,
	GL_FEEDBACK_BUFFER_SIZE		= 0x0DF1,
	GL_FEEDBACK_BUFFER_TYPE		= 0x0DF2,

	/* Selection */
	GL_SELECTION_BUFFER_POINTER	= 0x0DF3,
	GL_SELECTION_BUFFER_SIZE	= 0x0DF4,

	/* Fog */
	GL_FOG				= 0x0B60,
	GL_FOG_MODE			= 0x0B65,
	GL_FOG_DENSITY			= 0x0B62,
	GL_FOG_COLOR			= 0x0B66,
	GL_FOG_INDEX			= 0x0B61,
	GL_FOG_START			= 0x0B63,
	GL_FOG_END			= 0x0B64,
	GL_LINEAR			= 0x2601,
	GL_EXP				= 0x0800,
	GL_EXP2				= 0x0801,

	/* Logic Ops */
	GL_LOGIC_OP			= 0x0BF1,
	GL_INDEX_LOGIC_OP		= 0x0BF1,
	GL_COLOR_LOGIC_OP		= 0x0BF2,
	GL_LOGIC_OP_MODE		= 0x0BF0,
	GL_CLEAR			= 0x1500,
	GL_SET				= 0x150F,
	GL_COPY				= 0x1503,
	GL_COPY_INVERTED		= 0x150C,
	GL_NOOP				= 0x1505,
	GL_INVERT			= 0x150A,
	GL_AND				= 0x1501,
	GL_NAND				= 0x150E,
	GL_OR				= 0x1507,
	GL_NOR				= 0x1508,
	GL_XOR				= 0x1506,
	GL_EQUIV			= 0x1509,
	GL_AND_REVERSE			= 0x1502,
	GL_AND_INVERTED			= 0x1504,
	GL_OR_REVERSE			= 0x150B,
	GL_OR_INVERTED			= 0x150D,

	/* Stencil */
	GL_STENCIL_TEST			= 0x0B90,
	GL_STENCIL_WRITEMASK		= 0x0B98,
	GL_STENCIL_BITS			= 0x0D57,
	GL_STENCIL_FUNC			= 0x0B92,
	GL_STENCIL_VALUE_MASK		= 0x0B93,
	GL_STENCIL_REF			= 0x0B97,
	GL_STENCIL_FAIL			= 0x0B94,
	GL_STENCIL_PASS_DEPTH_PASS	= 0x0B96,
	GL_STENCIL_PASS_DEPTH_FAIL	= 0x0B95,
	GL_STENCIL_CLEAR_VALUE		= 0x0B91,
	GL_STENCIL_INDEX		= 0x1901,
	GL_KEEP				= 0x1E00,
	GL_REPLACE			= 0x1E01,
	GL_INCR				= 0x1E02,
	GL_DECR				= 0x1E03,

	/* Buffers, Pixel Drawing/Reading */
	GL_NONE				= 0,
	GL_LEFT				= 0x0406,
	GL_RIGHT			= 0x0407,
	/*GL_FRONT			= 0x0404, */
	/*GL_BACK			= 0x0405, */
	/*GL_FRONT_AND_BACK		= 0x0408, */
	GL_FRONT_LEFT			= 0x0400,
	GL_FRONT_RIGHT			= 0x0401,
	GL_BACK_LEFT			= 0x0402,
	GL_BACK_RIGHT			= 0x0403,
	GL_AUX0				= 0x0409,
	GL_AUX1				= 0x040A,
	GL_AUX2				= 0x040B,
	GL_AUX3				= 0x040C,
	GL_COLOR_INDEX			= 0x1900,
	GL_RED				= 0x1903,
	GL_GREEN			= 0x1904,
	GL_BLUE				= 0x1905,
	GL_ALPHA			= 0x1906,
	GL_LUMINANCE			= 0x1909,
	GL_LUMINANCE_ALPHA		= 0x190A,
	GL_ALPHA_BITS			= 0x0D55,
	GL_RED_BITS			= 0x0D52,
	GL_GREEN_BITS			= 0x0D53,
	GL_BLUE_BITS			= 0x0D54,
	GL_INDEX_BITS			= 0x0D51,
	GL_SUBPIXEL_BITS		= 0x0D50,
	GL_AUX_BUFFERS			= 0x0C00,
	GL_READ_BUFFER			= 0x0C02,
	GL_DRAW_BUFFER			= 0x0C01,
	GL_DOUBLEBUFFER			= 0x0C32,
	GL_STEREO			= 0x0C33,
	GL_BITMAP			= 0x1A00,
	GL_COLOR			= 0x1800,
	GL_DEPTH			= 0x1801,
	GL_STENCIL			= 0x1802,
	GL_DITHER			= 0x0BD0,
	GL_RGB				= 0x1907,
	GL_RGBA				= 0x1908,

	/* Implementation limits */
	GL_MAX_LIST_NESTING		= 0x0B31,
	GL_MAX_ATTRIB_STACK_DEPTH	= 0x0D35,
	GL_MAX_MODELVIEW_STACK_DEPTH	= 0x0D36,
	GL_MAX_NAME_STACK_DEPTH		= 0x0D37,
	GL_MAX_PROJECTION_STACK_DEPTH	= 0x0D38,
	GL_MAX_TEXTURE_STACK_DEPTH	= 0x0D39,
	GL_MAX_EVAL_ORDER		= 0x0D30,
	GL_MAX_LIGHTS			= 0x0D31,
	GL_MAX_CLIP_PLANES		= 0x0D32,
	GL_MAX_TEXTURE_SIZE		= 0x0D33,
	GL_MAX_PIXEL_MAP_TABLE		= 0x0D34,
	GL_MAX_VIEWPORT_DIMS		= 0x0D3A,
	GL_MAX_CLIENT_ATTRIB_STACK_DEPTH= 0x0D3B,

	/* Gets */
	GL_ATTRIB_STACK_DEPTH		= 0x0BB0,
	GL_CLIENT_ATTRIB_STACK_DEPTH	= 0x0BB1,
	GL_COLOR_CLEAR_VALUE		= 0x0C22,
	GL_COLOR_WRITEMASK		= 0x0C23,
	GL_CURRENT_INDEX		= 0x0B01,
	GL_CURRENT_COLOR		= 0x0B00,
	GL_CURRENT_NORMAL		= 0x0B02,
	GL_CURRENT_RASTER_COLOR		= 0x0B04,
	GL_CURRENT_RASTER_DISTANCE	= 0x0B09,
	GL_CURRENT_RASTER_INDEX		= 0x0B05,
	GL_CURRENT_RASTER_POSITION	= 0x0B07,
	GL_CURRENT_RASTER_TEXTURE_COORDS = 0x0B06,
	GL_CURRENT_RASTER_POSITION_VALID = 0x0B08,
	GL_CURRENT_TEXTURE_COORDS	= 0x0B03,
	GL_INDEX_CLEAR_VALUE		= 0x0C20,
	GL_INDEX_MODE			= 0x0C30,
	GL_INDEX_WRITEMASK		= 0x0C21,
	GL_MODELVIEW_MATRIX		= 0x0BA6,
	GL_MODELVIEW_STACK_DEPTH	= 0x0BA3,
	GL_NAME_STACK_DEPTH		= 0x0D70,
	GL_PROJECTION_MATRIX		= 0x0BA7,
	GL_PROJECTION_STACK_DEPTH	= 0x0BA4,
	GL_RENDER_MODE			= 0x0C40,
	GL_RGBA_MODE			= 0x0C31,
	GL_TEXTURE_MATRIX		= 0x0BA8,
	GL_TEXTURE_STACK_DEPTH		= 0x0BA5,
	GL_VIEWPORT			= 0x0BA2,

	/* Evaluators */
	GL_AUTO_NORMAL			= 0x0D80,
	GL_MAP1_COLOR_4			= 0x0D90,
	GL_MAP1_GRID_DOMAIN		= 0x0DD0,
	GL_MAP1_GRID_SEGMENTS		= 0x0DD1,
	GL_MAP1_INDEX			= 0x0D91,
	GL_MAP1_NORMAL			= 0x0D92,
	GL_MAP1_TEXTURE_COORD_1		= 0x0D93,
	GL_MAP1_TEXTURE_COORD_2		= 0x0D94,
	GL_MAP1_TEXTURE_COORD_3		= 0x0D95,
	GL_MAP1_TEXTURE_COORD_4		= 0x0D96,
	GL_MAP1_VERTEX_3		= 0x0D97,
	GL_MAP1_VERTEX_4		= 0x0D98,
	GL_MAP2_COLOR_4			= 0x0DB0,
	GL_MAP2_GRID_DOMAIN		= 0x0DD2,
	GL_MAP2_GRID_SEGMENTS		= 0x0DD3,
	GL_MAP2_INDEX			= 0x0DB1,
	GL_MAP2_NORMAL			= 0x0DB2,
	GL_MAP2_TEXTURE_COORD_1		= 0x0DB3,
	GL_MAP2_TEXTURE_COORD_2		= 0x0DB4,
	GL_MAP2_TEXTURE_COORD_3		= 0x0DB5,
	GL_MAP2_TEXTURE_COORD_4		= 0x0DB6,
	GL_MAP2_VERTEX_3		= 0x0DB7,
	GL_MAP2_VERTEX_4		= 0x0DB8,
	GL_COEFF			= 0x0A00,
	GL_DOMAIN			= 0x0A02,
	GL_ORDER			= 0x0A01,

	/* Hints */
	GL_FOG_HINT			= 0x0C54,
	GL_LINE_SMOOTH_HINT		= 0x0C52,
	GL_PERSPECTIVE_CORRECTION_HINT	= 0x0C50,
	GL_POINT_SMOOTH_HINT		= 0x0C51,
	GL_POLYGON_SMOOTH_HINT		= 0x0C53,
	GL_DONT_CARE			= 0x1100,
	GL_FASTEST			= 0x1101,
	GL_NICEST			= 0x1102,

	/* Scissor box */
	GL_SCISSOR_TEST			= 0x0C11,
	GL_SCISSOR_BOX			= 0x0C10,

	/* Pixel Mode / Transfer */
	GL_MAP_COLOR			= 0x0D10,
	GL_MAP_STENCIL			= 0x0D11,
	GL_INDEX_SHIFT			= 0x0D12,
	GL_INDEX_OFFSET			= 0x0D13,
	GL_RED_SCALE			= 0x0D14,
	GL_RED_BIAS			= 0x0D15,
	GL_GREEN_SCALE			= 0x0D18,
	GL_GREEN_BIAS			= 0x0D19,
	GL_BLUE_SCALE			= 0x0D1A,
	GL_BLUE_BIAS			= 0x0D1B,
	GL_ALPHA_SCALE			= 0x0D1C,
	GL_ALPHA_BIAS			= 0x0D1D,
	GL_DEPTH_SCALE			= 0x0D1E,
	GL_DEPTH_BIAS			= 0x0D1F,
	GL_PIXEL_MAP_S_TO_S_SIZE	= 0x0CB1,
	GL_PIXEL_MAP_I_TO_I_SIZE	= 0x0CB0,
	GL_PIXEL_MAP_I_TO_R_SIZE	= 0x0CB2,
	GL_PIXEL_MAP_I_TO_G_SIZE	= 0x0CB3,
	GL_PIXEL_MAP_I_TO_B_SIZE	= 0x0CB4,
	GL_PIXEL_MAP_I_TO_A_SIZE	= 0x0CB5,
	GL_PIXEL_MAP_R_TO_R_SIZE	= 0x0CB6,
	GL_PIXEL_MAP_G_TO_G_SIZE	= 0x0CB7,
	GL_PIXEL_MAP_B_TO_B_SIZE	= 0x0CB8,
	GL_PIXEL_MAP_A_TO_A_SIZE	= 0x0CB9,
	GL_PIXEL_MAP_S_TO_S		= 0x0C71,
	GL_PIXEL_MAP_I_TO_I		= 0x0C70,
	GL_PIXEL_MAP_I_TO_R		= 0x0C72,
	GL_PIXEL_MAP_I_TO_G		= 0x0C73,
	GL_PIXEL_MAP_I_TO_B		= 0x0C74,
	GL_PIXEL_MAP_I_TO_A		= 0x0C75,
	GL_PIXEL_MAP_R_TO_R		= 0x0C76,
	GL_PIXEL_MAP_G_TO_G		= 0x0C77,
	GL_PIXEL_MAP_B_TO_B		= 0x0C78,
	GL_PIXEL_MAP_A_TO_A		= 0x0C79,
	GL_PACK_ALIGNMENT		= 0x0D05,
	GL_PACK_LSB_FIRST		= 0x0D01,
	GL_PACK_ROW_LENGTH		= 0x0D02,
	GL_PACK_SKIP_PIXELS		= 0x0D04,
	GL_PACK_SKIP_ROWS		= 0x0D03,
	GL_PACK_SWAP_BYTES		= 0x0D00,
	GL_UNPACK_ALIGNMENT		= 0x0CF5,
	GL_UNPACK_LSB_FIRST		= 0x0CF1,
	GL_UNPACK_ROW_LENGTH		= 0x0CF2,
	GL_UNPACK_SKIP_PIXELS		= 0x0CF4,
	GL_UNPACK_SKIP_ROWS		= 0x0CF3,
	GL_UNPACK_SWAP_BYTES		= 0x0CF0,
	GL_ZOOM_X			= 0x0D16,
	GL_ZOOM_Y			= 0x0D17,

	/* Texture mapping */
	GL_TEXTURE_ENV			= 0x2300,
	GL_TEXTURE_ENV_MODE		= 0x2200,
	GL_TEXTURE_1D			= 0x0DE0,
	GL_TEXTURE_2D			= 0x0DE1,
	GL_TEXTURE_WRAP_S		= 0x2802,
	GL_TEXTURE_WRAP_T		= 0x2803,
	GL_TEXTURE_MAG_FILTER		= 0x2800,
	GL_TEXTURE_MIN_FILTER		= 0x2801,
	GL_TEXTURE_ENV_COLOR		= 0x2201,
	GL_TEXTURE_GEN_S		= 0x0C60,
	GL_TEXTURE_GEN_T		= 0x0C61,
	GL_TEXTURE_GEN_MODE		= 0x2500,
	GL_TEXTURE_BORDER_COLOR		= 0x1004,
	GL_TEXTURE_WIDTH		= 0x1000,
	GL_TEXTURE_HEIGHT		= 0x1001,
	GL_TEXTURE_BORDER		= 0x1005,
	GL_TEXTURE_COMPONENTS		= 0x1003,
	GL_TEXTURE_RED_SIZE		= 0x805C,
	GL_TEXTURE_GREEN_SIZE		= 0x805D,
	GL_TEXTURE_BLUE_SIZE		= 0x805E,
	GL_TEXTURE_ALPHA_SIZE		= 0x805F,
	GL_TEXTURE_LUMINANCE_SIZE	= 0x8060,
	GL_TEXTURE_INTENSITY_SIZE	= 0x8061,
	GL_NEAREST_MIPMAP_NEAREST	= 0x2700,
	GL_NEAREST_MIPMAP_LINEAR	= 0x2702,
	GL_LINEAR_MIPMAP_NEAREST	= 0x2701,
	GL_LINEAR_MIPMAP_LINEAR		= 0x2703,
	GL_OBJECT_LINEAR		= 0x2401,
	GL_OBJECT_PLANE			= 0x2501,
	GL_EYE_LINEAR			= 0x2400,
	GL_EYE_PLANE			= 0x2502,
	GL_SPHERE_MAP			= 0x2402,
	GL_DECAL			= 0x2101,
	GL_MODULATE			= 0x2100,
	GL_NEAREST			= 0x2600,
	GL_REPEAT			= 0x2901,
	GL_CLAMP			= 0x2900,
	GL_S				= 0x2000,
	GL_T				= 0x2001,
	GL_R				= 0x2002,
	GL_Q				= 0x2003,
	GL_TEXTURE_GEN_R		= 0x0C62,
	GL_TEXTURE_GEN_Q		= 0x0C63,

	/* GL 1.1 texturing */
	GL_PROXY_TEXTURE_1D		= 0x8063,
	GL_PROXY_TEXTURE_2D		= 0x8064,
	GL_TEXTURE_PRIORITY		= 0x8066,
	GL_TEXTURE_RESIDENT		= 0x8067,
	GL_TEXTURE_BINDING_1D		= 0x8068,
	GL_TEXTURE_BINDING_2D		= 0x8069,
	GL_TEXTURE_INTERNAL_FORMAT	= 0x1003,

	/* GL 1.2 texturing */
	GL_PACK_SKIP_IMAGES		= 0x806B,
	GL_PACK_IMAGE_HEIGHT		= 0x806C,
	GL_UNPACK_SKIP_IMAGES		= 0x806D,
	GL_UNPACK_IMAGE_HEIGHT		= 0x806E,
	GL_TEXTURE_3D			= 0x806F,
	GL_PROXY_TEXTURE_3D		= 0x8070,
	GL_TEXTURE_DEPTH		= 0x8071,
	GL_TEXTURE_WRAP_R		= 0x8072,
	GL_MAX_3D_TEXTURE_SIZE		= 0x8073,
	GL_TEXTURE_BINDING_3D		= 0x806A,

	/* Internal texture formats (GL 1.1) */
	GL_ALPHA4			= 0x803B,
	GL_ALPHA8			= 0x803C,
	GL_ALPHA12			= 0x803D,
	GL_ALPHA16			= 0x803E,
	GL_LUMINANCE4			= 0x803F,
	GL_LUMINANCE8			= 0x8040,
	GL_LUMINANCE12			= 0x8041,
	GL_LUMINANCE16			= 0x8042,
	GL_LUMINANCE4_ALPHA4		= 0x8043,
	GL_LUMINANCE6_ALPHA2		= 0x8044,
	GL_LUMINANCE8_ALPHA8		= 0x8045,
	GL_LUMINANCE12_ALPHA4		= 0x8046,
	GL_LUMINANCE12_ALPHA12		= 0x8047,
	GL_LUMINANCE16_ALPHA16		= 0x8048,
	GL_INTENSITY			= 0x8049,
	GL_INTENSITY4			= 0x804A,
	GL_INTENSITY8			= 0x804B,
	GL_INTENSITY12			= 0x804C,
	GL_INTENSITY16			= 0x804D,
	GL_R3_G3_B2			= 0x2A10,
	GL_RGB4				= 0x804F,
	GL_RGB5				= 0x8050,
	GL_RGB8				= 0x8051,
	GL_RGB10			= 0x8052,
	GL_RGB12			= 0x8053,
	GL_RGB16			= 0x8054,
	GL_RGBA2			= 0x8055,
	GL_RGBA4			= 0x8056,
	GL_RGB5_A1			= 0x8057,
	GL_RGBA8			= 0x8058,
	GL_RGB10_A2			= 0x8059,
	GL_RGBA12			= 0x805A,
	GL_RGBA16			= 0x805B,

	/* Utility */
	GL_VENDOR			= 0x1F00,
	GL_RENDERER			= 0x1F01,
	GL_VERSION			= 0x1F02,
	GL_EXTENSIONS			= 0x1F03,

	/* Errors */
	GL_INVALID_VALUE		= 0x0501,
	GL_INVALID_ENUM			= 0x0500,
	GL_INVALID_OPERATION		= 0x0502,
	GL_STACK_OVERFLOW		= 0x0503,
	GL_STACK_UNDERFLOW		= 0x0504,
	GL_OUT_OF_MEMORY		= 0x0505,

	/*
	 * Extensions
	 */

	/* GL_EXT_blend_minmax and GL_EXT_blend_color */
	GL_CONSTANT_COLOR_EXT			= 0x8001,
	GL_ONE_MINUS_CONSTANT_COLOR_EXT		= 0x8002,
	GL_CONSTANT_ALPHA_EXT			= 0x8003,
	GL_ONE_MINUS_CONSTANT_ALPHA_EXT		= 0x8004,
	GL_BLEND_EQUATION_EXT			= 0x8009,
	GL_MIN_EXT				= 0x8007,
	GL_MAX_EXT				= 0x8008,
	GL_FUNC_ADD_EXT				= 0x8006,
	GL_FUNC_SUBTRACT_EXT			= 0x800A,
	GL_FUNC_REVERSE_SUBTRACT_EXT		= 0x800B,
	GL_BLEND_COLOR_EXT			= 0x8005,

	/* GL_EXT_polygon_offset */
	GL_POLYGON_OFFSET_EXT			= 0x8037,
	GL_POLYGON_OFFSET_FACTOR_EXT		= 0x8038,
	GL_POLYGON_OFFSET_BIAS_EXT		= 0x8039,

	/* GL_EXT_vertex_array */
	GL_VERTEX_ARRAY_EXT			= 0x8074,
	GL_NORMAL_ARRAY_EXT			= 0x8075,
	GL_COLOR_ARRAY_EXT			= 0x8076,
	GL_INDEX_ARRAY_EXT			= 0x8077,
	GL_TEXTURE_COORD_ARRAY_EXT		= 0x8078,
	GL_EDGE_FLAG_ARRAY_EXT			= 0x8079,
	GL_VERTEX_ARRAY_SIZE_EXT		= 0x807A,
	GL_VERTEX_ARRAY_TYPE_EXT		= 0x807B,
	GL_VERTEX_ARRAY_STRIDE_EXT		= 0x807C,
	GL_VERTEX_ARRAY_COUNT_EXT		= 0x807D,
	GL_NORMAL_ARRAY_TYPE_EXT		= 0x807E,
	GL_NORMAL_ARRAY_STRIDE_EXT		= 0x807F,
	GL_NORMAL_ARRAY_COUNT_EXT		= 0x8080,
	GL_COLOR_ARRAY_SIZE_EXT			= 0x8081,
	GL_COLOR_ARRAY_TYPE_EXT			= 0x8082,
	GL_COLOR_ARRAY_STRIDE_EXT		= 0x8083,
	GL_COLOR_ARRAY_COUNT_EXT		= 0x8084,
	GL_INDEX_ARRAY_TYPE_EXT			= 0x8085,
	GL_INDEX_ARRAY_STRIDE_EXT		= 0x8086,
	GL_INDEX_ARRAY_COUNT_EXT		= 0x8087,
	GL_TEXTURE_COORD_ARRAY_SIZE_EXT		= 0x8088,
	GL_TEXTURE_COORD_ARRAY_TYPE_EXT		= 0x8089,
	GL_TEXTURE_COORD_ARRAY_STRIDE_EXT	= 0x808A,
	GL_TEXTURE_COORD_ARRAY_COUNT_EXT	= 0x808B,
	GL_EDGE_FLAG_ARRAY_STRIDE_EXT		= 0x808C,
	GL_EDGE_FLAG_ARRAY_COUNT_EXT		= 0x808D,
	GL_VERTEX_ARRAY_POINTER_EXT		= 0x808E,
	GL_NORMAL_ARRAY_POINTER_EXT		= 0x808F,
	GL_COLOR_ARRAY_POINTER_EXT		= 0x8090,
	GL_INDEX_ARRAY_POINTER_EXT		= 0x8091,
	GL_TEXTURE_COORD_ARRAY_POINTER_EXT	= 0x8092,
	GL_EDGE_FLAG_ARRAY_POINTER_EXT		= 0x8093,

	/* GL_EXT_texture_object */
	GL_TEXTURE_PRIORITY_EXT			= 0x8066,
	GL_TEXTURE_RESIDENT_EXT			= 0x8067,
	GL_TEXTURE_1D_BINDING_EXT		= 0x8068,
	GL_TEXTURE_2D_BINDING_EXT		= 0x8069,

	/* GL_EXT_texture3D */
	GL_PACK_SKIP_IMAGES_EXT			= 0x806B,
	GL_PACK_IMAGE_HEIGHT_EXT		= 0x806C,
	GL_UNPACK_SKIP_IMAGES_EXT		= 0x806D,
	GL_UNPACK_IMAGE_HEIGHT_EXT		= 0x806E,
	GL_TEXTURE_3D_EXT			= 0x806F,
	GL_PROXY_TEXTURE_3D_EXT			= 0x8070,
	GL_TEXTURE_DEPTH_EXT			= 0x8071,
	GL_TEXTURE_WRAP_R_EXT			= 0x8072,
	GL_MAX_3D_TEXTURE_SIZE_EXT		= 0x8073,
	GL_TEXTURE_3D_BINDING_EXT		= 0x806A,

	/* GL_EXT_paletted_texture */
	GL_TABLE_TOO_LARGE_EXT			= 0x8031,
	GL_COLOR_TABLE_FORMAT_EXT		= 0x80D8,
	GL_COLOR_TABLE_WIDTH_EXT		= 0x80D9,
	GL_COLOR_TABLE_RED_SIZE_EXT		= 0x80DA,
	GL_COLOR_TABLE_GREEN_SIZE_EXT		= 0x80DB,
	GL_COLOR_TABLE_BLUE_SIZE_EXT		= 0x80DC,
	GL_COLOR_TABLE_ALPHA_SIZE_EXT	 	= 0x80DD,
	GL_COLOR_TABLE_LUMINANCE_SIZE_EXT	= 0x80DE,
	GL_COLOR_TABLE_INTENSITY_SIZE_EXT	= 0x80DF,
	GL_TEXTURE_INDEX_SIZE_EXT		= 0x80ED,
	GL_COLOR_INDEX1_EXT			= 0x80E2,
	GL_COLOR_INDEX2_EXT			= 0x80E3,
	GL_COLOR_INDEX4_EXT			= 0x80E4,
	GL_COLOR_INDEX8_EXT			= 0x80E5,
	GL_COLOR_INDEX12_EXT			= 0x80E6,
	GL_COLOR_INDEX16_EXT			= 0x80E7,

	/* GL_EXT_shared_texture_palette */
	GL_SHARED_TEXTURE_PALETTE_EXT		= 0x81FB,

	/* GL_EXT_point_parameters */
	GL_POINT_SIZE_MIN_EXT			= 0x8126,
	GL_POINT_SIZE_MAX_EXT			= 0x8127,
	GL_POINT_FADE_THRESHOLD_SIZE_EXT	= 0x8128,
	GL_DISTANCE_ATTENUATION_EXT		= 0x8129,

	/* GL_EXT_rescale_normal */
	GL_RESCALE_NORMAL_EXT			= 0x803A,

	/* GL_EXT_abgr */
	GL_ABGR_EXT				= 0x8000,

	/* GL_EXT_stencil_wrap */
	GL_INCR_WRAP_EXT			= 0x8507,
	GL_DECR_WRAP_EXT			= 0x8508,

	/* GL_SGIS_texture_edge_clamp */
	GL_CLAMP_TO_EDGE_SGIS			= 0x812F,

	/* GL_INGR_blend_func_separate */
	GL_BLEND_DST_RGB_INGR			= 0x80C8,
	GL_BLEND_SRC_RGB_INGR			= 0x80C9,
	GL_BLEND_DST_ALPHA_INGR			= 0x80CA,
	GL_BLEND_SRC_ALPHA_INGR			= 0x80CB,

	/* OpenGL 1.2 */
	GL_RESCALE_NORMAL			= 0x803A,
	GL_CLAMP_TO_EDGE			= 0x812F,
	GL_MAX_ELEMENTS_VERTICES		= 0x80E8,
	GL_MAX_ELEMENTS_INDICES			= 0x80E9,
	GL_BGR					= 0x80E0,
	GL_BGRA					= 0x80E1,
	GL_UNSIGNED_BYTE_3_3_2			= 0x8032,
	GL_UNSIGNED_BYTE_2_3_3_REV		= 0x8362,
	GL_UNSIGNED_SHORT_5_6_5			= 0x8363,
	GL_UNSIGNED_SHORT_5_6_5_REV		= 0x8364,
	GL_UNSIGNED_SHORT_4_4_4_4		= 0x8033,
	GL_UNSIGNED_SHORT_4_4_4_4_REV		= 0x8365,
	GL_UNSIGNED_SHORT_5_5_5_1		= 0x8034,
	GL_UNSIGNED_SHORT_1_5_5_5_REV		= 0x8366,
	GL_UNSIGNED_INT_8_8_8_8			= 0x8035,
	GL_UNSIGNED_INT_8_8_8_8_REV		= 0x8367,
	GL_UNSIGNED_INT_10_10_10_2		= 0x8036,
	GL_UNSIGNED_INT_2_10_10_10_REV		= 0x8368,
	GL_LIGHT_MODEL_COLOR_CONTROL		= 0x81F8,
	GL_SINGLE_COLOR				= 0x81F9,
	GL_SEPARATE_SPECULAR_COLOR		= 0x81FA,
	GL_TEXTURE_MIN_LOD			= 0x813A,
	GL_TEXTURE_MAX_LOD			= 0x813B,
	GL_TEXTURE_BASE_LEVEL			= 0x813C,
	GL_TEXTURE_MAX_LEVEL			= 0x813D,
        GL_SMOOTH_POINT_SIZE_RANGE		= 0x0B12,
        GL_SMOOTH_POINT_SIZE_GRANULARITY	= 0x0B13,
        GL_SMOOTH_LINE_WIDTH_RANGE		= 0x0B22,
        GL_SMOOTH_LINE_WIDTH_GRANULARITY	= 0x0B23,
        GL_ALIASED_POINT_SIZE_RANGE		= 0x846D,
        GL_ALIASED_LINE_WIDTH_RANGE		= 0x846E,

	/* GL_ARB_multitexture */
	GL_TEXTURE0_ARB				= 0x84C0,
	GL_TEXTURE1_ARB				= 0x84C1,
	GL_TEXTURE2_ARB				= 0x84C2,
	GL_TEXTURE3_ARB				= 0x84C3,
	GL_TEXTURE4_ARB				= 0x84C4,
	GL_TEXTURE5_ARB				= 0x84C5,
	GL_TEXTURE6_ARB				= 0x84C6,
	GL_TEXTURE7_ARB				= 0x84C7,
	GL_TEXTURE8_ARB				= 0x84C8,
	GL_TEXTURE9_ARB				= 0x84C9,
	GL_TEXTURE10_ARB			= 0x84CA,
	GL_TEXTURE11_ARB			= 0x84CB,
	GL_TEXTURE12_ARB			= 0x84CC,
	GL_TEXTURE13_ARB			= 0x84CD,
	GL_TEXTURE14_ARB			= 0x84CE,
	GL_TEXTURE15_ARB			= 0x84CF,
	GL_TEXTURE16_ARB			= 0x84D0,
	GL_TEXTURE17_ARB			= 0x84D1,
	GL_TEXTURE18_ARB			= 0x84D2,
	GL_TEXTURE19_ARB			= 0x84D3,
	GL_TEXTURE20_ARB			= 0x84D4,
	GL_TEXTURE21_ARB			= 0x84D5,
	GL_TEXTURE22_ARB			= 0x84D6,
	GL_TEXTURE23_ARB			= 0x84D7,
	GL_TEXTURE24_ARB			= 0x84D8,
	GL_TEXTURE25_ARB			= 0x84D9,
	GL_TEXTURE26_ARB			= 0x84DA,
	GL_TEXTURE27_ARB			= 0x84DB,
	GL_TEXTURE28_ARB			= 0x84DC,
	GL_TEXTURE29_ARB			= 0x84DD,
	GL_TEXTURE30_ARB			= 0x84DE,
	GL_TEXTURE31_ARB			= 0x84DF,
	GL_ACTIVE_TEXTURE_ARB			= 0x84E0,
	GL_CLIENT_ACTIVE_TEXTURE_ARB		= 0x84E1,
	GL_MAX_TEXTURE_UNITS_ARB		= 0x84E2,

	/*
	 * OpenGL 1.2 imaging subset (NOT IMPLEMENTED BY MESA)
	 */
	/* GL_EXT_color_table */
	GL_COLOR_TABLE				= 0x80D0,
	GL_POST_CONVOLUTION_COLOR_TABLE		= 0x80D1,
	GL_POST_COLOR_MATRIX_COLOR_TABLE	= 0x80D2,
	GL_PROXY_COLOR_TABLE			= 0x80D3,
	GL_PROXY_POST_CONVOLUTION_COLOR_TABLE	= 0x80D4,
	GL_PROXY_POST_COLOR_MATRIX_COLOR_TABLE	= 0x80D5,
	GL_COLOR_TABLE_SCALE			= 0x80D6,
	GL_COLOR_TABLE_BIAS			= 0x80D7,
	GL_COLOR_TABLE_FORMAT			= 0x80D8,
	GL_COLOR_TABLE_WIDTH			= 0x80D9,
	GL_COLOR_TABLE_RED_SIZE			= 0x80DA,
	GL_COLOR_TABLE_GREEN_SIZE		= 0x80DB,
	GL_COLOR_TABLE_BLUE_SIZE		= 0x80DC,
	GL_COLOR_TABLE_ALPHA_SIZE		= 0x80DD,
	GL_COLOR_TABLE_LUMINANCE_SIZE		= 0x80DE,
	GL_COLOR_TABLE_INTENSITY_SIZE		= 0x80DF,
	/* GL_EXT_convolution and GL_HP_convolution_border_modes */
	GL_CONVOLUTION_1D			= 0x8010,
	GL_CONVOLUTION_2D			= 0x8011,
	GL_SEPARABLE_2D				= 0x8012,
	GL_CONVOLUTION_BORDER_MODE		= 0x8013,
	GL_CONVOLUTION_FILTER_SCALE		= 0x8014,
	GL_CONVOLUTION_FILTER_BIAS		= 0x8015,
	GL_REDUCE				= 0x8016,
	GL_CONVOLUTION_FORMAT			= 0x8017,
	GL_CONVOLUTION_WIDTH			= 0x8018,
	GL_CONVOLUTION_HEIGHT			= 0x8019,
	GL_MAX_CONVOLUTION_WIDTH		= 0x801A,
	GL_MAX_CONVOLUTION_HEIGHT		= 0x801B,
	GL_POST_CONVOLUTION_RED_SCALE		= 0x801C,
	GL_POST_CONVOLUTION_GREEN_SCALE		= 0x801D,
	GL_POST_CONVOLUTION_BLUE_SCALE		= 0x801E,
	GL_POST_CONVOLUTION_ALPHA_SCALE		= 0x801F,
	GL_POST_CONVOLUTION_RED_BIAS		= 0x8020,
	GL_POST_CONVOLUTION_GREEN_BIAS		= 0x8021,
	GL_POST_CONVOLUTION_BLUE_BIAS		= 0x8022,
	GL_POST_CONVOLUTION_ALPHA_BIAS		= 0x8023,
	GL_CONSTANT_BORDER			= 0x8151,
	GL_REPLICATE_BORDER			= 0x8153,
	GL_CONVOLUTION_BORDER_COLOR		= 0x8154,
	/* GL_SGI_color_matrix */
	GL_COLOR_MATRIX				= 0x80B1,
	GL_COLOR_MATRIX_STACK_DEPTH		= 0x80B2,
	GL_MAX_COLOR_MATRIX_STACK_DEPTH		= 0x80B3,
	GL_POST_COLOR_MATRIX_RED_SCALE		= 0x80B4,
	GL_POST_COLOR_MATRIX_GREEN_SCALE	= 0x80B5,
	GL_POST_COLOR_MATRIX_BLUE_SCALE		= 0x80B6,
	GL_POST_COLOR_MATRIX_ALPHA_SCALE	= 0x80B7,
	GL_POST_COLOR_MATRIX_RED_BIAS		= 0x80B8,
	GL_POST_COLOR_MATRIX_GREEN_BIAS		= 0x80B9,
	GL_POST_COLOR_MATRIX_BLUE_BIAS		= 0x80BA,
	GL_POST_COLOR_MATRIX_ALPHA_BIAS		= 0x80BB,
	/* GL_EXT_histogram */
	GL_HISTOGRAM				= 0x8024,
	GL_PROXY_HISTOGRAM			= 0x8025,
	GL_HISTOGRAM_WIDTH			= 0x8026,
	GL_HISTOGRAM_FORMAT			= 0x8027,
	GL_HISTOGRAM_RED_SIZE			= 0x8028,
	GL_HISTOGRAM_GREEN_SIZE			= 0x8029,
	GL_HISTOGRAM_BLUE_SIZE			= 0x802A,
	GL_HISTOGRAM_ALPHA_SIZE			= 0x802B,
	GL_HISTOGRAM_LUMINANCE_SIZE		= 0x802C,
	GL_HISTOGRAM_SINK			= 0x802D,
	GL_MINMAX				= 0x802E,
	GL_MINMAX_FORMAT			= 0x802F,
	GL_MINMAX_SINK				= 0x8030,
	GL_TABLE_TOO_LARGE			= 0x8031,
	/* GL_EXT_blend_color, GL_EXT_blend_minmax */
	GL_BLEND_EQUATION			= 0x8009,
	GL_MIN					= 0x8007,
	GL_MAX					= 0x8008,
	GL_FUNC_ADD				= 0x8006,
	GL_FUNC_SUBTRACT			= 0x800A,
	GL_FUNC_REVERSE_SUBTRACT		= 0x800B,
	GL_BLEND_COLOR				= 0x8005,

	/* GL_NV_texgen_reflection (nVidia) */
	GL_NORMAL_MAP_NV			= 0x8511,
	GL_REFLECTION_MAP_NV			= 0x8512,

	/* GL_PGI_misc_hints */
	GL_PREFER_DOUBLEBUFFER_HINT_PGI		= 107000,
	GL_STRICT_DEPTHFUNC_HINT_PGI		= 107030,
	GL_STRICT_LIGHTING_HINT_PGI		= 107031,
	GL_STRICT_SCISSOR_HINT_PGI		= 107032,
	GL_FULL_STIPPLE_HINT_PGI		= 107033,
	GL_NATIVE_GRAPHICS_BEGIN_HINT_PGI	= 107011,
	GL_NATIVE_GRAPHICS_END_HINT_PGI		= 107012,
	GL_CONSERVE_MEMORY_HINT_PGI		= 107005,
	GL_RECLAIM_MEMORY_HINT_PGI		= 107006,
	GL_ALWAYS_FAST_HINT_PGI			= 107020,
	GL_ALWAYS_SOFT_HINT_PGI			= 107021,
	GL_ALLOW_DRAW_OBJ_HINT_PGI		= 107022,
	GL_ALLOW_DRAW_WIN_HINT_PGI		= 107023,
	GL_ALLOW_DRAW_FRG_HINT_PGI		= 107024,
	GL_ALLOW_DRAW_SPN_HINT_PGI		= 107024,
	GL_ALLOW_DRAW_MEM_HINT_PGI		= 107025,
	GL_CLIP_NEAR_HINT_PGI			= 107040,
	GL_CLIP_FAR_HINT_PGI			= 107041,
	GL_WIDE_LINE_HINT_PGI		  	= 107042,
	GL_BACK_NORMALS_HINT_PGI		= 107043,
	GL_NATIVE_GRAPHICS_HANDLE_PGI		= 107010,

	/* GL_EXT_compiled_vertex_array */
	GL_ARRAY_ELEMENT_LOCK_FIRST_EXT		= 0x81A8,
	GL_ARRAY_ELEMENT_LOCK_COUNT_EXT		= 0x81A9,

	/* GL_EXT_clip_volume_hint */
	GL_CLIP_VOLUME_CLIPPING_HINT_EXT	= 0x80F0


/* When you add new enums, please make sure you update the strings
 * in enums.c as well...
 */

}
#ifdef CENTERLINE_CLPP
  /* CenterLine C++ workaround: */
  gl_enum;
  typedef int GLenum;
#else
  /* all other compilers */
  GLenum;
#endif


/* GL_NO_ERROR must be zero */
#define GL_NO_ERROR 0



enum {
	GL_CURRENT_BIT		= 0x00000001,
	GL_POINT_BIT		= 0x00000002,
	GL_LINE_BIT		= 0x00000004,
	GL_POLYGON_BIT		= 0x00000008,
	GL_POLYGON_STIPPLE_BIT	= 0x00000010,
	GL_PIXEL_MODE_BIT	= 0x00000020,
	GL_LIGHTING_BIT		= 0x00000040,
	GL_FOG_BIT		= 0x00000080,
	GL_DEPTH_BUFFER_BIT	= 0x00000100,
	GL_ACCUM_BUFFER_BIT	= 0x00000200,
	GL_STENCIL_BUFFER_BIT	= 0x00000400,
	GL_VIEWPORT_BIT		= 0x00000800,
	GL_TRANSFORM_BIT	= 0x00001000,
	GL_ENABLE_BIT		= 0x00002000,
	GL_COLOR_BUFFER_BIT	= 0x00004000,
	GL_HINT_BIT		= 0x00008000,
	GL_EVAL_BIT		= 0x00010000,
	GL_LIST_BIT		= 0x00020000,
	GL_TEXTURE_BIT		= 0x00040000,
	GL_SCISSOR_BIT		= 0x00080000,
	GL_ALL_ATTRIB_BITS	= 0x000FFFFF
};


enum {
	GL_CLIENT_PIXEL_STORE_BIT	= 0x00000001,
	GL_CLIENT_VERTEX_ARRAY_BIT	= 0x00000002
};
#define GL_ALL_CLIENT_ATTRIB_BITS 0xFFFFFFFF


typedef unsigned int GLbitfield;


#ifdef CENTERLINE_CLPP
#define signed
#endif


/*
 *
 * Data types (may be architecture dependent in some cases)
 *
 */

/*  C type		GL type		storage                            */
/*-------------------------------------------------------------------------*/
typedef void		GLvoid;
typedef unsigned char	GLboolean;
typedef signed char	GLbyte;		/* 1-byte signed */
typedef short		GLshort;	/* 2-byte signed */
typedef int		GLint;		/* 4-byte signed */
typedef unsigned char	GLubyte;	/* 1-byte unsigned */
typedef unsigned short	GLushort;	/* 2-byte unsigned */
typedef unsigned int	GLuint;		/* 4-byte unsigned */
typedef int		GLsizei;	/* 4-byte signed */
typedef float		GLfloat;	/* single precision float */
typedef float		GLclampf;	/* single precision float in [0,1] */
typedef double		GLdouble;	/* double precision float */
typedef double		GLclampd;	/* double precision float in [0,1] */


/************** GL API ... ICVM SYSTEM CALLS   **********/


void glHintPGI(GLenum target,GLint mode) ICVMCALL(3200);
void glUnlockArraysEXT(void) ICVMCALL(3201);
void glLockArraysEXT(GLint first,GLsizei count) ICVMCALL(3202);
void glGetSeparableFilter(GLenum target,GLenum format,GLenum type,GLvoid *row,GLvoid *column,GLvoid *span) ICVMCALL(3203);
void glSeparableFilter2D(GLenum target,GLenum internalformat,GLsizei width,GLsizei height,GLenum format,GLenum type,const GLvoid *row,const GLvoid *column) ICVMCALL(3204);
void glGetConvolutionParameteriv(GLenum target,GLenum pname,GLint *params) ICVMCALL(3205);
void glGetConvolutionParameterfv(GLenum target,GLenum pname,GLfloat *params) ICVMCALL(3206);
void glGetConvolutionFilter(GLenum target,GLenum format,GLenum type,GLvoid *image) ICVMCALL(3207);
void glCopyConvolutionFilter2D(GLenum target,GLenum internalformat,GLint x,GLint y,GLsizei width,GLsizei height) ICVMCALL(3208);
void glCopyConvolutionFilter1D(GLenum target,GLenum internalformat,GLint x,GLint y,GLsizei width) ICVMCALL(3209);
void glConvolutionParameteriv(GLenum target,GLenum pname,const GLint *params) ICVMCALL(3210);
void glConvolutionParameteri(GLenum target,GLenum pname,GLint params) ICVMCALL(3211);
void glConvolutionParameterfv(GLenum target,GLenum pname,const GLfloat *params) ICVMCALL(3212);
void glConvolutionParameterf(GLenum target,GLenum pname,GLfloat params) ICVMCALL(3213);
void glConvolutionFilter2D(GLenum target,GLenum internalformat,GLsizei width,GLsizei height,GLenum format,GLenum type,const GLvoid *image) ICVMCALL(3214);
void glConvolutionFilter1D(GLenum target,GLenum internalformat,GLsizei width,GLenum format,GLenum type,const GLvoid *image) ICVMCALL(3215);
void glGetMinmaxParameteriv(GLenum target,GLenum pname,GLint *params) ICVMCALL(3216);
void glGetMinmaxParameterfv(GLenum target,GLenum pname,GLfloat *params) ICVMCALL(3217);
void glGetMinmax(GLenum target,GLboolean reset,GLenum format,GLenum types,GLvoid *values) ICVMCALL(3218);
void glResetMinmax(GLenum target) ICVMCALL(3219);
void glMinmax(GLenum target,GLenum internalformat,GLboolean sink) ICVMCALL(3220);
void glGetHistogramParameteriv(GLenum target,GLenum pname,GLint *params) ICVMCALL(3221);
void glGetHistogramParameterfv(GLenum target,GLenum pname,GLfloat *params) ICVMCALL(3222);
void glGetHistogram(GLenum target,GLboolean reset,GLenum format,GLenum type,GLvoid *values) ICVMCALL(3223);
void glResetHistogram(GLenum target) ICVMCALL(3224);
void glHistogram(GLenum target,GLsizei width,GLenum internalformat,GLboolean sink) ICVMCALL(3225);
void glBlendColor(GLclampf red,GLclampf green,GLclampf blue,GLclampf alpha) ICVMCALL(3226);
void glBlendEquation(GLenum mode) ICVMCALL(3227);
void glGetColorTableParameteriv(GLenum target,GLenum pname,GLint *params) ICVMCALL(3228);
void glGetColorTableParameterfv(GLenum target,GLenum pname,GLfloat *params) ICVMCALL(3229);
void glGetColorTable(GLenum target,GLenum format,GLenum type,GLvoid *table) ICVMCALL(3230);
void glCopyColorTable(GLenum target,GLenum internalformat,GLint x,GLint y,GLsizei width) ICVMCALL(3231);
void glCopyColorSubTable(GLenum target,GLsizei start,GLint x,GLint y,GLsizei width) ICVMCALL(3232);
void glColorTableParameterfv(GLenum target,GLenum pname,const GLfloat *params) ICVMCALL(3233);
void glColorTableParameteriv(GLenum target,GLenum pname,const GLint *params) ICVMCALL(3234);
void glColorSubTable(GLenum target,GLsizei start,GLsizei count,GLenum format,GLenum type,const GLvoid *data) ICVMCALL(3235);
void glColorTable(GLenum target,GLenum internalformat,GLsizei width,GLenum format,GLenum type,const GLvoid *table) ICVMCALL(3236);
void glCopyTexSubImage3D(GLenum target,GLint level,GLint xoffset,GLint yoffset,GLint zoffset,GLint x,GLint y,GLsizei width,GLsizei height) ICVMCALL(3237);
void glTexSubImage3D(GLenum target,GLint level,GLint xoffset,GLint yoffset,GLint zoffset,GLsizei width,GLsizei height,GLsizei depth,GLenum format,GLenum type,const GLvoid *pixels) ICVMCALL(3238);
void glTexImage3D(GLenum target,GLint level,GLint internalFormat,GLsizei width,GLsizei height,GLsizei depth,GLint border,GLenum format,GLenum type,const GLvoid *pixels) ICVMCALL(3239);
void glDrawRangeElements(GLenum mode,GLuint start,GLuint end,GLsizei count,GLenum type,const GLvoid *indices) ICVMCALL(3240);
void glBlendFuncSeparateINGR(GLenum sfactorRGB,GLenum dfactorRGB,GLenum sfactorAlpha,GLenum dfactorAlpha) ICVMCALL(3241);
void glPointParameterfvEXT(GLenum pname,const GLfloat *params) ICVMCALL(3242);
void glPointParameterfEXT(GLenum pname,GLfloat param) ICVMCALL(3243);
void glMultiTexCoord4svARB(GLenum target,const GLshort *v) ICVMCALL(3244);
void glMultiTexCoord4sARB(GLenum target,GLshort s,GLshort t,GLshort r,GLshort q) ICVMCALL(3245);
void glMultiTexCoord4ivARB(GLenum target,const GLint *v) ICVMCALL(3246);
void glMultiTexCoord4iARB(GLenum target,GLint s,GLint t,GLint r,GLint q) ICVMCALL(3247);
void glMultiTexCoord4fvARB(GLenum target,const GLfloat *v) ICVMCALL(3248);
void glMultiTexCoord4fARB(GLenum target,GLfloat s,GLfloat t,GLfloat r,GLfloat q) ICVMCALL(3249);
void glMultiTexCoord4dvARB(GLenum target,const GLdouble *v) ICVMCALL(3250);
void glMultiTexCoord4dARB(GLenum target,GLdouble s,GLdouble t,GLdouble r,GLdouble q) ICVMCALL(3251);
void glMultiTexCoord3svARB(GLenum target,const GLshort *v) ICVMCALL(3252);
void glMultiTexCoord3sARB(GLenum target,GLshort s,GLshort t,GLshort r) ICVMCALL(3253);
void glMultiTexCoord3ivARB(GLenum target,const GLint *v) ICVMCALL(3254);
void glMultiTexCoord3iARB(GLenum target,GLint s,GLint t,GLint r) ICVMCALL(3255);
void glMultiTexCoord3fvARB(GLenum target,const GLfloat *v) ICVMCALL(3256);
void glMultiTexCoord3fARB(GLenum target,GLfloat s,GLfloat t,GLfloat r) ICVMCALL(3257);
void glMultiTexCoord3dvARB(GLenum target,const GLdouble *v) ICVMCALL(3258);
void glMultiTexCoord3dARB(GLenum target,GLdouble s,GLdouble t,GLdouble r) ICVMCALL(3259);
void glMultiTexCoord2svARB(GLenum target,const GLshort *v) ICVMCALL(3260);
void glMultiTexCoord2sARB(GLenum target,GLshort s,GLshort t) ICVMCALL(3261);
void glMultiTexCoord2ivARB(GLenum target,const GLint *v) ICVMCALL(3262);
void glMultiTexCoord2iARB(GLenum target,GLint s,GLint t) ICVMCALL(3263);
void glMultiTexCoord2fvARB(GLenum target,const GLfloat *v) ICVMCALL(3264);
void glMultiTexCoord2fARB(GLenum target,GLfloat s,GLfloat t) ICVMCALL(3265);
void glMultiTexCoord2dvARB(GLenum target,const GLdouble *v) ICVMCALL(3266);
void glMultiTexCoord2dARB(GLenum target,GLdouble s,GLdouble t) ICVMCALL(3267);
void glMultiTexCoord1svARB(GLenum target,const GLshort *v) ICVMCALL(3268);
void glMultiTexCoord1sARB(GLenum target,GLshort s) ICVMCALL(3269);
void glMultiTexCoord1ivARB(GLenum target,const GLint *v) ICVMCALL(3270);
void glMultiTexCoord1iARB(GLenum target,GLint s) ICVMCALL(3271);
void glMultiTexCoord1fvARB(GLenum target,const GLfloat *v) ICVMCALL(3272);
void glMultiTexCoord1fARB(GLenum target,GLfloat s) ICVMCALL(3273);
void glMultiTexCoord1dvARB(GLenum target,const GLdouble *v) ICVMCALL(3274);
void glMultiTexCoord1dARB(GLenum target,GLdouble s) ICVMCALL(3275);
void glClientActiveTextureARB(GLenum texture) ICVMCALL(3276);
void glActiveTextureARB(GLenum texture) ICVMCALL(3277);
void glGetColorTableParameterivEXT(GLenum target,GLenum pname,GLint *params) ICVMCALL(3278);
void glGetColorTableParameterfvEXT(GLenum target,GLenum pname,GLfloat *params) ICVMCALL(3279);
void glGetColorTableEXT(GLenum target,GLenum format,GLenum type,GLvoid *table) ICVMCALL(3280);
void glColorSubTableEXT(GLenum target,GLsizei start,GLsizei count,GLenum format,GLenum type,const GLvoid *data) ICVMCALL(3281);
void glColorTableEXT(GLenum target,GLenum internalformat,GLsizei width,GLenum format,GLenum type,const GLvoid *table) ICVMCALL(3282);
void glCopyTexSubImage3DEXT(GLenum target,GLint level,GLint xoffset,GLint yoffset,GLint zoffset,GLint x,GLint y,GLsizei width,GLsizei height) ICVMCALL(3283);
void glTexSubImage3DEXT(GLenum target,GLint level,GLint xoffset,GLint yoffset,GLint zoffset,GLsizei width,GLsizei height,GLsizei depth,GLenum format,GLenum type,const GLvoid *pixels) ICVMCALL(3284);
void glTexImage3DEXT(GLenum target,GLint level,GLenum internalFormat,GLsizei width,GLsizei height,GLsizei depth,GLint border,GLenum format,GLenum type,const GLvoid *pixels) ICVMCALL(3285);
GLboolean glIsTextureEXT(GLuint texture) ICVMCALL(3286);
GLboolean glAreTexturesResidentEXT(GLsizei n,const GLuint *textures,GLboolean *residences) ICVMCALL(3287);
void glPrioritizeTexturesEXT(GLsizei n,const GLuint *textures,const GLclampf *priorities) ICVMCALL(3288);
void glBindTextureEXT(GLenum target,GLuint texture) ICVMCALL(3289);
void glDeleteTexturesEXT(GLsizei n,const GLuint *textures) ICVMCALL(3290);
void glGenTexturesEXT(GLsizei n,GLuint *textures) ICVMCALL(3291);
void glDrawArraysEXT(GLenum mode,GLint first,GLsizei count) ICVMCALL(3292);
void glArrayElementEXT(GLint i) ICVMCALL(3293);
void glGetPointervEXT(GLenum pname,void **params) ICVMCALL(3294);
void glEdgeFlagPointerEXT(GLsizei stride,GLsizei count,const GLboolean *ptr) ICVMCALL(3295);
void glTexCoordPointerEXT(GLint size,GLenum type,GLsizei stride,GLsizei count,const GLvoid *ptr) ICVMCALL(3296);
void glIndexPointerEXT(GLenum type,GLsizei stride,GLsizei count,const GLvoid *ptr) ICVMCALL(3297);
void glColorPointerEXT(GLint size,GLenum type,GLsizei stride,GLsizei count,const GLvoid *ptr) ICVMCALL(3298);
void glNormalPointerEXT(GLenum type,GLsizei stride,GLsizei count,const GLvoid *ptr) ICVMCALL(3299);
void glVertexPointerEXT(GLint size,GLenum type,GLsizei stride,GLsizei count,const GLvoid *ptr) ICVMCALL(3300);
void glPolygonOffsetEXT(GLfloat factor,GLfloat bias) ICVMCALL(3301);
void glBlendColorEXT(GLclampf red,GLclampf green,GLclampf blue,GLclampf alpha) ICVMCALL(3302);
void glBlendEquationEXT(GLenum mode) ICVMCALL(3303);
void glPopName(void) ICVMCALL(3304);
void glPushName(GLuint name) ICVMCALL(3305);
void glLoadName(GLuint name) ICVMCALL(3306);
void glInitNames(void) ICVMCALL(3307);
void glSelectBuffer(GLsizei size,GLuint *buffer) ICVMCALL(3308);
void glPassThrough(GLfloat token) ICVMCALL(3309);
void glFeedbackBuffer(GLsizei size,GLenum type,GLfloat *buffer) ICVMCALL(3310);
void glFogiv(GLenum pname,const GLint *params) ICVMCALL(3311);
void glFogfv(GLenum pname,const GLfloat *params) ICVMCALL(3312);
void glFogi(GLenum pname,GLint param) ICVMCALL(3313);
void glFogf(GLenum pname,GLfloat param) ICVMCALL(3314);
void glEvalMesh2(GLenum mode,GLint i1,GLint i2,GLint j1,GLint j2) ICVMCALL(3315);
void glEvalMesh1(GLenum mode,GLint i1,GLint i2) ICVMCALL(3316);
void glEvalPoint2(GLint i,GLint j) ICVMCALL(3317);
void glEvalPoint1(GLint i) ICVMCALL(3318);
void glMapGrid2f(GLint un,GLfloat u1,GLfloat u2,GLint vn,GLfloat v1,GLfloat v2) ICVMCALL(3319);
void glMapGrid2d(GLint un,GLdouble u1,GLdouble u2,GLint vn,GLdouble v1,GLdouble v2) ICVMCALL(3320);
void glMapGrid1f(GLint un,GLfloat u1,GLfloat u2) ICVMCALL(3321);
void glMapGrid1d(GLint un,GLdouble u1,GLdouble u2) ICVMCALL(3322);
void glEvalCoord2fv(const GLfloat *u) ICVMCALL(3323);
void glEvalCoord2dv(const GLdouble *u) ICVMCALL(3324);
void glEvalCoord2f(GLfloat u,GLfloat v) ICVMCALL(3325);
void glEvalCoord2d(GLdouble u,GLdouble v) ICVMCALL(3326);
void glEvalCoord1fv(const GLfloat *u) ICVMCALL(3327);
void glEvalCoord1dv(const GLdouble *u) ICVMCALL(3328);
void glEvalCoord1f(GLfloat u) ICVMCALL(3329);
void glEvalCoord1d(GLdouble u) ICVMCALL(3330);
void glGetMapiv(GLenum target,GLenum query,GLint *v) ICVMCALL(3331);
void glGetMapfv(GLenum target,GLenum query,GLfloat *v) ICVMCALL(3332);
void glGetMapdv(GLenum target,GLenum query,GLdouble *v) ICVMCALL(3333);
void glMap2f(GLenum target,GLfloat u1,GLfloat u2,GLint ustride,GLint uorder,GLfloat v1,GLfloat v2,GLint vstride,GLint vorder,const GLfloat *points) ICVMCALL(3334);
void glMap2d(GLenum target,GLdouble u1,GLdouble u2,GLint ustride,GLint uorder,GLdouble v1,GLdouble v2,GLint vstride,GLint vorder,const GLdouble *points) ICVMCALL(3335);
void glMap1f(GLenum target,GLfloat u1,GLfloat u2,GLint stride,GLint order,const GLfloat *points) ICVMCALL(3336);
void glMap1d(GLenum target,GLdouble u1,GLdouble u2,GLint stride,GLint order,const GLdouble *points) ICVMCALL(3337);
void glCopyTexSubImage2D(GLenum target,GLint level,GLint xoffset,GLint yoffset,GLint x,GLint y,GLsizei width,GLsizei height) ICVMCALL(3338);
void glCopyTexSubImage1D(GLenum target,GLint level,GLint xoffset,GLint x,GLint y,GLsizei width) ICVMCALL(3339);
void glCopyTexImage2D(GLenum target,GLint level,GLenum internalformat,GLint x,GLint y,GLsizei width,GLsizei height,GLint border) ICVMCALL(3340);
void glCopyTexImage1D(GLenum target,GLint level,GLenum internalformat,GLint x,GLint y,GLsizei width,GLint border) ICVMCALL(3341);
void glTexSubImage2D(GLenum target,GLint level,GLint xoffset,GLint yoffset,GLsizei width,GLsizei height,GLenum format,GLenum type,const GLvoid *pixels) ICVMCALL(3342);
void glTexSubImage1D(GLenum target,GLint level,GLint xoffset,GLsizei width,GLenum format,GLenum type,const GLvoid *pixels) ICVMCALL(3343);
GLboolean glIsTexture(GLuint texture) ICVMCALL(3344);
GLboolean glAreTexturesResident(GLsizei n,const GLuint *textures,GLboolean *residences) ICVMCALL(3345);
void glPrioritizeTextures(GLsizei n,const GLuint *textures,const GLclampf *priorities) ICVMCALL(3346);
void glBindTexture(GLenum target,GLuint texture) ICVMCALL(3347);
void glDeleteTextures(GLsizei n,const GLuint *textures) ICVMCALL(3348);
void glGenTextures(GLsizei n,GLuint *textures) ICVMCALL(3349);
void glGetTexImage(GLenum target,GLint level,GLenum format,GLenum type,GLvoid *pixels) ICVMCALL(3350);
void glTexImage2D(GLenum target,GLint level,GLint internalFormat,GLsizei width,GLsizei height,GLint border,GLenum format,GLenum type,const GLvoid *pixels) ICVMCALL(3351);
void glTexImage1D(GLenum target,GLint level,GLint internalFormat,GLsizei width,GLint border,GLenum format,GLenum type,const GLvoid *pixels) ICVMCALL(3352);
void glGetTexLevelParameteriv(GLenum target,GLint level,GLenum pname,GLint *params) ICVMCALL(3353);
void glGetTexLevelParameterfv(GLenum target,GLint level,GLenum pname,GLfloat *params) ICVMCALL(3354);
void glGetTexParameteriv(GLenum target,GLenum pname,GLint *params) ICVMCALL(3355);
void glGetTexParameterfv(GLenum target,GLenum pname,GLfloat *params) ICVMCALL(3356);
void glTexParameteriv(GLenum target,GLenum pname,const GLint *params) ICVMCALL(3357);
void glTexParameterfv(GLenum target,GLenum pname,const GLfloat *params) ICVMCALL(3358);
void glTexParameteri(GLenum target,GLenum pname,GLint param) ICVMCALL(3359);
void glTexParameterf(GLenum target,GLenum pname,GLfloat param) ICVMCALL(3360);
void glGetTexEnviv(GLenum target,GLenum pname,GLint *params) ICVMCALL(3361);
void glGetTexEnvfv(GLenum target,GLenum pname,GLfloat *params) ICVMCALL(3362);
void glTexEnviv(GLenum target,GLenum pname,const GLint *params) ICVMCALL(3363);
void glTexEnvfv(GLenum target,GLenum pname,const GLfloat *params) ICVMCALL(3364);
void glTexEnvi(GLenum target,GLenum pname,GLint param) ICVMCALL(3365);
void glTexEnvf(GLenum target,GLenum pname,GLfloat param) ICVMCALL(3366);
void glGetTexGeniv(GLenum coord,GLenum pname,GLint *params) ICVMCALL(3367);
void glGetTexGenfv(GLenum coord,GLenum pname,GLfloat *params) ICVMCALL(3368);
void glGetTexGendv(GLenum coord,GLenum pname,GLdouble *params) ICVMCALL(3369);
void glTexGeniv(GLenum coord,GLenum pname,const GLint *params) ICVMCALL(3370);
void glTexGenfv(GLenum coord,GLenum pname,const GLfloat *params) ICVMCALL(3371);
void glTexGendv(GLenum coord,GLenum pname,const GLdouble *params) ICVMCALL(3372);
void glTexGeni(GLenum coord,GLenum pname,GLint param) ICVMCALL(3373);
void glTexGenf(GLenum coord,GLenum pname,GLfloat param) ICVMCALL(3374);
void glTexGend(GLenum coord,GLenum pname,GLdouble param) ICVMCALL(3375);
void glClearStencil(GLint s) ICVMCALL(3376);
void glStencilOp(GLenum fail,GLenum zfail,GLenum zpass) ICVMCALL(3377);
void glStencilMask(GLuint mask) ICVMCALL(3378);
void glStencilFunc(GLenum func,GLint ref,GLuint mask) ICVMCALL(3379);
void glCopyPixels(GLint x,GLint y,GLsizei width,GLsizei height,GLenum type) ICVMCALL(3380);
void glDrawPixels(GLsizei width,GLsizei height,GLenum format,GLenum type,const GLvoid *pixels) ICVMCALL(3381);
void glReadPixels(GLint x,GLint y,GLsizei width,GLsizei height,GLenum format,GLenum type,GLvoid *pixels) ICVMCALL(3382);
void glBitmap(GLsizei width,GLsizei height,GLfloat xorig,GLfloat yorig,GLfloat xmove,GLfloat ymove,const GLubyte *bitmap) ICVMCALL(3383);
void glGetPixelMapusv(GLenum map,GLushort *values) ICVMCALL(3384);
void glGetPixelMapuiv(GLenum map,GLuint *values) ICVMCALL(3385);
void glGetPixelMapfv(GLenum map,GLfloat *values) ICVMCALL(3386);
void glPixelMapusv(GLenum map,GLint mapsize,const GLushort *values) ICVMCALL(3387);
void glPixelMapuiv(GLenum map,GLint mapsize,const GLuint *values) ICVMCALL(3388);
void glPixelMapfv(GLenum map,GLint mapsize,const GLfloat *values) ICVMCALL(3389);
void glPixelTransferi(GLenum pname,GLint param) ICVMCALL(3390);
void glPixelTransferf(GLenum pname,GLfloat param) ICVMCALL(3391);
void glPixelStorei(GLenum pname,GLint param) ICVMCALL(3392);
void glPixelStoref(GLenum pname,GLfloat param) ICVMCALL(3393);
void glPixelZoom(GLfloat xfactor,GLfloat yfactor) ICVMCALL(3394);
void glColorMaterial(GLenum face,GLenum mode) ICVMCALL(3395);
void glGetMaterialiv(GLenum face,GLenum pname,GLint *params) ICVMCALL(3396);
void glGetMaterialfv(GLenum face,GLenum pname,GLfloat *params) ICVMCALL(3397);
void glMaterialiv(GLenum face,GLenum pname,const GLint *params) ICVMCALL(3398);
void glMaterialfv(GLenum face,GLenum pname,const GLfloat *params) ICVMCALL(3399);
void glMateriali(GLenum face,GLenum pname,GLint param) ICVMCALL(3400);
void glMaterialf(GLenum face,GLenum pname,GLfloat param) ICVMCALL(3401);
void glLightModeliv(GLenum pname,const GLint *params) ICVMCALL(3402);
void glLightModelfv(GLenum pname,const GLfloat *params) ICVMCALL(3403);
void glLightModeli(GLenum pname,GLint param) ICVMCALL(3404);
void glLightModelf(GLenum pname,GLfloat param) ICVMCALL(3405);
void glGetLightiv(GLenum light,GLenum pname,GLint *params) ICVMCALL(3406);
void glGetLightfv(GLenum light,GLenum pname,GLfloat *params) ICVMCALL(3407);
void glLightiv(GLenum light,GLenum pname,const GLint *params) ICVMCALL(3408);
void glLightfv(GLenum light,GLenum pname,const GLfloat *params) ICVMCALL(3409);
void glLighti(GLenum light,GLenum pname,GLint param) ICVMCALL(3410);
void glLightf(GLenum light,GLenum pname,GLfloat param) ICVMCALL(3411);
void glShadeModel(GLenum mode) ICVMCALL(3412);
void glInterleavedArrays(GLenum format,GLsizei stride,const GLvoid *pointer) ICVMCALL(3413);
void glDrawElements(GLenum mode,GLsizei count,GLenum type,const GLvoid *indices) ICVMCALL(3414);
void glDrawArrays(GLenum mode,GLint first,GLsizei count) ICVMCALL(3415);
void glArrayElement(GLint i) ICVMCALL(3416);
void glGetPointerv(GLenum pname,void **params) ICVMCALL(3417);
void glEdgeFlagPointer(GLsizei stride,const GLvoid *ptr) ICVMCALL(3418);
void glTexCoordPointer(GLint size,GLenum type,GLsizei stride,const GLvoid *ptr) ICVMCALL(3419);
void glIndexPointer(GLenum type,GLsizei stride,const GLvoid *ptr) ICVMCALL(3420);
void glColorPointer(GLint size,GLenum type,GLsizei stride,const GLvoid *ptr) ICVMCALL(3421);
void glNormalPointer(GLenum type,GLsizei stride,const GLvoid *ptr) ICVMCALL(3422);
void glVertexPointer(GLint size,GLenum type,GLsizei stride,const GLvoid *ptr) ICVMCALL(3423);
void glRectsv(const GLshort *v1,const GLshort *v2) ICVMCALL(3424);
void glRectiv(const GLint *v1,const GLint *v2) ICVMCALL(3425);
void glRectfv(const GLfloat *v1,const GLfloat *v2) ICVMCALL(3426);
void glRectdv(const GLdouble *v1,const GLdouble *v2) ICVMCALL(3427);
void glRects(GLshort x1,GLshort y1,GLshort x2,GLshort y2) ICVMCALL(3428);
void glRecti(GLint x1,GLint y1,GLint x2,GLint y2) ICVMCALL(3429);
void glRectf(GLfloat x1,GLfloat y1,GLfloat x2,GLfloat y2) ICVMCALL(3430);
void glRectd(GLdouble x1,GLdouble y1,GLdouble x2,GLdouble y2) ICVMCALL(3431);
void glRasterPos4sv(const GLshort *v) ICVMCALL(3432);
void glRasterPos4iv(const GLint *v) ICVMCALL(3433);
void glRasterPos4fv(const GLfloat *v) ICVMCALL(3434);
void glRasterPos4dv(const GLdouble *v) ICVMCALL(3435);
void glRasterPos3sv(const GLshort *v) ICVMCALL(3436);
void glRasterPos3iv(const GLint *v) ICVMCALL(3437);
void glRasterPos3fv(const GLfloat *v) ICVMCALL(3438);
void glRasterPos3dv(const GLdouble *v) ICVMCALL(3439);
void glRasterPos2sv(const GLshort *v) ICVMCALL(3440);
void glRasterPos2iv(const GLint *v) ICVMCALL(3441);
void glRasterPos2fv(const GLfloat *v) ICVMCALL(3442);
void glRasterPos2dv(const GLdouble *v) ICVMCALL(3443);
void glRasterPos4s(GLshort x,GLshort y,GLshort z,GLshort w) ICVMCALL(3444);
void glRasterPos4i(GLint x,GLint y,GLint z,GLint w) ICVMCALL(3445);
void glRasterPos4f(GLfloat x,GLfloat y,GLfloat z,GLfloat w) ICVMCALL(3446);
void glRasterPos4d(GLdouble x,GLdouble y,GLdouble z,GLdouble w) ICVMCALL(3447);
void glRasterPos3s(GLshort x,GLshort y,GLshort z) ICVMCALL(3448);
void glRasterPos3i(GLint x,GLint y,GLint z) ICVMCALL(3449);
void glRasterPos3f(GLfloat x,GLfloat y,GLfloat z) ICVMCALL(3450);
void glRasterPos3d(GLdouble x,GLdouble y,GLdouble z) ICVMCALL(3451);
void glRasterPos2s(GLshort x,GLshort y) ICVMCALL(3452);
void glRasterPos2i(GLint x,GLint y) ICVMCALL(3453);
void glRasterPos2f(GLfloat x,GLfloat y) ICVMCALL(3454);
void glRasterPos2d(GLdouble x,GLdouble y) ICVMCALL(3455);
void glTexCoord4sv(const GLshort *v) ICVMCALL(3456);
void glTexCoord4iv(const GLint *v) ICVMCALL(3457);
void glTexCoord4fv(const GLfloat *v) ICVMCALL(3458);
void glTexCoord4dv(const GLdouble *v) ICVMCALL(3459);
void glTexCoord3sv(const GLshort *v) ICVMCALL(3460);
void glTexCoord3iv(const GLint *v) ICVMCALL(3461);
void glTexCoord3fv(const GLfloat *v) ICVMCALL(3462);
void glTexCoord3dv(const GLdouble *v) ICVMCALL(3463);
void glTexCoord2sv(const GLshort *v) ICVMCALL(3464);
void glTexCoord2iv(const GLint *v) ICVMCALL(3465);
void glTexCoord2fv(const GLfloat *v) ICVMCALL(3466);
void glTexCoord2dv(const GLdouble *v) ICVMCALL(3467);
void glTexCoord1sv(const GLshort *v) ICVMCALL(3468);
void glTexCoord1iv(const GLint *v) ICVMCALL(3469);
void glTexCoord1fv(const GLfloat *v) ICVMCALL(3470);
void glTexCoord1dv(const GLdouble *v) ICVMCALL(3471);
void glTexCoord4s(GLshort s,GLshort t,GLshort r,GLshort q) ICVMCALL(3472);
void glTexCoord4i(GLint s,GLint t,GLint r,GLint q) ICVMCALL(3473);
void glTexCoord4f(GLfloat s,GLfloat t,GLfloat r,GLfloat q) ICVMCALL(3474);
void glTexCoord4d(GLdouble s,GLdouble t,GLdouble r,GLdouble q) ICVMCALL(3475);
void glTexCoord3s(GLshort s,GLshort t,GLshort r) ICVMCALL(3476);
void glTexCoord3i(GLint s,GLint t,GLint r) ICVMCALL(3477);
void glTexCoord3f(GLfloat s,GLfloat t,GLfloat r) ICVMCALL(3478);
void glTexCoord3d(GLdouble s,GLdouble t,GLdouble r) ICVMCALL(3479);
void glTexCoord2s(GLshort s,GLshort t) ICVMCALL(3480);
void glTexCoord2i(GLint s,GLint t) ICVMCALL(3481);
void glTexCoord2f(GLfloat s,GLfloat t) ICVMCALL(3482);
void glTexCoord2d(GLdouble s,GLdouble t) ICVMCALL(3483);
void glTexCoord1s(GLshort s) ICVMCALL(3484);
void glTexCoord1i(GLint s) ICVMCALL(3485);
void glTexCoord1f(GLfloat s) ICVMCALL(3486);
void glTexCoord1d(GLdouble s) ICVMCALL(3487);
void glColor4usv(const GLushort *v) ICVMCALL(3488);
void glColor4uiv(const GLuint *v) ICVMCALL(3489);
void glColor4ubv(const GLubyte *v) ICVMCALL(3490);
void glColor4sv(const GLshort *v) ICVMCALL(3491);
void glColor4iv(const GLint *v) ICVMCALL(3492);
void glColor4fv(const GLfloat *v) ICVMCALL(3493);
void glColor4dv(const GLdouble *v) ICVMCALL(3494);
void glColor4bv(const GLbyte *v) ICVMCALL(3495);
void glColor3usv(const GLushort *v) ICVMCALL(3496);
void glColor3uiv(const GLuint *v) ICVMCALL(3497);
void glColor3ubv(const GLubyte *v) ICVMCALL(3498);
void glColor3sv(const GLshort *v) ICVMCALL(3499);
void glColor3iv(const GLint *v) ICVMCALL(3500);
void glColor3fv(const GLfloat *v) ICVMCALL(3501);
void glColor3dv(const GLdouble *v) ICVMCALL(3502);
void glColor3bv(const GLbyte *v) ICVMCALL(3503);
void glColor4us(GLushort red,GLushort green,GLushort blue,GLushort alpha) ICVMCALL(3504);
void glColor4ui(GLuint red,GLuint green,GLuint blue,GLuint alpha) ICVMCALL(3505);
void glColor4ub(GLubyte red,GLubyte green,GLubyte blue,GLubyte alpha) ICVMCALL(3506);
void glColor4s(GLshort red,GLshort green,GLshort blue,GLshort alpha) ICVMCALL(3507);
void glColor4i(GLint red,GLint green,GLint blue,GLint alpha) ICVMCALL(3508);
void glColor4f(GLfloat red,GLfloat green,GLfloat blue,GLfloat alpha) ICVMCALL(3509);
void glColor4d(GLdouble red,GLdouble green,GLdouble blue,GLdouble alpha) ICVMCALL(3510);
void glColor4b(GLbyte red,GLbyte green,GLbyte blue,GLbyte alpha) ICVMCALL(3511);
void glColor3us(GLushort red,GLushort green,GLushort blue) ICVMCALL(3512);
void glColor3ui(GLuint red,GLuint green,GLuint blue) ICVMCALL(3513);
void glColor3ub(GLubyte red,GLubyte green,GLubyte blue) ICVMCALL(3514);
void glColor3s(GLshort red,GLshort green,GLshort blue) ICVMCALL(3515);
void glColor3i(GLint red,GLint green,GLint blue) ICVMCALL(3516);
void glColor3f(GLfloat red,GLfloat green,GLfloat blue) ICVMCALL(3517);
void glColor3d(GLdouble red,GLdouble green,GLdouble blue) ICVMCALL(3518);
void glColor3b(GLbyte red,GLbyte green,GLbyte blue) ICVMCALL(3519);
void glIndexubv(const GLubyte *c) ICVMCALL(3520);
void glIndexsv(const GLshort *c) ICVMCALL(3521);
void glIndexiv(const GLint *c) ICVMCALL(3522);
void glIndexfv(const GLfloat *c) ICVMCALL(3523);
void glIndexdv(const GLdouble *c) ICVMCALL(3524);
void glIndexub(GLubyte c) ICVMCALL(3525);
void glIndexs(GLshort c) ICVMCALL(3526);
void glIndexi(GLint c) ICVMCALL(3527);
void glIndexf(GLfloat c) ICVMCALL(3528);
void glIndexd(GLdouble c) ICVMCALL(3529);
void glNormal3sv(const GLshort *v) ICVMCALL(3530);
void glNormal3iv(const GLint *v) ICVMCALL(3531);
void glNormal3fv(const GLfloat *v) ICVMCALL(3532);
void glNormal3dv(const GLdouble *v) ICVMCALL(3533);
void glNormal3bv(const GLbyte *v) ICVMCALL(3534);
void glNormal3s(GLshort nx,GLshort ny,GLshort nz) ICVMCALL(3535);
void glNormal3i(GLint nx,GLint ny,GLint nz) ICVMCALL(3536);
void glNormal3f(GLfloat nx,GLfloat ny,GLfloat nz) ICVMCALL(3537);
void glNormal3d(GLdouble nx,GLdouble ny,GLdouble nz) ICVMCALL(3538);
void glNormal3b(GLbyte nx,GLbyte ny,GLbyte nz) ICVMCALL(3539);
void glVertex4sv(const GLshort *v) ICVMCALL(3540);
void glVertex4iv(const GLint *v) ICVMCALL(3541);
void glVertex4fv(const GLfloat *v) ICVMCALL(3542);
void glVertex4dv(const GLdouble *v) ICVMCALL(3543);
void glVertex3sv(const GLshort *v) ICVMCALL(3544);
void glVertex3iv(const GLint *v) ICVMCALL(3545);
void glVertex3fv(const GLfloat *v) ICVMCALL(3546);
void glVertex3dv(const GLdouble *v) ICVMCALL(3547);
void glVertex2sv(const GLshort *v) ICVMCALL(3548);
void glVertex2iv(const GLint *v) ICVMCALL(3549);
void glVertex2fv(const GLfloat *v) ICVMCALL(3550);
void glVertex2dv(const GLdouble *v) ICVMCALL(3551);
void glVertex4s(GLshort x,GLshort y,GLshort z,GLshort w) ICVMCALL(3552);
void glVertex4i(GLint x,GLint y,GLint z,GLint w) ICVMCALL(3553);
void glVertex4f(GLfloat x,GLfloat y,GLfloat z,GLfloat w) ICVMCALL(3554);
void glVertex4d(GLdouble x,GLdouble y,GLdouble z,GLdouble w) ICVMCALL(3555);
void glVertex3s(GLshort x,GLshort y,GLshort z) ICVMCALL(3556);
void glVertex3i(GLint x,GLint y,GLint z) ICVMCALL(3557);
void glVertex3f(GLfloat x,GLfloat y,GLfloat z) ICVMCALL(3558);
void glVertex3d(GLdouble x,GLdouble y,GLdouble z) ICVMCALL(3559);
void glVertex2s(GLshort x,GLshort y) ICVMCALL(3560);
void glVertex2i(GLint x,GLint y) ICVMCALL(3561);
void glVertex2f(GLfloat x,GLfloat y) ICVMCALL(3562);
void glVertex2d(GLdouble x,GLdouble y) ICVMCALL(3563);
void glEnd(void) ICVMCALL(3564);
void glBegin(GLenum mode) ICVMCALL(3565);
void glListBase(GLuint base) ICVMCALL(3566);
void glCallLists(GLsizei n,GLenum type,const GLvoid *lists) ICVMCALL(3567);
void glCallList(GLuint list) ICVMCALL(3568);
void glEndList(void) ICVMCALL(3569);
void glNewList(GLuint list,GLenum mode) ICVMCALL(3570);
GLuint glGenLists(GLsizei range) ICVMCALL(3571);
void glDeleteLists(GLuint list,GLsizei range) ICVMCALL(3572);
GLboolean glIsList(GLuint list) ICVMCALL(3573);
void glTranslatef(GLfloat x,GLfloat y,GLfloat z) ICVMCALL(3574);
void glTranslated(GLdouble x,GLdouble y,GLdouble z) ICVMCALL(3575);
void glScalef(GLfloat x,GLfloat y,GLfloat z) ICVMCALL(3576);
void glScaled(GLdouble x,GLdouble y,GLdouble z) ICVMCALL(3577);
void glRotatef(GLfloat angle,GLfloat x,GLfloat y,GLfloat z) ICVMCALL(3578);
void glRotated(GLdouble angle,GLdouble x,GLdouble y,GLdouble z) ICVMCALL(3579);
void glMultMatrixf(const GLfloat *m) ICVMCALL(3580);
void glMultMatrixd(const GLdouble *m) ICVMCALL(3581);
void glLoadMatrixf(const GLfloat *m) ICVMCALL(3582);
void glLoadMatrixd(const GLdouble *m) ICVMCALL(3583);
void glLoadIdentity(void) ICVMCALL(3584);
void glPopMatrix(void) ICVMCALL(3585);
void glPushMatrix(void) ICVMCALL(3586);
void glViewport(GLint x,GLint y,GLsizei width,GLsizei height) ICVMCALL(3587);
void glFrustum(GLdouble left,GLdouble right,GLdouble bottom,GLdouble top,GLdouble near_val,GLdouble far_val) ICVMCALL(3588);
void glOrtho(GLdouble left,GLdouble right,GLdouble bottom,GLdouble top,GLdouble near_val,GLdouble far_val) ICVMCALL(3589);
void glMatrixMode(GLenum mode) ICVMCALL(3590);
void glAccum(GLenum op,GLfloat value) ICVMCALL(3591);
void glClearAccum(GLfloat red,GLfloat green,GLfloat blue,GLfloat alpha) ICVMCALL(3592);
void glDepthRange(GLclampd near_val,GLclampd far_val) ICVMCALL(3593);
void glDepthMask(GLboolean flag) ICVMCALL(3594);
void glDepthFunc(GLenum func) ICVMCALL(3595);
void glClearDepth(GLclampd depth) ICVMCALL(3596);
void glHint(GLenum target,GLenum mode) ICVMCALL(3597);
void glFlush(void) ICVMCALL(3598);
void glFinish(void) ICVMCALL(3599);
const GLubyte* glGetString(GLenum name) ICVMCALL(3600);
GLenum glGetError(void) ICVMCALL(3601);
GLint glRenderMode(GLenum mode) ICVMCALL(3602);
void glPopClientAttrib(void) ICVMCALL(3603);
void glPushClientAttrib(GLbitfield mask) ICVMCALL(3604);
void glPopAttrib(void) ICVMCALL(3605);
void glPushAttrib(GLbitfield mask) ICVMCALL(3606);
void glGetIntegerv(GLenum pname,GLint *params) ICVMCALL(3607);
void glGetFloatv(GLenum pname,GLfloat *params) ICVMCALL(3608);
void glGetDoublev(GLenum pname,GLdouble *params) ICVMCALL(3609);
void glGetBooleanv(GLenum pname,GLboolean *params) ICVMCALL(3610);
void glDisableClientState(GLenum cap) ICVMCALL(3611);
void glEnableClientState(GLenum cap) ICVMCALL(3612);
GLboolean glIsEnabled(GLenum cap) ICVMCALL(3613);
void glDisable(GLenum cap) ICVMCALL(3614);
void glEnable(GLenum cap) ICVMCALL(3615);
void glReadBuffer(GLenum mode) ICVMCALL(3616);
void glDrawBuffer(GLenum mode) ICVMCALL(3617);
void glGetClipPlane(GLenum plane,GLdouble *equation) ICVMCALL(3618);
void glClipPlane(GLenum plane,const GLdouble *equation) ICVMCALL(3619);
void glScissor(GLint x,GLint y,GLsizei width,GLsizei height) ICVMCALL(3620);
void glEdgeFlagv(const GLboolean *flag) ICVMCALL(3621);
void glEdgeFlag(GLboolean flag) ICVMCALL(3622);
void glGetPolygonStipple(GLubyte *mask) ICVMCALL(3623);
void glPolygonStipple(const GLubyte *mask) ICVMCALL(3624);
void glPolygonOffset(GLfloat factor,GLfloat units) ICVMCALL(3625);
void glPolygonMode(GLenum face,GLenum mode) ICVMCALL(3626);
void glLineStipple(GLint factor,GLushort pattern) ICVMCALL(3627);
void glLineWidth(GLfloat width) ICVMCALL(3628);
void glPointSize(GLfloat size) ICVMCALL(3629);
void glFrontFace(GLenum mode) ICVMCALL(3630);
void glCullFace(GLenum mode) ICVMCALL(3631);
void glLogicOp(GLenum opcode) ICVMCALL(3632);
void glBlendFunc(GLenum sfactor,GLenum dfactor) ICVMCALL(3633);
void glAlphaFunc(GLenum func,GLclampf ref) ICVMCALL(3634);
void glColorMask(GLboolean red,GLboolean green,GLboolean blue,GLboolean alpha) ICVMCALL(3635);
void glIndexMask(GLuint mask) ICVMCALL(3636);
void glClear(GLbitfield mask) ICVMCALL(3637);
void glClearColor(GLclampf red,GLclampf green,GLclampf blue,GLclampf alpha) ICVMCALL(3638);
void glClearIndex(GLfloat c) ICVMCALL(3639);



/*
 * Compile-time tests for extensions:
 */
#define GL_EXT_blend_color		1
#define GL_EXT_blend_logic_op		1
#define GL_EXT_blend_minmax		1
#define GL_EXT_blend_subtract		1
#define GL_EXT_polygon_offset		1
#define GL_EXT_vertex_array		1
#define GL_EXT_texture_object		1
#define GL_EXT_texture3D		1
#define GL_EXT_paletted_texture		1
#define GL_EXT_shared_texture_palette	1
#define GL_EXT_point_parameters		1
#define GL_EXT_rescale_normal		1
#define GL_EXT_abgr			1
#define GL_EXT_stencil_wrap		1
#define GL_MESA_window_pos		1
#define GL_MESA_resize_buffers		1
#define GL_SGIS_texture_edge_clamp	1
#define GL_INGR_blend_func_separate	1
#define GL_ARB_multitexture		1
#define GL_NV_texgen_reflection		1
#define GL_PGI_misc_hints               1
#define GL_EXT_compiled_vertex_array    1
#define GL_EXT_clip_volume_hint         1


#ifdef __cplusplus
}
#endif

#endif
