// Copyright 2012-2013 UT-Battelle, LLC.  See LICENSE.txt for more information.
#include "EL1DWindow.h"

#include <QMouseEvent>
#include <QToolBar>
#include <QAction>
#include <QActionGroup>

#include <eavlColorTable.h>
#include <eavlRenderer.h>
#include <eavl1DWindow.h>
#include <eavlScene.h>
#include <eavlTexture.h>
#include <eavlTextAnnotation.h>

#include <cfloat>

// ****************************************************************************
// Constructor:  EL1DWindow::EL1DWindow
//
// Programmer:  Jeremy Meredith
// Creation:    January 17, 2013
//
// Modifications:
// ****************************************************************************
EL1DWindow::EL1DWindow(ELWindowManager *parent)
    : QGLWidget(parent)
{
    settings = NULL;

    mousedown = false;
    shiftKey = false;
    lastx = lasty = -1;
    showghosts = false;
    showmesh = false;
    barstyle = false;

    scene = new eavl1DGLScene();
    window = new eavl1DWindow(eavlColor::white, NULL, scene);

    // force creation
    GetSettings();
}


// ****************************************************************************
// Method:  EL1DWindow::SetPipeline
//
// Purpose:
///   Set the pipeline this window is showing.
//
// Arguments:
//   index      the index of the pipeline
//   p          the pipeline to show
//
// Programmer:  Jeremy Meredith
// Creation:    January 17, 2013
//
// Modifications:
// ****************************************************************************
void
EL1DWindow::PipelineUpdated(Pipeline *pipe)
{
    settings->PipelineUpdated(pipe);
    UpdatePlots();
    ResetView();
}

// ****************************************************************************
// Method:  EL1DWindow::CurrentPipelineChanged
//
// Purpose:
///   Tell this window what the currently-edited pipeline index is.
//
// Arguments:
//   index      the new pipeline being edited
//
// Programmer:  Jeremy Meredith
// Creation:    January 17, 2013
//
// Modifications:
// ****************************************************************************
void
EL1DWindow::CurrentPipelineChanged(int index)
{
    /*
    currentPipeline = index;
    UpdatePlots();
    if (watchedPipelines[0])
        updateGL();
        */
}


// ****************************************************************************
// Method:  EL1DWindow::initializeGL
//
// Purpose:
///   QGLWidget method for initializing various OpenGL things.
//
// Arguments:
//   none
//
// Programmer:  Jeremy Meredith
// Creation:    January 17, 2013
//
// Modifications:
// ****************************************************************************
void
EL1DWindow::initializeGL()
{
}


bool
EL1DWindow::UpdatePlots()
{
    //cerr << "EL2DWindow::UpdatePlots\n";
    bool shoulddraw = false;
    scene->plots.clear();
    for (unsigned int i=0;  i<settings->plots.size(); i++)
    {
        Plot &p = settings->plots[i];
        if (!p.pipe || p.pipe->results.size() == 0)
            continue;
        p.CreateRenderer();
        if (!p.renderer)
            continue;
        shoulddraw = true;
        scene->plots.push_back(p.renderer);
    }
    return shoulddraw;
}


// ****************************************************************************
// Method:  EL1DWindow::ResetView
//
// Purpose:
///   Sets up the camera based on the current pipelines.
//
// Arguments:
//   w,h        new width, height
//
// Programmer:  Jeremy Meredith
// Creation:    January 17, 2013
//
// Modifications:
// ****************************************************************************
void
EL1DWindow::ResetView()
{
    //cerr << "EL1DWindow::ResetView\n";
    UpdatePlots();
    scene->ResetView(window);
    updateGL();
}

// ****************************************************************************
// Method:  EL1DWindow::paintGL
//
// Purpose:
///   QGLWidget method for when the window needs to be painter.
///   This does all the drawing.
//
// Arguments:
//   none
//
// Programmer:  Jeremy Meredith
// Creation:    January 17, 2013
//
// Modifications:
//   Jeremy Meredith, Thu Nov 29 12:19:33 EST 2012
//   Added nodal surface normal lighting support.
//
// ****************************************************************************
void
EL1DWindow::paintGL()
{
    bool shoulddraw = UpdatePlots();

    ///\todo: note: there's some issue where this method is getting
    /// called before it's supposed to be.  (I believe it's from
    /// my use of updateGL() in other methods in this class when
    /// e.g. the variable or other settings change.)  If we continue
    /// on anyway, then something bad happens, such as creating
    /// a texture with either the wrong thread or before the context
    /// is ready, which makes the texture unusable.  A safe test
    /// we currently use before proceeding is simply whether or
    /// not any of the plots have data, because we know everything
    /// is safely initialized since the user was able to hit Execute.
    if (!shoulddraw)
        return;

    window->Paint();
}

