#ifndef _INT_GL_2_1_H
#define _INT_GL_2_1_H


#ifndef APIENTRY
//	#define GLE_REMOVE_APIENTRY
	#if defined(__MINGW32__)
		#define APIENTRY __stdcall
	#elif (_MSC_VER >= 800) || defined(_STDCALL_SUPPORTED) || defined(__BORLANDC__)
		#define APIENTRY __stdcall
	#else
		#define APIENTRY
	#endif
#endif //APIENTRY

#ifndef GLE_FUNCPTR
	#define GLE_REMOVE_FUNCPTR
	#if defined(_WIN32)
		#define GLE_FUNCPTR APIENTRY
	#else
		#define GLE_FUNCPTR
	#endif
#endif //GLE_FUNCPTR

#ifndef GLAPI
	#define GLAPI extern
#endif


#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


#define GL_PIXEL_PACK_BUFFER 0x88EB
#define GL_PIXEL_UNPACK_BUFFER 0x88EC
#define GL_PIXEL_PACK_BUFFER_BINDING 0x88ED
#define GL_PIXEL_UNPACK_BUFFER_BINDING 0x88EF
#define GL_FLOAT_MAT2x3 0x8B65
#define GL_FLOAT_MAT2x4 0x8B66
#define GL_FLOAT_MAT3x2 0x8B67
#define GL_FLOAT_MAT3x4 0x8B68
#define GL_FLOAT_MAT4x2 0x8B69
#define GL_FLOAT_MAT4x3 0x8B6A
#define GL_SRGB 0x8C40
#define GL_SRGB8 0x8C41
#define GL_SRGB_ALPHA 0x8C42
#define GL_SRGB8_ALPHA8 0x8C43
#define GL_COMPRESSED_SRGB 0x8C48
#define GL_COMPRESSED_SRGB_ALPHA 0x8C49
typedef void (GLE_FUNCPTR * PFNGLUNIFORMMATRIX2X3FVPROC)(GLint , GLsizei , GLboolean , const GLfloat *);
typedef void (GLE_FUNCPTR * PFNGLUNIFORMMATRIX3X2FVPROC)(GLint , GLsizei , GLboolean , const GLfloat *);
typedef void (GLE_FUNCPTR * PFNGLUNIFORMMATRIX2X4FVPROC)(GLint , GLsizei , GLboolean , const GLfloat *);
typedef void (GLE_FUNCPTR * PFNGLUNIFORMMATRIX4X2FVPROC)(GLint , GLsizei , GLboolean , const GLfloat *);
typedef void (GLE_FUNCPTR * PFNGLUNIFORMMATRIX3X4FVPROC)(GLint , GLsizei , GLboolean , const GLfloat *);
typedef void (GLE_FUNCPTR * PFNGLUNIFORMMATRIX4X3FVPROC)(GLint , GLsizei , GLboolean , const GLfloat *);

extern PFNGLUNIFORMMATRIX2X3FVPROC __gleUniformMatrix2x3fv;
#define glUniformMatrix2x3fv __gleUniformMatrix2x3fv
extern PFNGLUNIFORMMATRIX3X2FVPROC __gleUniformMatrix3x2fv;
#define glUniformMatrix3x2fv __gleUniformMatrix3x2fv
extern PFNGLUNIFORMMATRIX2X4FVPROC __gleUniformMatrix2x4fv;
#define glUniformMatrix2x4fv __gleUniformMatrix2x4fv
extern PFNGLUNIFORMMATRIX4X2FVPROC __gleUniformMatrix4x2fv;
#define glUniformMatrix4x2fv __gleUniformMatrix4x2fv
extern PFNGLUNIFORMMATRIX3X4FVPROC __gleUniformMatrix3x4fv;
#define glUniformMatrix3x4fv __gleUniformMatrix3x4fv
extern PFNGLUNIFORMMATRIX4X3FVPROC __gleUniformMatrix4x3fv;
#define glUniformMatrix4x3fv __gleUniformMatrix4x3fv


#ifdef __cplusplus
}
#endif //__cplusplus


#ifdef GLE_REMOVE_FUNCPTR
	#undef GLE_REMOVE_FUNCPTR
	#undef GLE_FUNPTR
#endif //GLE_REMOVE_FUNCPTR
/*
#ifdef GLE_REMOVE_APIENTRY
	#undef GLE_REMOVE_APIENTRY
	#undef APIENTRY
#endif //GLE_REMOVE_APIENTRY
*/

#endif //_INT_GL_2_1_H
