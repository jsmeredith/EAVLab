// Copyright 2012-2013 UT-Battelle, LLC.  See LICENSE.txt for more information.
#include "EL3DWindow.h"

#include <QMouseEvent>
#include <QToolBar>
#include <QAction>
#include <QActionGroup>

#include <eavlColorTable.h>
#include <eavlRenderer.h>
#include <eavl3DWindow.h>
#include <eavlScene.h>
#include <eavlTexture.h>
#include <eavlTextAnnotation.h>

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

    scene = new eavl3DGLScene();
    window = new eavl3DWindow(eavlColor(0.15, 0.0, 0.25), NULL, scene);

    ///\todo: hack: assuming 4 pipelines
    currentPipeline = 0;
    watchedPipelines.resize(NUMPIPES+1, false);
    for (int i=0; i<NUMPIPES; i++)
    {
        Plot p;
        p.data = NULL;
        p.colortable = "dense";
        p.variable_fieldindex = -1;
        p.renderer = NULL;
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
    //cerr << "EL3DWindow::PipelineUpdated\n";
    Plot &p = plots[index];

    p.variable_fieldindex = -1;
    p.cellset_index = -1;

    p.data = pipe->result;

    if (p.renderer)
        delete p.renderer;
    p.renderer = NULL;

    UpdatePlots();
    ResetView();

    settings->UpdateFromPipeline(pipe);

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
    //cerr << "EL3DWindow::CurrentPipelineChanged\n";
    currentPipeline = index;
    UpdatePlots();
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
}

bool
EL3DWindow::UpdatePlots()
{
    //cerr << "EL3DWindow::UpdatePlots\n";
    bool shoulddraw = false;
    scene->plots.clear();
    for (unsigned int i=0;  i<plots.size(); i++)
    {
        bool watchingCurrent = watchedPipelines[0];
        bool watchingThis = watchedPipelines[i+1];
        //cerr << "i="<<i<<" watchingCurrent="<<watchingCurrent
        //     <<" watchingThis="<<watchingThis<<endl;
        if (!(watchingCurrent && currentPipeline==(int)i) &&
            !watchingThis)
            continue;

        Plot &p = plots[i];
        if (!p.data)
            continue;
        shoulddraw = true;

        if (!p.renderer && p.variable_fieldindex >= 0)
        {
            p.renderer = new eavlPseudocolorRenderer(p.data, 
                                                     p.colortable,
                                                     p.cellset_index < 0 ? "" : p.data->GetCellSet(p.cellset_index)->GetName(),
                                                     p.data->GetField(p.variable_fieldindex)->GetArray()->GetName());
        }
        if (!p.renderer)
        {
            p.renderer = new eavlSingleColorRenderer(p.data, 
                                                     eavlColor::white,
                                                     p.cellset_index < 0 ? "" : p.data->GetCellSet(p.cellset_index)->GetName());
        }

        scene->plots.push_back(p.renderer);
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
                                         1,0,-1,
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


    t1->Render(view);
    t1b->Render(view);
    t1c->Render(view);
    t2->Render(view);
    t3->Render(view);
    t3b->Render(view);
    t4->Render(view);
    t4b->Render(view);
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
            window->view.Zoom3D(zoom);
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
    //cerr << "EL3DWindow::SettingsColorTableChanged\n";
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
    //cerr << "EL3DWindow::SettingsVarChanged\n";
    ///\todo: just prototyping; only affect plot 0
    Plot &p = plots[0];
    delete p.renderer;
    p.renderer = NULL;
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
