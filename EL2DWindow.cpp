// Copyright 2012-2013 UT-Battelle, LLC.  See LICENSE.txt for more information.
#include "EL2DWindow.h"

#include <QMouseEvent>
#include <QToolBar>
#include <QAction>
#include <QActionGroup>

#include <eavlColorTable.h>
#include <eavlRenderer.h>
#include <eavlWindow.h>
#include <eavlScene.h>
#include <eavlBitmapFont.h>
#include <eavlBitmapFontFactory.h>
#include <eavlPNGImporter.h>
#include <eavlTexture.h>
#include <eavlTextAnnotation.h>
#include <eavlColorBarAnnotation.h>
#include <eavl2DAxisAnnotation.h>
#include <eavl2DFrameAnnotation.h>

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

    view.vl = -.7;
    view.vr = +.7;
    view.vb = -.7;
    view.vt = +.7;
    window = new eavlWindow(view);
    scene = new eavl2DGLScene(window, view);
    colorbar = new eavlColorBarAnnotation(window);
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
        p.pcRenderer = NULL;
        p.meshRenderer = NULL;
        plots.push_back(p);
    }
}


// ****************************************************************************
// Method:  EL2DWindow::SetPipeline
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
EL2DWindow::PipelineUpdated(int index, Pipeline *pipe)
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

    UpdatePlots();
    ResetView();

    settings->UpdateFromPipeline(pipe);
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
EL2DWindow::CurrentPipelineChanged(int index)
{
    currentPipeline = index;
    UpdatePlots();
    if (watchedPipelines[0])
        updateGL();
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
    //makeCurrent();
    scene->Initialize();
}


bool
EL2DWindow::UpdatePlots()
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

        scene->plots.push_back(p);
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
    scene->ResetView();
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
    glClearColor(0.0, 0.15, 0.3, 1.0);
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

    // okay, we think it's safe to proceed now!

    scene->Paint();

    // test of font rendering
