#ifndef _INT_GL_3_2_HPP
#define _INT_GL_3_2_HPP


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


extern void (GLE_FUNCPTR *__gleGetInteger64i_v)(GLenum , GLuint , GLint64 *);
extern void (GLE_FUNCPTR *__gleGetBufferParameteri64v)(GLenum , GLenum , GLint64 *);
extern void (GLE_FUNCPTR *__gleFramebufferTexture)(GLenum , GLenum , GLuint , GLint );
#ifdef __cplusplus
}
#endif //__cplusplus



namespace gl
{
	enum _int_gl_3_2
	{
		GL_CONTEXT_CORE_PROFILE_BIT      = 0x00000001,
		GL_CONTEXT_COMPATIBILITY_PROFILE_BIT = 0x00000002,
		GL_LINES_ADJACENCY               = 0x000A,
		GL_LINE_STRIP_ADJACENCY          = 0x000B,
		GL_TRIANGLES_ADJACENCY           = 0x000C,
		GL_TRIANGLE_STRIP_ADJACENCY      = 0x000D,
		GL_PROGRAM_POINT_SIZE            = 0x8642,
		GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS = 0x8C29,
		GL_FRAMEBUFFER_ATTACHMENT_LAYERED = 0x8DA7,
		GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS = 0x8DA8,
		GL_GEOMETRY_SHADER               = 0x8DD9,
		GL_GEOMETRY_VERTICES_OUT         = 0x8916,
		GL_GEOMETRY_INPUT_TYPE           = 0x8917,
		GL_GEOMETRY_OUTPUT_TYPE          = 0x8918,
		GL_MAX_GEOMETRY_UNIFORM_COMPONENTS = 0x8DDF,
		GL_MAX_GEOMETRY_OUTPUT_VERTICES  = 0x8DE0,
		GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS = 0x8DE1,
		GL_MAX_VERTEX_OUTPUT_COMPONENTS  = 0x9122,
		GL_MAX_GEOMETRY_INPUT_COMPONENTS = 0x9123,
		GL_MAX_GEOMETRY_OUTPUT_COMPONENTS = 0x9124,
		GL_MAX_FRAGMENT_INPUT_COMPONENTS = 0x9125,
		GL_CONTEXT_PROFILE_MASK          = 0x9126,
	};

	inline void GetInteger64i_v(GLenum target, GLuint index, GLint64 *data) { ::__gleGetInteger64i_v(target, index, data); }
	inline void GetBufferParameteri64v(GLenum target, GLenum pname, GLint64 *params) { ::__gleGetBufferParameteri64v(target, pname, params); }
	inline void FramebufferTexture(GLenum target, GLenum attachment, GLuint texture, GLint level) { ::__gleFramebufferTexture(target, attachment, texture, level); }

}

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

#endif //_INT_GL_3_2_HPP
