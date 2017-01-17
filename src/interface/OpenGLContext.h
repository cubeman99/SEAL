#ifndef _OPEN_GL_CONTEXT_H_
#define _OPEN_GL_CONTEXT_H_

#include <graphics/OpenGLIncludes.h>
#include <wx/wx.h>
#include <wx/glcanvas.h>


//-----------------------------------------------------------------------------
// OpenGLContext - The rendering context used by all GL canvases
//-----------------------------------------------------------------------------
class OpenGLContext : public wxGLContext
{
public:
    OpenGLContext(wxGLCanvas *canvas);

private:
};


#endif // _OPEN_GL_CONTEXT_H_