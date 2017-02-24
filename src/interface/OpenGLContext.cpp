#include "OpenGLContext.h"
#include "MainApplication.h"
#include <graphics/Graphics.h>


//-----------------------------------------------------------------------------
// Internal OpenGL helper functions
//-----------------------------------------------------------------------------

static void CheckGLError()
{
    GLenum errLast = GL_NO_ERROR;

    for ( ;; )
    {
        GLenum err = glGetError();
        if ( err == GL_NO_ERROR )
            return;

        // normally the error is reset by the call to glGetError() but if
        // glGetError() itself returns an error, we risk looping forever here
        // so check that we get a different error than the last time
        if ( err == errLast )
        {
            wxLogError(wxT("OpenGL error state couldn't be reset."));
            return;
        }

        errLast = err;

        wxLogError(wxT("OpenGL error %d"), err);
    }
}

static wxString glGetwxString(GLenum name)
{
    const GLubyte* v = glGetString(name);
    if (v == NULL)
    {
        // The error is not important. It is GL_INVALID_ENUM.
        // We just want to clear the error stack.
        glGetError();

        return wxString();
    }

    return wxString((const char*) v);
}


//-----------------------------------------------------------------------------
// OpenGLContext
//-----------------------------------------------------------------------------

OpenGLContext::OpenGLContext(wxGLCanvas* canvas) :
	wxGLContext(canvas)
{
    SetCurrent(*canvas);
	
    // set up the parameters we want to use
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

	// Initialize GLEW.
	glewInit();

    CheckGLError();
}

