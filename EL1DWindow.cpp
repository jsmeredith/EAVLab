// Copyright 2012-2013 UT-Battelle, LLC.  See LICENSE.txt for more information.
#include "EL1DWindow.h"

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
#include <eavl2DAxisAnnotation.h>
#include <eavl2DFrameAnnotation.h>

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

    view.viewtype = eavlView::EAVL_VIEW_2D;
    view.vl = -.7;
    view.vr = +.7;
    view.vb = -.7;
    view.vt = +.7;
    window = new eavl1DGLWindow(view);
    haxis = new eavl2DAxisAnnotation(window);
    vaxis = new eavl2DAxisAnnotation(window);
    frame = new eavl2DFrameAnnotation(window);

    ///\todo: hack: assuming 4 pipelines
    currentPipeline = 0;
    watchedPipelines.resize(NUMPIPES+1, false);
    for (int i=0; i<NUMPIPES; i++)
    {
        eavlPlot p;
        p.data = NULL;
        p.colortable = "dense";
        p.variable_fieldindex = -1;
        plots.push_back(p);
    }
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
EL1DWindow::PipelineUpdated(int index, Pipeline *pipe)
{
    eavlPlot &p = plots[index];

    p.variable_fieldindex = -1;
    p.cellset_index = -1;

    p.data = pipe->result;

    if (p.curveRenderer)
        delete p.curveRenderer;
    p.curveRenderer = NULL;
    if (p.barRenderer)
        delete p.barRenderer;
    p.barRenderer = NULL;

    UpdatePlots();
    ResetView();

    settings->UpdateFromPipeline(pipe);
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
    currentPipeline = index;
    UpdatePlots();
    if (watchedPipelines[0])
        updateGL();
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
    //makeCurrent();
    window->Initialize();
}