#if 0
    static eavlTextAnnotation *tt=NULL;
    if (!tt)
    {
        switch (4)
        {
          case 1:
            tt = new eavlWorldTextAnnotation(window,"Test 2D world text, [] height=0.05 at (0.3,0.3)",
                                             eavlColor::white, .05,
                                             0.3, 0.3, 0,
                                             0,0,-1,   0,1,0);
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
    tt->Setup(view);
    tt->Render();
    ///\todo: hack: should SetMatrices maybe do this?
    glViewport(0,0,view.w,view.h);
#endif

    glDisable(GL_DEPTH_TEST);

    view.SetMatricesForScreen();
    float vl, vr, vt, vb;
    view.GetReal2DViewport(vl,vr,vb,vt);

    frame->Setup(view);
    frame->SetExtents(vl,vr, vb,vt);
    frame->SetColor(eavlColor(.7,.7,.7));
    frame->Render();

    haxis->SetColor(eavlColor::white);
    haxis->SetScreenPosition(vl,vb, vr,vb);
    haxis->SetRangeForAutoTicks(view.view2d.l, view.view2d.r);
    haxis->SetMajorTickSize(0, .05, 1.0);
    haxis->SetMinorTickSize(0, .02, 1.0);
    haxis->SetLabelAnchor(0.5, 1.0);
    haxis->Setup(view);
    haxis->Render();

    vaxis->SetColor(eavlColor::white);
    vaxis->SetScreenPosition(vl,vb, vl,vt);
    vaxis->SetRangeForAutoTicks(view.view2d.b, view.view2d.t);
    vaxis->SetMajorTickSize(.05 / view.aspect, 0, 1.0);
    vaxis->SetMinorTickSize(.02 / view.aspect, 0, 1.0);
    vaxis->SetLabelAnchor(1.0, 0.47);
    vaxis->Setup(view);
    vaxis->Render();

    if (plots[0].pcRenderer)
    {
        double vmin, vmax;
        ((eavlPseudocolorRenderer*)(plots[0].pcRenderer))->GetLimits(vmin, vmax);
        colorbar->SetAxisColor(eavlColor::white);
        colorbar->SetRange(vmin, vmax, 5);
        colorbar->SetColorTable(plots[0].colortable);
        colorbar->Setup(view);
        colorbar->Render();
    }

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
    view.w = w;
    view.h = h;
    //makeCurrent();
    scene->Resize(w,h);
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
            /* simple code, fixed viewport:
            double zoom = y2 - y1;
            double factor = pow(4., zoom);
            double xc = (view.view2d.l + view.view2d.r) / 2.;
            double yc = (view.view2d.b + view.view2d.t) / 2.;
            double xs = (view.view2d.r - view.view2d.l);
            double ys = (view.view2d.t - view.view2d.b);
            xs /= factor;
            ys /= factor;
            view.view2d.l = xc - .5*xs;
            view.view2d.r = xc + .5*xs;
            view.view2d.b = yc - .5*ys;
            view.view2d.t = yc + .5*ys;*/

            double zoom = y2 - y1;
            double factor = pow(4., zoom);
            double xc = (view.view2d.l + view.view2d.r) / 2.;
            double yc = (view.view2d.b + view.view2d.t) / 2.;
            double xs = (view.view2d.r - view.view2d.l) / 2.;
            double ys = (view.view2d.t - view.view2d.b) / 2.;
            // If we're zooming in, we first want to expand the
            // viewport if possible before actually having to pull
            // the x/y region in.  We accomplish expanding the
            // viewport the horizontal/vertical direction by
            // (respectively) pulling in the y/x limits while
            // leaving the x/y limits alone.  (Or at least leaving
            // the x/y limits as large as possible.)
            double allowed_x_expansion = (view.vr - view.vl) / (vr-vl);
            double allowed_y_expansion = (view.vt - view.vb) / (vt-vb);

            /*
            cerr << "allowx = "<<allowed_x_expansion<<endl;
            cerr << "allowy = "<<allowed_y_expansion<<endl;
            cerr << "factor = "<<factor<<endl;
            cerr << endl;
            */

            if (zoom > 0 && allowed_x_expansion>1.01)
            {
                // not using this:
                //double xfactor = factor;
                //if (allowed_x_expansion > xfactor)
                //    xfactor = 1;
                //else
                //    xfactor /= allowed_x_expansion;

                bool in_l = xc - xs/factor < view.minextents[0];
                bool in_r = xc + xs/factor > view.maxextents[0];
                if (in_l && in_r)
                {
                    view.view2d.l = xc - xs/factor;
                    view.view2d.r = xc + xs/factor;
                }
                else if (in_l)
                {
                    view.view2d.l = xc - xs/(factor*factor);
                }
                else if (in_r)
                {
                    view.view2d.r = xc + xs/(factor*factor);
                }

                view.view2d.b = yc - ys/factor;
                view.view2d.t = yc + ys/factor;
            }
            else if (zoom > 0 && allowed_y_expansion>1.01)
            {
                // not using this:
                //double yfactor = factor;
                //if (allowed_y_expansion > yfactor)
                //    yfactor = 1;
                //else
                //    yfactor /= allowed_y_expansion;

                bool in_b = yc - ys/factor < view.minextents[1];
                bool in_t = yc + ys/factor > view.maxextents[1];
                if (in_b && in_t)
                {
                    view.view2d.b = yc - ys/factor;
                    view.view2d.t = yc + ys/factor;
                }
                else if (in_b)
                {
                    view.view2d.b = yc - ys/(factor*factor);
                }
                else if (in_t)
                {
                    view.view2d.t = yc + ys/(factor*factor);
                }

                view.view2d.l = xc - xs/factor;
                view.view2d.r = xc + xs/factor;
            }
            else
            {
                view.view2d.l = xc - xs/factor;
                view.view2d.r = xc + xs/factor;
                view.view2d.b = yc - ys/factor;
                view.view2d.t = yc + ys/factor;
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
// Method:  EL2DWindow::watchedPipelinesChanged
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
EL2DWindow::watchedPipelinesChanged(vector<bool> watched)
{
    watchedPipelines = watched;
    updateGL();
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
        settings = new EL2DWindowSettings;
        connect(settings, SIGNAL(ColorTableChanged(const QString&)),
                this, SLOT(SettingsColorTableChanged(const QString&)));
        connect(settings, SIGNAL(VarChanged(const QString&)),
                this, SLOT(SettingsVarChanged(const QString&)));
    }
    return settings;
}

// ****************************************************************************
// Method:  EL2DWindow::SettingsColorTableChanged
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
EL2DWindow::SettingsColorTableChanged(const QString &ct)
{
    ///\todo: just prototyping; only affect plot 0
    plots[0].colortable = ct.toStdString();
    updateGL();
}

// ****************************************************************************
// Method:  EL2DWindow::SettingsVarChanged
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
EL2DWindow::SettingsVarChanged(const QString &var)
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
