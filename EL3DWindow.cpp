// Copyright 2012 UT-Battelle, LLC.  See LICENSE.txt for more information.
#include "EL3DWindow.h"

#include <QMouseEvent>

#include <eavlColorTable.h>
#include <eavlRenderer.h>

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

    ///\todo: hack: assuming 4 pipelines
    currentPipeline = 0;
    watchedPipelines.resize(NUMPIPES+1, false);
    colortexId = 0;
    for (int i=0; i<NUMPIPES; i++)
    {
        Plot p;
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
    Plot &p = plots[index];

    p.variable_fieldindex = -1;
    p.cellset_index = -1;
    p.vmin = 0;
    p.vmax = 0;

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
    makeCurrent();
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
    dmin[0] = dmin[1] = dmin[2] = FLT_MAX;
    dmax[0] = dmax[1] = dmax[2] = -FLT_MAX;

    for (unsigned int i=0; i<plots.size(); i++)
    {
        Plot &p = plots[i];
        if (!p.data)
            continue;

        int npts = p.data->GetNumPoints();
        int dim = p.data->GetCoordinateSystem(0)->GetDimension();

        //CHIMERA HACK
        if (dim > 3)
            dim = 3;
    
        if (dim < 2 || dim > 3)
            THROW(eavlException,"only supports 2 or 3 dimensions for now");
        for (int d=0; d<dim; d++)
        {
            for (int i=0; i<npts; i++)
            {
                double v = p.data->GetPoint(i,d);
                //cerr << "findspatialextents: d="<<d<<" i="<<i<<"  v="<<v<<endl;
                if (v < dmin[d])
                    dmin[d] = v;
                if (v > dmax[d])
                    dmax[d] = v;
            }
        }
    }

    // untouched dims force to zero
    if (dmin[0] > dmax[0])
        dmin[0] = dmax[0] = 0;
    if (dmin[1] > dmax[1])
        dmin[1] = dmax[1] = 0;
    if (dmin[2] > dmax[2])
        dmin[2] = dmax[2] = 0;

    //cerr << "extents: "
    //     << dmin[0]<<":"<<dmax[0]<<"  "
    //     << dmin[1]<<":"<<dmax[1]<<"  "
    //     << dmin[2]<<":"<<dmax[2]<<"\n";

    ds_size = sqrt( (dmax[0]-dmin[0])*(dmax[0]-dmin[0]) +
                    (dmax[1]-dmin[1])*(dmax[1]-dmin[1]) +
                    (dmax[2]-dmin[2])*(dmax[2]-dmin[2]) );
                    
    center = eavlPoint3((dmax[0]+dmin[0]) / 2,
                        (dmax[1]+dmin[1]) / 2,
                        (dmax[2]+dmin[2]) / 2);

    camera.at   = center;
    camera.from = camera.at + eavlVector3(0,0, -ds_size*2);
    camera.up   = eavlVector3(0,1,0);
    camera.fov  = 0.5;
    camera.nearplane = ds_size/16.;
    camera.farplane = ds_size*4;

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
    makeCurrent();

    glClearColor(0.0, 0.2, 0.3, 1.0);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // set up matrices
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho(-1,1, -1,1, -1,1);

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    // create a color table
    ///\todo: we're only showing first-plot color table
    eavlColorTable ct(plots[0].colortable);
    if (plots[0].colortable != saved_colortable)
    {
        if (colortexId == 0)
        {
            glGenTextures(1, (GLuint*)&colortexId);
        }

        glBindTexture(GL_TEXTURE_1D, colortexId);
        // note: 2048 was NOT supported on Jeremy's Intel IGP laptop
        //       but 1024 IS.  Real NVIDIA cards can go up to 8192.
        const int n = 1024;
        float colors[n*3];
        for (int i=0; i<n; i++)
        {
            eavlColor c = ct.Map(float(i)/float(n-1));
            colors[3*i+0] = c.c[0];
            colors[3*i+1] = c.c[1];
            colors[3*i+2] = c.c[2];
        }
        glTexImage1D(GL_TEXTURE_1D, 0,
                     GL_RGB,
                     n,
                     0,
                     GL_RGB,
                     GL_FLOAT,
                     colors);

        saved_colortable = plots[0].colortable;
    }

    // draw the color table across a big 3d rectangle
    //glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    glEnable(GL_TEXTURE_1D);
    glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    //glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_WRAP_R, GL_CLAMP); // R is the 3rd coord (not alphabetical)

    if (ct.smooth)
    {
        glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }
    else
    {
        glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }
    glBindTexture(GL_TEXTURE_1D, colortexId); 
    glColor3fv(eavlColor::white.c);
    glBegin(GL_QUADS);
    glTexCoord1f(0);
    glVertex3f(-.9, .87 ,.99);
    glVertex3f(-.9, .95 ,.99);
    glTexCoord1f(1);
    glVertex3f(+.9, .95 ,.99);
    glVertex3f(+.9, .87 ,.99);
    glEnd();

    int plotcount = 0;
    for (unsigned int i=0; i<plots.size(); i++)
        plotcount += (plots[i].data) ? 1 : 0;
    if (plotcount == 0)
        return;

    // matrices
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    camera.UpdateProjectionMatrix();
    glMultMatrixf(camera.P.GetOpenGLMatrix4x4());

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    // lighting
    bool lighting = true;
    if (lighting)
    {
        bool twoSidedLighting = true;
        glShadeModel(GL_SMOOTH);
        glEnable(GL_LIGHTING);
        glEnable(GL_COLOR_MATERIAL);
        glEnable(GL_LIGHT0);
        glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, twoSidedLighting?1:0);
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, eavlColor::grey20.c);
        glLightfv(GL_LIGHT0, GL_AMBIENT, eavlColor::black.c);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, eavlColor::grey50.c);
        float lightdir[4] = {0, 0, 1, 0};
        glLightfv(GL_LIGHT0, GL_POSITION, lightdir);
        glLightfv(GL_LIGHT0, GL_SPECULAR, eavlColor::white.c);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, eavlColor::grey40.c);
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 8.0f);
    }

    camera.UpdateViewMatrix();
    glMultMatrixf(camera.V.GetOpenGLMatrix4x4());

    // render the plots
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


        try
        {
            if (p.cellset_index < 0)
            {
                if (p.pcRenderer)          p.pcRenderer->RenderPoints();
                else if (p.meshRenderer)   p.meshRenderer->RenderPoints();
            }
            else
            {
                eavlCellSet *cs = p.data->GetCellSet(p.cellset_index);
                if (cs->GetDimensionality() == 1)
                {
                    if (p.pcRenderer)          p.pcRenderer->RenderCells1D(cs);
                    else if (p.meshRenderer)   p.meshRenderer->RenderCells1D(cs);
                }
                else if (cs->GetDimensionality() == 2)
                {
                    eavlField *normals = NULL;
                    // look for face-centered surface normals first
                    for (int i=0; i<p.data->GetNumFields(); i++)
                    {
                        if (p.data->GetField(i)->GetArray()->GetName() == "surface_normals" &&
                            p.data->GetField(i)->GetAssociation() == eavlField::ASSOC_CELL_SET &&
                            p.data->GetField(i)->GetAssocCellSet() == p.cellset_index)
                        {
                            normals = p.data->GetField(i);
                        }
                    }
                    // override with node-centered ones if we have them
                    for (int i=0; i<p.data->GetNumFields(); i++)
                    {
                        if (p.data->GetField(i)->GetArray()->GetName() == "nodecentered_surface_normals" &&
                            p.data->GetField(i)->GetAssociation() == eavlField::ASSOC_POINTS)
                        {
                            normals = p.data->GetField(i);
                        }
                    }

                    if (p.pcRenderer)          p.pcRenderer->RenderCells2D(cs, normals);
                    else if (p.meshRenderer)   p.meshRenderer->RenderCells2D(cs, normals);
                }
            }
        }
        catch (const eavlException &e)
        {
            // The user can specify one cell for geometry and
            // a different one for coloring; this currently results
            // in an error; we'll just ignore it.
            cerr << e.GetErrorText() << endl;
            cerr << "-\n";
        }
    }

    // bounding box
    glDisable(GL_TEXTURE_1D);
    glDisable(GL_LIGHTING);
    glLineWidth(1);
    glColor3f(.6,.6,.6);
    glBegin(GL_LINES);
    glVertex3d(dmin[0],dmin[1],dmin[2]); glVertex3d(dmin[0],dmin[1],dmax[2]);
    glVertex3d(dmin[0],dmax[1],dmin[2]); glVertex3d(dmin[0],dmax[1],dmax[2]);
    glVertex3d(dmax[0],dmin[1],dmin[2]); glVertex3d(dmax[0],dmin[1],dmax[2]);
    glVertex3d(dmax[0],dmax[1],dmin[2]); glVertex3d(dmax[0],dmax[1],dmax[2]);

    glVertex3d(dmin[0],dmin[1],dmin[2]); glVertex3d(dmin[0],dmax[1],dmin[2]);
    glVertex3d(dmin[0],dmin[1],dmax[2]); glVertex3d(dmin[0],dmax[1],dmax[2]);
    glVertex3d(dmax[0],dmin[1],dmin[2]); glVertex3d(dmax[0],dmax[1],dmin[2]);
    glVertex3d(dmax[0],dmin[1],dmax[2]); glVertex3d(dmax[0],dmax[1],dmax[2]);

    glVertex3d(dmin[0],dmin[1],dmin[2]); glVertex3d(dmax[0],dmin[1],dmin[2]);
    glVertex3d(dmin[0],dmin[1],dmax[2]); glVertex3d(dmax[0],dmin[1],dmax[2]);
    glVertex3d(dmin[0],dmax[1],dmin[2]); glVertex3d(dmax[0],dmax[1],dmin[2]);
    glVertex3d(dmin[0],dmax[1],dmax[2]); glVertex3d(dmax[0],dmax[1],dmax[2]);
    glEnd();

    //delete[] pts;
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
    makeCurrent();
    glViewport(0, 0, w, h);

    camera.aspect = float(w)/float(h);
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
                T1.CreateTranslate(-center);
                eavlMatrix4x4 T2;
                T2.CreateTranslate(center);
                
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
            eavlVector3 view_dir((camera.at-camera.from).normalized());
            camera.at   += view_dir*(y2-y1)*ds_size;
            camera.from += view_dir*(y2-y1)*ds_size;
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
    Plot &p = plots[0];
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
