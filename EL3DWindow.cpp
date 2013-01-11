// Copyright 2012-2013 UT-Battelle, LLC.  See LICENSE.txt for more information.
#include "EL3DWindow.h"

#include <QMouseEvent>
#include <QToolBar>
#include <QAction>
#include <QActionGroup>

#include <eavlColorTable.h>
#include <eavlRenderer.h>
#include <eavlWindow.h>
#include <eavlBitmapFont.h>
#include <eavlBitmapFontFactory.h>
#include <eavlPNGImporter.h>
#include <eavlTexture.h>
#include <eavlTextAnnotation.h>
#include <eavlColorBarAnnotation.h>
#include <eavlBoundingBoxAnnotation.h>

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
    settings = NULL;

    mousedown = false;
    shiftKey = false;
    lastx = lasty = -1;
    showghosts = false;
    showmesh = false;

    window = new eavl3DGLWindow;
    colorbar = new eavlColorBarAnnotation(window);
    bbox = new eavlBoundingBoxAnnotation(window);

    ///\todo: hack: assuming 4 pipelines
    currentPipeline = 0;
    watchedPipelines.resize(NUMPIPES+1, false);
    for (int i=0; i<NUMPIPES; i++)
    {
        eavlPlot p;
        p.data = NULL;
        p.colortable = "dense";
        p.variable_fieldindex = -1;
        p.pcRenderer = NULL;
        p.meshRenderer = NULL;
        plots.push_back(p);
    }
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
EL3DWindow::PipelineUpdated(int index, Pipeline *pipe)
{
    eavlPlot &p = plots[index];

    p.variable_fieldindex = -1;
    p.cellset_index = -1;

    p.data = pipe->result;

    if (p.pcRenderer)
        delete p.pcRenderer;
    p.pcRenderer = NULL;
    if (p.meshRenderer)
        delete p.meshRenderer;
    p.meshRenderer = NULL;

    settings->UpdateFromPipeline(pipe);

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
EL3DWindow::CurrentPipelineChanged(int index)
{
    currentPipeline = index;
    if (watchedPipelines[0])
        updateGL();
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
    //makeCurrent();
    window->Initialize();
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
    window->ResetView();
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
    glClearColor(0.0, 0.15, 0.3, 1.0);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    bool shoulddraw = false;
    window->plots.clear();
    for (unsigned int i=0;  i<plots.size(); i++)
    {
        bool watchingCurrent = watchedPipelines[0];
        bool watchingThis = watchedPipelines[i+1];
        //cerr << "i="<<i<<" watchingCurrent="<<watchingCurrent
        //     <<" watchingThis="<<watchingThis<<endl;
        if (!(watchingCurrent && currentPipeline==(int)i) &&
            !watchingThis)
            continue;

        eavlPlot &p = plots[i];
        if (!p.data)
            continue;
        shoulddraw = true;

        if (!p.pcRenderer && p.variable_fieldindex >= 0)
        {
            p.pcRenderer = new eavlPseudocolorRenderer(p.data, 
                                                       p.colortable,
                                                       p.data->GetField(p.variable_fieldindex)->GetArray()->GetName());
        }
        if (!p.meshRenderer)
        {
            p.meshRenderer = new eavlSingleColorRenderer(p.data, 
                                                         eavlColor::white);
        }

        window->plots.push_back(p);
    }

    window->Paint();

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

#if 0
    // test of font rendering
    static eavlTextAnnotation *t1=NULL,*t1b=NULL,*t1c=NULL, *t2=NULL,*t3=NULL,*t3b=NULL, *t4=NULL,*t4b=NULL;
    if (!t1)
    {
        t1 = new eavlScreenTextAnnotation(window,"Test 2D text, [] height=0.03 near upper-left corner (.1,.9)",
                                          eavlColor::white, .03,
                                          0.1, 0.9);
        t1b = new eavlScreenTextAnnotation(window,"Test 2D text, [] height=0.03 at (.1,.87) should be immediately below other 2D text",
                                           eavlColor::white, .03,
                                           0.1, 0.87);
        t1c = new eavlScreenTextAnnotation(window,"Test 2D text, [] height=0.03 at (.1,.1) oriented at 90 degrees",
                                           eavlColor::white, .03,
                                           0.1, 0.1, 90.);
        t2 = new eavlWorldTextAnnotation(window,"Test 3D text (You), height=1.0 in 3D space at (-5,0,0), diagonal along X=Z, Y=up",
                                         eavlColor::white,
                                         1.0,
                                         -5,0,0,
                                         1,0,-1,
                                         0,1,0);
        t3 = new eavlBillboardTextAnnotation(window,"Test 3D billboard text, height=0.03 in screen space at (0,5,0)",
                                    eavlColor::white,
                                    .03,
                                    0,5,0, true);
        t3b = new eavlBillboardTextAnnotation(window,"Test 3D billboard text, height=1.0 in 3D space at (0,-12,0)",
                                    eavlColor::white,
                                    1.0,
                                    0,-12,0, false);

        t4 = new eavlBillboardTextAnnotation(window,"Test 3D billboard text, height in screen space at 90deg",
                                             eavlColor::white,
                                             .03,
                                             0,5,0, true, 90.0);
        t4b = new eavlBillboardTextAnnotation(window,"Test 3D billboard text, height in 3D space at 90deg",
                                              eavlColor::white,
                                              1.0,
                                              0,-12,0, false, 90.0);
    }


    t1->Setup(window->camera);
    t1->Render();

    t1b->Setup(window->camera);
    t1b->Render();

    t1c->Setup(window->camera);
    t1c->Render();

    t2->Setup(window->camera);
    t2->Render();

    t3->Setup(window->camera);
    t3->Render();

    t3b->Setup(window->camera);
    t3b->Render();

    t4->Setup(window->camera);
    t4->Render();

    t4b->Setup(window->camera);
    t4b->Render();
#endif

    colorbar->SetColorTable(plots[0].colortable);
    colorbar->Setup(window->camera);
    colorbar->Render();

    bbox->SetExtents(((eavl3DGLWindow*)window)->dmin[0],
                     ((eavl3DGLWindow*)window)->dmax[0],
                     ((eavl3DGLWindow*)window)->dmin[1],
                     ((eavl3DGLWindow*)window)->dmax[1],
                     ((eavl3DGLWindow*)window)->dmin[2],
                     ((eavl3DGLWindow*)window)->dmax[2]);
    bbox->Setup(window->camera);
    bbox->Render();
                     
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
    //makeCurrent();
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
        eavlCamera &camera = window->camera;
        float x1 =  ((float(lastx*2)/float(width()))  - 1.0);
        float y1 = -((float(lasty*2)/float(height())) - 1.0);
        float x2 =  ((float(  x  *2)/float(width()))  - 1.0);
        float y2 = -((float(  y  *2)/float(height())) - 1.0);

        if (mev->buttons() & Qt::LeftButton)
        {
            if (shiftKey)
            {
                eavlMatrix4x4 R;
                R.CreateRBT(camera.from, camera.at, camera.up);
                eavlVector3 pan(5.0*(x1-x2),5.0*(y1-y2),0);
                pan = R*pan;
                eavlMatrix4x4 T;
                T.CreateTranslate(pan);
                camera.from = T*camera.from;
                camera.at   = T*camera.at;
            }
            else
            {
                eavlMatrix4x4 R1;
                R1.CreateTrackball(-x1,-y1, -x2,-y2);
                eavlMatrix4x4 T1;
                T1.CreateTranslate(-camera.at);
                eavlMatrix4x4 T2;
                T2.CreateTranslate(camera.at);
                
                eavlMatrix4x4 V1(camera.V);
                V1.m[0][3]=0;
                V1.m[1][3]=0;
                V1.m[2][3]=0;
                eavlMatrix4x4 V2(V1);
                V2.Transpose();
                
                eavlMatrix4x4 MM = T2 * V2 * R1 * V1 * T1;
                
                camera.from = MM*camera.from;
                camera.at   = MM*camera.at;
                camera.up   = MM*camera.up;
            }
        }
        else if (mev->buttons() & Qt::MidButton)
        {
            ///\todo: disabled zoom for now; bad way to do this...
            /*
            eavlVector3 view_dir((camera.at-camera.from).normalized());
            camera.at   += view_dir*(y2-y1)*ds_size;
            camera.from += view_dir*(y2-y1)*ds_size;
            */
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
// Method:  EL3DWindow::watchedPipelinesChanged
//
// Purpose:
///   Change which pipelines this window should watch.
//
// Arguments:
//   watched    the new vector of size NUMPIPES+1 for new pipelines watch set
//
// Programmer:  Jeremy Meredith
// Creation:    August 16, 2012
//
// Modifications:
// ****************************************************************************
void
EL3DWindow::watchedPipelinesChanged(vector<bool> watched)
{
    watchedPipelines = watched;
    updateGL();
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
        settings = new EL3DWindowSettings;
        connect(settings, SIGNAL(ColorTableChanged(const QString&)),
                this, SLOT(SettingsColorTableChanged(const QString&)));
        connect(settings, SIGNAL(VarChanged(const QString&)),
                this, SLOT(SettingsVarChanged(const QString&)));
    }
    return settings;
}

// ****************************************************************************
// Method:  EL3DWindow::SettingsColorTableChanged
//
// Purpose:
///   Slot for when the color table in the settings has changed.
//
// Arguments:
//   ct         the new color table name
//
// Programmer:  Jeremy Meredith
// Creation:    August 20, 2012
//
// Modifications:
// ****************************************************************************
void
EL3DWindow::SettingsColorTableChanged(const QString &ct)
{
    ///\todo: just prototyping; only affect plot 0
    plots[0].colortable = ct.toStdString();
    updateGL();
}

// ****************************************************************************
// Method:  EL3DWindow::SettingsVarChanged
//
// Purpose:
///   Slot for when the variable in the settings has changed.
//
// Arguments:
//   var        the new variable name
//
// Programmer:  Jeremy Meredith
// Creation:    August 20, 2012
//
// Modifications:
//   Jeremy Meredith, Thu Nov 29 12:19:56 EST 2012
//   Try to keep the same cell set selected after an update.
//
// ****************************************************************************
void
EL3DWindow::SettingsVarChanged(const QString &var)
{
    ///\todo: just prototyping; only affect plot 0
    eavlPlot &p = plots[0];
    delete p.pcRenderer;
    p.pcRenderer = NULL;
    p.variable_fieldindex = -1;
    p.cellset_index = -1;
    if (p.data)
    {
        for (int i=0; i<p.data->GetNumFields(); i++)
        {
            ///\todo: we're taking the *last* field with this name,
            /// the theory being that e.g. if someone adds an extface
            /// operator, it uses the same variable name with a new
            /// cell set later in the lst.  so we want the last cell set.
            /// this is a bit hack-ish.
            if (p.data->GetField(i)->GetArray()->GetName() == var.toStdString())
            {
                p.variable_fieldindex = i;
                if (p.data->GetField(i)->GetAssociation() == eavlField::ASSOC_CELL_SET)
                {
                    p.cellset_index = p.data->GetField(i)->GetAssocCellSet();
                }
                else
                {
                    p.cellset_index = p.data->GetNumCellSets()-1;
                }
                // don't put a break; here. see above
            }
        }
        if (p.cellset_index == -1)
        {
            for (int i=0; i<p.data->GetNumCellSets(); i++)
            {
                if (p.data->GetCellSet(i)->GetName() == var.toStdString())
                {
                    p.cellset_index = i;
                    break;
                }
            }
        }
    }
    updateGL();
}
