#ifndef _MAIN_APPLICATION_H_
#define _MAIN_APPLICATION_H_

#include <graphics/OpenGLIncludes.h>
#include <wx/wx.h>
#include "OpenGLContext.h"


//-----------------------------------------------------------------------------
// MainApplication - The shared application class among all simulation windows.
//-----------------------------------------------------------------------------
class MainApplication : public wxApp
{
public:
    MainApplication();

    // Returns the shared context used by all frames and sets it as current for
    // the given canvas.
    OpenGLContext& GetGLContext(wxGLCanvas *canvas);

    // virtual wxApp methods
    virtual bool OnInit();
    virtual int OnExit();
	virtual int OnRun();

private:
    OpenGLContext *m_glContext;
};


DECLARE_APP(MainApplication)


#endif // _MAIN_APPLICATION_H_