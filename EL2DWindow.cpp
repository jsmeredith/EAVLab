// Copyright 2012-2013 UT-Battelle, LLC.  See LICENSE.txt for more information.
#include "EL2DWindow.h"

#include <QMouseEvent>
#include <QToolBar>
#include <QAction>
#include <QActionGroup>

#include <eavlColorTable.h>
#include <eavlPlot.h>
#include <eavl2DWindow.h>
#include <eavlScene.h>
#include <eavlSceneRendererGL.h>
#include <eavlRenderSurfaceGL.h>
#include <eavlWorldAnnotatorGL.h>

#include <cfloat>

// ****************************************************************************
// Constructor:  EL2DWindow::EL2DWindow
//
// Programmer:  Jeremy Meredith
// Creation:    August 16, 2012
//
// Modifications:
// ****************************************************************************
EL2DWindow::EL2DWindow(ELWindowManager *parent)
    : QGLWidget(parent)
{
    settings = NULL;

    mousedown = false;
    shiftKey = false;
    lastx = lasty = -1;
    showghosts = false;
    showmesh = false;

    scene = new eavl2DScene();
    window = new eavl2DWindow(eavlColor(0.0, 0.12, 0.25),
                              new eavlRenderSurfaceGL,
                              scene,
                              new eavlSceneRendererGL,
                              new eavlWorldAnnotatorGL);

    // force creation
    GetSettings();
}


// ****************************************************************************
// Method:  EL2DWindow::PipelineUpdated
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
EL2DWindow::PipelineUpdated(Pipeline *pipe)
{
    settings->PipelineUpdated(pipe);
    UpdatePlots();
    ResetView();
}

// ****************************************************************************
// Method:  EL2DWindow::CurrentPipelineChanged
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
EL2DWindow::CurrentPipelineChanged(int)
{
    /*
    currentPipeline = index;
    UpdatePlots();
    if (watchedPipelines[0])
        updateGL();
        */
}


// ****************************************************************************
// Method:  EL2DWindow::initializeGL
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
EL2DWindow::initializeGL()
{
}


bool
EL2DWindow::UpdatePlots()
{
    //cerr << "EL2DWindow::UpdatePlots\n";
    bool shoulddraw = false;
    scene->plots.clear();
    for (unsigned int i=0;  i<settings->plots.size(); i++)
    {
        Plot &p = settings->plots[i];
        if (!p.pipe || p.pipe->results.size() == 0)
            continue;
        p.CreateEAVLPlot();
        if (!p.eavlplot)
            continue;
        shoulddraw = true;
        scene->plots.push_back(p.eavlplot);
    }
    return shoulddraw;
}


// ****************************************************************************
// Method:  EL2DWindow::ResetView
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
EL2DWindow::ResetView()
{
    scene->ResetView(window);
    updateGL();
}

// ****************************************************************************
// Method:  EL2DWindow::paintGL
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
EL2DWindow::paintGL()
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
    static eavlTextAnnotation *t1=NULL,*t2=NULL,*t3=NULL,*t4=NULL, *t5=NULL, *t6=NULL, *t7=NULL, *t8=NULL;
    if (!t1)
    {
        t1 = new eavlWorldTextAnnotation(window,
            "Test 2D world text, [] height=0.05 at (0.3,0.7)",
                                         eavlColor::white, .05,
                                         0.3, 0.7, 0,
                                         0,0,+1, // "normal" is towards the viewer
                                         -1,1,0); // "up" is towards the upper-left
        // default anchor = lower-left

        t2 = new eavlScreenTextAnnotation(window,
            "Test 2D screen text, [] height=0.05 centered with top anchor at 0.9y",
                                          eavlColor::white, .05,
                                          0, 0.9);
        t2->SetRawAnchor(0,1);

        t3 = new eavlBillboardTextAnnotation(window,
            "Test 2D billboard text, [] height=0.05 screen space at (.3,.3)",
                                             eavlColor::white, .05,
                                             0.3, 0.3, 0.0, true);

        t4 = new eavlBillboardTextAnnotation(window,
            "Test 2D billboard text, [] height=0.05 world space at (.3,.5)",
                                             eavlColor::white, .05,
                                             0.3, 0.5, 0.0, false);
        //t4->SetRawAnchor(-1,-1);

        t5 = new eavlBillboardTextAnnotation(window,
            "Test 2D billboard text, [] height=0.05 world space at (.3,.5) at 135deg",
                                             eavlColor::white, .05,
                                             0.3, 0.5, 0.0, false,
                                             135);
        t6 = new eavlViewportAnchoredScreenTextAnnotation(window,
                                                          "Test viewport anchored text upper-right inside viewport",
                                                          eavlColor::white, .035,
                                                          1,1, 0,0, 0.0);
        t6->SetAlignment(eavlTextAnnotation::Right, eavlTextAnnotation::Top);
        t7 = new eavlViewportAnchoredScreenTextAnnotation(window,
                                                          "Test next line viewport",
                                                          eavlColor::white, .035,
                                                          1,1, 0,-.035, 0.0);
        t7->SetAlignment(eavlTextAnnotation::Right, eavlTextAnnotation::Top);
        t8 = new eavlViewportAnchoredScreenTextAnnotation(window,
                                                          "Test outside viewport",
                                                          eavlColor::white, .035,
                                                          -1,-1, 0,0, 0.0);
        t8->SetAlignment(eavlTextAnnotation::Left, eavlTextAnnotation::Top);
    }
    t1->Render(window->view);
    t2->Render(window->view);
    t3->Render(window->view);
    t4->Render(window->view);
    t5->Render(window->view);
    t6->Render(window->view);
    t7->Render(window->view);
    t8->Render(window->view);
#endif


}

// ****************************************************************************
// Method:  EL2DWindow::resizeGL
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
EL2DWindow::resizeGL(int w, int h)
{
    window->Resize(w,h);
}

// ****************************************************************************
// Method:  EL2DWindow::mousePressEvent
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
EL2DWindow::mousePressEvent(QMouseEvent *mev)
{
    shiftKey = (mev->modifiers() & Qt::ShiftModifier);
    makeCurrent();

    lastx = mev->x();
    lasty = mev->y();
    mousedown = true;
    //updateGL();
}

// ****************************************************************************
// Method:  EL2DWindow::mouseMoveEvent
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
EL2DWindow::mouseMoveEvent(QMouseEvent *mev)
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
// Method:  EL2DWindow::mouseReleaseEvent
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
EL2DWindow::mouseReleaseEvent(QMouseEvent *)
{
    makeCurrent();

    mousedown = false;
    shiftKey = false;
    //updateGL();
}


// ****************************************************************************
// Method:  EL2DWindow::GetSettings
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
EL2DWindow::GetSettings()
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
EL2DWindow::SomethingChanged()
{
    updateGL();
}
