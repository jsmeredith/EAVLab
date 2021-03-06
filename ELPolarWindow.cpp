// Copyright 2012-2013 UT-Battelle, LLC.  See LICENSE.txt for more information.
#include "ELPolarWindow.h"

#include <QMouseEvent>
#include <QToolBar>
#include <QAction>
#include <QActionGroup>

#include <eavlColorTable.h>
#include <eavlPlot.h>
#include <eavlPolarWindow.h>
#include <eavlScene.h>
#include <eavlSceneRendererGL.h>
#include <eavlRenderSurfaceGL.h>
#include <eavlWorldAnnotatorGL.h>

#include <cfloat>

// ****************************************************************************
// Constructor:  ELPolarWindow::ELPolarWindow
//
// Programmer:  Jeremy Meredith
// Creation:    August 16, 2012
//
// Modifications:
// ****************************************************************************
ELPolarWindow::ELPolarWindow(ELWindowManager *parent)
    : QGLWidget(parent)
{
    settings = NULL;

    mousedown = false;
    shiftKey = false;
    lastx = lasty = -1;
    showghosts = false;
    showmesh = false;

    scene = new eavlPolarScene();
    window = new eavlPolarWindow(eavlColor(0.0, 0.12, 0.25),
                                 new eavlRenderSurfaceGL,
                                 scene,
                                 new eavlSceneRendererGL,
                                 new eavlWorldAnnotatorGL);

    // force creation
    GetSettings();
}


// ****************************************************************************
// Method:  ELPolarWindow::PipelineUpdated
//
// Purpose:
///   Set the pipeline this window is showing.
//
// Arguments:
//   index      the index of the pipeline
//   p          the pipeline to show
//
// Programmer:  Jeremy Meredith
// Creation:    August 16, 2012
//
// Modifications:
// ****************************************************************************
void
ELPolarWindow::PipelineUpdated(Pipeline *pipe)
{
    settings->PipelineUpdated(pipe);
    UpdatePlots();
    ResetView();
}

// ****************************************************************************
// Method:  ELPolarWindow::CurrentPipelineChanged
//
// Purpose:
///   Tell this window what the currently-edited pipeline index is.
//
// Arguments:
//   index      the new pipeline being edited
//
// Programmer:  Jeremy Meredith
// Creation:    August 16, 2012
//
// Modifications:
// ****************************************************************************
void
ELPolarWindow::CurrentPipelineChanged(int)
{
    /*
    currentPipeline = index;
    UpdatePlots();
    if (watchedPipelines[0])
        updateGL();
        */
}


// ****************************************************************************
// Method:  ELPolarWindow::initializeGL
//
// Purpose:
///   QGLWidget method for initializing various OpenGL things.
//
// Arguments:
//   none
//
// Programmer:  Jeremy Meredith
// Creation:    August 16, 2012
//
// Modifications:
// ****************************************************************************
void
ELPolarWindow::initializeGL()
{
}

static void TransformTo2DCart(double r, double q, double h,
                              double &x, double &y, double &z)
{
    x = r * cos(q*M_PI/180.);
    y = r * sin(q*M_PI/180.);
    z = h;
}


bool
ELPolarWindow::UpdatePlots()
{
    //cerr << "ELPolarWindow::UpdatePlots\n";
    bool shoulddraw = false;
    scene->plots.clear();
    for (unsigned int i=0;  i<settings->plots.size(); i++)
    {
        Plot &p = settings->plots[i];
        if (!p.pipe || p.pipe->results.size() == 0)
            continue;
        p.xform = &TransformTo2DCart;
        p.CreateEAVLPlot();
        if (!p.eavlplot)
            continue;
        shoulddraw = true;
        scene->plots.push_back(p.eavlplot);
    }
    return shoulddraw;
}


// ****************************************************************************
// Method:  ELPolarWindow::ResetView
//
// Purpose:
///   Sets up the camera based on the current pipelines.
//
// Arguments:
//   w,h        new width, height
//
// Programmer:  Jeremy Meredith
// Creation:    August 16, 2012
//
// Modifications:
// ****************************************************************************
void
ELPolarWindow::ResetView()
{
    scene->ResetView(window);
    updateGL();
}

