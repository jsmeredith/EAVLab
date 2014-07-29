// Copyright 2012-2013 UT-Battelle, LLC.  See LICENSE.txt for more information.
#include "EL3DWindow.h"

#include "ELRenderOptions.h"

#include <QMouseEvent>
#include <QToolBar>
#include <QAction>
#include <QActionGroup>
#include <QKeyEvent>

#include <eavlColorTable.h>
#include <eavlPlot.h>
#include <eavl3DWindow.h>
#include <eavlScene.h>
#include <eavlTexture.h>
#include <eavlTextAnnotation.h>
#include <eavlSceneRendererSimpleGL.h>
#include <eavlSceneRendererGL.h>
#include <eavlSceneRendererSimpleRT.h>

#include <cfloat>

// ****************************************************************************
// Constructor:  EL3DWindow::EL3DWindow
//
// Programmer:  Jeremy Meredith
// Creation:    August 16, 2012
//
// Modifications:
// ****************************************************************************
EL3DWindow::EL3DWindow(ELWindowManager *parent)
    : QGLWidget(parent)
{
    setFormat(QGLFormat(QGL::SampleBuffers));

    settings = NULL;

    mousedown = false;
    shiftKey = false;
    lastx = lasty = -1;
    showghosts = false;
    showmesh = false;

    scene = new eavl3DGLScene();
    window = new eavl3DWindow(eavlColor(0.15, 0.0, 0.25), NULL, scene,
                              new eavlSceneRendererGL);

    // force creation
    GetSettings();
}


// ****************************************************************************
// Method:  EL3DWindow::SetPipeline
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
EL3DWindow::PipelineUpdated(Pipeline *pipe)
{
    settings->PipelineUpdated(pipe);
    UpdatePlots();
    ResetView();
}

// ****************************************************************************
// Method:  EL3DWindow::CurrentPipelineChanged
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
EL3DWindow::CurrentPipelineChanged(int)
{
    /*
    //cerr << "EL3DWindow::CurrentPipelineChanged\n";
    currentPipeline = index;
    UpdatePlots();
    if (watchedPipelines[0])
        updateGL();
        */
}


// ****************************************************************************
// Method:  EL3DWindow::initializeGL
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
EL3DWindow::initializeGL()
{
}

bool
EL3DWindow::UpdatePlots()
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
// Method:  EL3DWindow::ResetView
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
EL3DWindow::ResetView()
{
    //cerr << "EL3DWindow::ResetView\n";
    UpdatePlots();
    scene->ResetView(window);
    updateGL();
}

// ****************************************************************************
// Method:  EL3DWindow::paintGL
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
EL3DWindow::paintGL()
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


#if 0
    // various tests of font rendering
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    static eavlTextAnnotation *t1=NULL,*t1b=NULL,*t1c=NULL, *t2=NULL,*t3=NULL,*t3b=NULL, *t4=NULL,*t4b=NULL;
    if (!t1)
    {
        t1 = new eavlScreenTextAnnotation(window,"Test 2D text, [] height=0.05 near upper-left corner (-.9,.9)",
                                          eavlColor::white, .05,
                                          -.9, 0.9);
        t1b = new eavlScreenTextAnnotation(window,"Test 2D text, [] height=0.05 at (-.9,.85) should be immediately below other 2D text",
                                           eavlColor::white, .05,
                                           -.9, 0.85);
        t1c = new eavlScreenTextAnnotation(window,"Test 2D text, [] height=0.05 at (-.9,-.9) oriented at 90 degrees",
                                           eavlColor::white, .05,
                                           -.9,-.9, 90.);
        t2 = new eavlWorldTextAnnotation(window,"Test 3D text (You), height=1.0 in 3D space at (-5,0,0), diagonal along X=Z, Y=up",
                                         eavlColor::white,
                                         1.0,
                                         -5,0,0,
                                         -1,0,1,
                                         0,1,0);
        t3 = new eavlBillboardTextAnnotation(window,"Test 3D billboard text, height=0.05 in screen space at (0,5,0)",
                                    eavlColor::white,
                                    .05,
                                    0,5,0, true);
        t3b = new eavlBillboardTextAnnotation(window,"Test 3D billboard text, height=1.0 in 3D space at (0,-12,0)",
                                    eavlColor::white,
                                    1.0,
                                    0,-12,0, false);

        t4 = new eavlBillboardTextAnnotation(window,"Test 3D billboard text, height in screen space at 90deg",
                                             eavlColor::white,
                                             .05,
                                             0,5,0, true, 90.0);
        t4b = new eavlBillboardTextAnnotation(window,"Test 3D billboard text, height in 3D space at 90deg",
                                              eavlColor::white,
                                              1.0,
                                              0,-12,0, false, 90.0);
    }


    t1->Render(window->view);
    t1b->Render(window->view);
    t1c->Render(window->view);
    t2->Render(window->view);
    t3->Render(window->view);
    t3b->Render(window->view);
    t4->Render(window->view);
    t4b->Render(window->view);
#endif

}

// ****************************************************************************
// Method:  EL3DWindow::resizeGL
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
EL3DWindow::resizeGL(int w, int h)
{
    window->Resize(w,h);
}

// ****************************************************************************
// Method:  EL3DWindow::mousePressEvent
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
EL3DWindow::mousePressEvent(QMouseEvent *mev)
{
    shiftKey = (mev->modifiers() & Qt::ShiftModifier);
    makeCurrent();

    lastx = mev->x();
    lasty = mev->y();
    mousedown = true;
    //updateGL();
}

// ****************************************************************************
// Method:  EL3DWindow::mouseMoveEvent
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
EL3DWindow::mouseMoveEvent(QMouseEvent *mev)
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
            if (shiftKey)
            {
                double dx = x2-x1;
                double dy = y2-y1;
                window->view.Pan3D(dx, dy);
            }
            else
            {
                window->view.TrackballRotate(x1,y1, x2,y2);
            }
        }
        else if (mev->buttons() & Qt::MidButton)
        {
            double zoom = y2-y1;
            
            if (shiftKey)
            {
                window->view.MoveForward3d(zoom);
            }
            else 
            {
                window->view.Zoom3D(zoom);
            }
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
// Method:  EL3DWindow::mouseReleaseEvent
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
EL3DWindow::mouseReleaseEvent(QMouseEvent *)
{
    makeCurrent();

    mousedown = false;
    shiftKey = false;
    //updateGL();
}


// ****************************************************************************
// Method:  EL3DWindow::GetSettings
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
EL3DWindow::GetSettings()
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
EL3DWindow::SomethingChanged()
{
    updateGL();
}

void
EL3DWindow::SetRendererType(const QString &type)
{
    if (type == "OpenGL")
        window->SetSceneRenderer(new eavlSceneRendererGL);
    if (type == "OpenGL (simple)")
        window->SetSceneRenderer(new eavlSceneRendererSimpleGL);
    else if (type == "RayTrace")
        window->SetSceneRenderer(new eavlSceneRendererSimpleRT);
    else
        ;
}

void
EL3DWindow::SetRendererOptions(Attribute *atts)
{
    RenderingAttributes *r = dynamic_cast<RenderingAttributes*>(atts);
    if (!r)
        return;

    eavlSceneRenderer *sr = window->GetSceneRenderer();
    sr->SetAmbientCoefficient(r->Ka);
    sr->SetDiffuseCoefficient(r->Kd);
    sr->SetSpecularCoefficient(r->Ks);
    sr->SetLightDirection(r->Lx, r->Ly, r->Lz);
    sr->SetEyeLight(r->eyeLight);
    //sr->SetPointRadius(r->pointRadius);
}