// ****************************************************************************
// Method:  EL1DWindow::resizeGL
//
// Purpose:
///   QGLWidget method for when the window is created or resized.
///   Here, set the aspect and gl viewport.
//
// Arguments:
//   w,h        new width, height
//
// Programmer:  Jeremy Meredith
// Creation:    January 17, 2013
//
// Modifications:
// ****************************************************************************
void
EL1DWindow::resizeGL(int w, int h)
{
    window->Resize(w,h);
}

// ****************************************************************************
// Method:  EL1DWindow::mousePressEvent
//
// Purpose:
///   Mouse button is now down; start interaction.
//
// Arguments:
//   mev        the mouse event
//
// Programmer:  Jeremy Meredith
// Creation:    January 17, 2013
//
// Modifications:
// ****************************************************************************
void
EL1DWindow::mousePressEvent(QMouseEvent *mev)
{
    shiftKey = (mev->modifiers() & Qt::ShiftModifier);
    makeCurrent();

    lastx = mev->x();
    lasty = mev->y();
    mousedown = true;
    //updateGL();
}

// ****************************************************************************
// Method:  EL1DWindow::mouseMoveEvent
//
// Purpose:
///   Mouse is moved (while mouse is down).  Change camera.
//
// Arguments:
//   mev        the mouse event
//
// Programmer:  Jeremy Meredith
// Creation:    January 17, 2013
//
// Modifications:
// ****************************************************************************
void
EL1DWindow::mouseMoveEvent(QMouseEvent *mev)
{
    makeCurrent();

    int x = mev->x();
    int y = mev->y();


    if (mousedown)
    {
        double x1 =  ((double(lastx*2)/double(width()))  - 1.0);
        double y1 = -((double(lasty*2)/double(height())) - 1.0);
        double x2 =  ((double(  x  *2)/double(width()))  - 1.0);
        double y2 = -((double(  y  *2)/double(height())) - 1.0);

        if (mev->buttons() & Qt::LeftButton)
        {
            double dx = x2-x1;
            double dy = y2-y1;
            window->view.Pan2D(dx,dy);
        }
        else if (mev->buttons() & Qt::MidButton)
        {
            double zoom = y2 - y1;
            window->view.Zoom2D(zoom, false);
        }
        // No: we want a popup menu instead!
        //else if (mev->buttons() & Qt::RightButton)
        //{
        //}

        //renderer->resetProgress();
        updateGL();
    }
    lastx = x;
    lasty = y;
}

// ****************************************************************************
// Method:  EL1DWindow::mouseReleaseEvent
//
// Purpose:
///   Mouse button is no longer down; finish interaction.
//
// Arguments:
//   mev        the mouse event
//
// Programmer:  Jeremy Meredith
// Creation:    January 17, 2013
//
// Modifications:
// ****************************************************************************
void
EL1DWindow::mouseReleaseEvent(QMouseEvent *)
{
    makeCurrent();

    mousedown = false;
    shiftKey = false;
    //updateGL();
}

// ****************************************************************************
// Method:  EL1DWindow::GetSettings
//
// Purpose:
///   Return the settings for this window, creating and connecting
///   signals/slots if necessary.
//
// Arguments:
//   none
//
// Programmer:  Jeremy Meredith
// Creation:    January 17, 2013
//
// Modifications:
// ****************************************************************************
QWidget *
EL1DWindow::GetSettings()
{
    if (!settings)
    {
        settings = new ELPlotList;
        settings->oneDimensional = true;
        connect(settings, SIGNAL(SomethingChanged()),
                this, SLOT(SomethingChanged()));
    }
    return settings;
}

// ****************************************************************************
// Method:  
//
// Purpose:
///   
//
// Arguments:
//   
//
// Programmer:  Jeremy Meredith
// Creation:    March 12, 2013
//
// Modifications:
// ****************************************************************************
void
EL1DWindow::SomethingChanged()
{
    updateGL();
}
