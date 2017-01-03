#ifndef _TEST_GL_CONTEXT_H_
#define _TEST_GL_CONTEXT_H_

#include <graphics/OpenGLIncludes.h>
#include <wx/wx.h>
#include <wx/glcanvas.h>


void CheckGLError();
wxString glGetwxString(GLenum name);


// the rendering context used by all GL canvases
class OpenGLContext : public wxGLContext
{
public:
    OpenGLContext(wxGLCanvas *canvas);

private:
};


#endif // _TEST_GL_CONTEXT_H_