// ****************************************************************************
// Method:  ELPolarWindow::paintGL
//
// Purpose:
///   QGLWidget method for when the window needs to be painter.
///   This does all the drawing.
//
// Arguments:
//   none
//
// Programmer:  Jeremy Meredith
// Creation:    August 16, 2012
//
// Modifications:
//   Jeremy Meredith, Thu Nov 29 12:19:33 EST 2012
//   Added nodal surface normal lighting support.
//
// ****************************************************************************
void
ELPolarWindow::paintGL()
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

    // okay, we think it's safe to proceed now!
    window->Paint();

    // test of font rendering
#if 0
    static eavlTextAnnotation *tt=NULL;
    if (!tt)
    {
        switch (1)
        {
          case 1:
            tt = new eavlWorldTextAnnotation(window,"Test 2D world text, [] height=0.05 at (0.3,0.3)",
                                             eavlColor::white, .05,
                                             0.3, 0.3, 0,
                                             0,0,-1,   -1,1,0);
            //tt->SetAnchor(.5,.5);
            break;
          case 2:
            tt = new eavlScreenTextAnnotation(window,"Test 2D text, [] height=0.05 centered with top anchor at 0.9y",
                                              eavlColor::white, .05,
                                              0, 0.9);
            tt->SetAnchor(.5,1);
            break;
          case 3:
            tt = new eavlBillboardTextAnnotation(window,"Test 2D text, [] height=0.05 screen space at (.3,.3)",
                                                 eavlColor::white, .05,
                                                 0.3, 0.3, 0.0, true);
            //tt->SetAnchor(.5,0);
            break;
          case 4:
            tt = new eavlBillboardTextAnnotation(window,"Test 2D text, [] height=0.05 world space at (.3,.3)",
                                                 eavlColor::white, .05,
                                                 0.3, 0.3, 0.0, false);
            tt->SetAnchor(0,0);
            break;
        }
    }
    glDisable(GL_DEPTH_TEST);
    tt->Render(view);
    ///\todo: hack: should SetMatrices maybe do this?
#endif


}

// ****************************************************************************
// Method:  ELPolarWindow::resizeGL
//
// Purpose:
///   QGLWidget method for when the window is created or resized.
///   Here, set the aspect and gl viewport.
//
// Arguments:
//   w,h        new width, height
//
// Programmer:  Jeremy Meredith
// Creation:    August 16, 2012
//
// Modifications:
// ****************************************************************************
void
ELPolarWindow::resizeGL(int w, int h)
{
    window->Resize(w,h);
}

// ****************************************************************************
// Method:  ELPolarWindow::mousePressEvent
//
// Purpose:
///   Mouse button is now down; start interaction.
//
// Arguments:
//   mev        the mouse event
//
// Programmer:  Jeremy Meredith
// Creation:    August 15, 2012
//
// Modifications:
// ****************************************************************************
void
ELPolarWindow::mousePressEvent(QMouseEvent *mev)
{
    shiftKey = (mev->modifiers() & Qt::ShiftModifier);
    makeCurrent();

    lastx = mev->x();
    lasty = mev->y();
    mousedown = true;
    //updateGL();
}

// ****************************************************************************
// Method:  ELPolarWindow::mouseMoveEvent
//
// Purpose:
///   Mouse is moved (while mouse is down).  Change camera.
//
// Arguments:
//   mev        the mouse event
//
// Programmer:  Jeremy Meredith
// Creation:    August 15, 2012
//
// Modifications:
// ****************************************************************************
void
ELPolarWindow::mouseMoveEvent(QMouseEvent *mev)
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
            window->view.Zoom2D(zoom, true);
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
// Method:  ELPolarWindow::mouseReleaseEvent
//
// Purpose:
///   Mouse button is no longer down; finish interaction.
//
// Arguments:
//   mev        the mouse event
//
// Programmer:  Jeremy Meredith
// Creation:    August 15, 2012
//
// Modifications:
// ****************************************************************************
void
ELPolarWindow::mouseReleaseEvent(QMouseEvent *)
{
    makeCurrent();

    mousedown = false;
    shiftKey = false;
    //updateGL();
}


// ****************************************************************************
// Method:  ELPolarWindow::GetSettings
//
// Purpose:
///   Return the settings for this window, creating and connecting
///   signals/slots if necessary.
//
// Arguments:
//   none
//
// Programmer:  Jeremy Meredith
// Creation:    August 16, 2012
//
// Modifications:
// ****************************************************************************
QWidget *
ELPolarWindow::GetSettings()
{
    if (!settings)
    {
        settings = new ELPlotList;
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
ELPolarWindow::SomethingChanged()
{
    updateGL();
}
