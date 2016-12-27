#ifndef _TEST_GL_CONTEXT_H_
#define _TEST_GL_CONTEXT_H_

#include <graphics/OpenGLIncludes.h>
#include <wx/wx.h>
#include <wx/glcanvas.h>


// the rendering context used by all GL canvases
class OpenGLContext : public wxGLContext
{
public:
    OpenGLContext(wxGLCanvas *canvas);

    // render the cube showing it at given angles
    void DrawRotatedCube(float xangle, float yangle);

private:
    // textures for the cube faces
    GLuint m_textures[6];
};


#endif // _TEST_GL_CONTEXT_H_