bool
EL1DWindow::UpdatePlots()
{
    //cerr << "EL3DWindow::UpdatePlots\n";
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

        if (!barstyle && !p.curveRenderer && p.variable_fieldindex >= 0)
        {
            p.curveRenderer = new eavlCurveRenderer(p.data, 
                                                    eavlColor(0,.7,0),
                                                    p.data->GetField(p.variable_fieldindex)->GetArray()->GetName());
        }
        if (barstyle && !p.barRenderer && p.variable_fieldindex >= 0)
        {
            p.barRenderer = new eavlBarRenderer(p.data, 
                                                eavlColor(0,.7,0),
                                                .10, // gap is 10% of bar width
                                                p.data->GetField(p.variable_fieldindex)->GetArray()->GetName());
        }

        window->plots.push_back(p);
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
    window->ResetView();
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
    glClearColor(1,1,1, 1.0);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

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

    glDisable(GL_DEPTH_TEST);

    view.SetMatricesForScreen();
    float vl, vr, vt, vb;
    view.GetReal2DViewport(vl,vr,vb,vt);

    frame->Setup(view);
    frame->SetExtents(vl,vr, vb,vt);
    frame->SetColor(eavlColor(.5,.5,.5));
    frame->Render();

    haxis->SetColor(eavlColor::black);
    haxis->SetScreenPosition(vl,vb, vr,vb);
    haxis->SetRangeForAutoTicks(view.view2d.l, view.view2d.r);
    haxis->SetMajorTickSize(0, .05, 1.0);
    haxis->SetMinorTickSize(0, .02, 1.0);
    haxis->SetLabelAnchor(0.5, 1.0);
    haxis->Setup(view);
    haxis->Render();

    vaxis->SetColor(eavlColor::black);
    vaxis->SetScreenPosition(vl,vb, vl,vt);
    vaxis->SetRangeForAutoTicks(view.view2d.b, view.view2d.t);
    vaxis->SetMajorTickSize(.05 / view.aspect, 0, 1.0);
    vaxis->SetMinorTickSize(.02 / view.aspect, 0, 1.0);
    vaxis->SetLabelAnchor(1.0, 0.47);
    vaxis->Setup(view);
    vaxis->Render();


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
    view.w = w;
    view.h = h;
    //makeCurrent();
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

        float vl, vr, vt, vb;
        view.GetReal2DViewport(vl,vr,vb,vt);

        float x1 =  ((float(lastx*2)/float(width()))  - 1.0);
        float y1 = -((float(lasty*2)/float(height())) - 1.0);
        float x2 =  ((float(  x  *2)/float(width()))  - 1.0);
        float y2 = -((float(  y  *2)/float(height())) - 1.0);

        if (mev->buttons() & Qt::LeftButton)
        {
            float xpan = (x2-x1) * (view.view2d.r-view.view2d.l) / (vr - vl);
            float ypan = (y2-y1) * (view.view2d.t-view.view2d.b) / (vt - vb);

            view.view2d.l -= xpan;
            view.view2d.r -= xpan;

            view.view2d.t -= ypan;
            view.view2d.b -= ypan;
        }
        else if (mev->buttons() & Qt::MidButton)
        {
            double zoom = y2 - y1;
            double factor = pow(4., zoom);
            double xc = (view.view2d.l + view.view2d.r) / 2.;
            double yc = (view.view2d.b + view.view2d.t) / 2.;
            double xs = (view.view2d.r - view.view2d.l) / 2.;
            double ys = (view.view2d.t - view.view2d.b) / 2.;

            view.view2d.l = xc - xs/factor;
            view.view2d.r = xc + xs/factor;
            view.view2d.b = yc - ys/factor;
            view.view2d.t = yc + ys/factor;
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
// Method:  EL1DWindow::watchedPipelinesChanged
//
// Purpose:
///   Change which pipelines this window should watch.
//
// Arguments:
//   watched    the new vector of size NUMPIPES+1 for new pipelines watch set
//
// Programmer:  Jeremy Meredith
// Creation:    January 17, 2013
//
// Modifications:
// ****************************************************************************
void
EL1DWindow::watchedPipelinesChanged(vector<bool> watched)
{
    watchedPipelines = watched;
    updateGL();
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
        settings = new EL1DWindowSettings;
        connect(settings, SIGNAL(StyleChanged(const QString&)),
                this, SLOT(SettingsStyleChanged(const QString&)));
        connect(settings, SIGNAL(VarChanged(const QString&)),
                this, SLOT(SettingsVarChanged(const QString&)));
    }
    return settings;
}

// ****************************************************************************
// Method:  EL1DWindow::SettingsColorTableChanged
//
// Purpose:
///   Slot for when the color table in the settings has changed.
//
// Arguments:
//   ct         the new color table name
//
// Programmer:  Jeremy Meredith
// Creation:    January 17, 2013
//
// Modifications:
// ****************************************************************************
void
EL1DWindow::SettingsStyleChanged(const QString &style)
{
    ///\todo: just prototyping; only affect plot 0
    for (unsigned int i=0;  i<plots.size(); i++)
    {
        eavlPlot &p = plots[i];
        delete p.curveRenderer;
        p.curveRenderer = NULL;
        delete p.barRenderer;
        p.barRenderer = NULL;
    }
    barstyle = (style == "Bars");
    updateGL();
}

// ****************************************************************************
// Method:  EL1DWindow::SettingsVarChanged
//
// Purpose:
///   Slot for when the variable in the settings has changed.
//
// Arguments:
//   var        the new variable name
//
// Programmer:  Jeremy Meredith
// Creation:    January 17, 2013
//
// Modifications:
//   Jeremy Meredith, Thu Nov 29 12:19:56 EST 2012
//   Try to keep the same cell set selected after an update.
//
// ****************************************************************************
void
EL1DWindow::SettingsVarChanged(const QString &var)
{
    ///\todo: just prototyping; only affect plot 0
    eavlPlot &p = plots[0];
    delete p.curveRenderer;
    p.curveRenderer = NULL;
    delete p.barRenderer;
    p.barRenderer = NULL;
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

    ///\todo: HACK
    ResetView();

    updateGL();
}
