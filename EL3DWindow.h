// Copyright 2012-2013 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef EL_3D_WINDOW_H
#define EL_3D_WINDOW_H

#include "ELWindowManager.h"

#include <QTextEdit>
#include <QGLWidget>

#include <eavlView.h>
#include <eavlDataSet.h>
#include <Plot.h>

#include "ELPlotList.h"

class eavl3DWindow;
class eavlScene;
class Pipeline;
class eavlRenderer;
class eavlColorBarAnnotation;
class eavlBoundingBoxAnnotation;
class eavl3DAxisAnnotation;

// ****************************************************************************
// Class:  EL3DWindow
//
// Purpose:
///   Output window containing 3D view.
//
// Programmer:  Jeremy Meredith
// Creation:    August 15, 2012
//
// Modifications:
// ****************************************************************************
class EL3DWindow : public QGLWidget
{
    Q_OBJECT
  protected:
    ELPlotList *settings;
  public:
    EL3DWindow(ELWindowManager *parent);
    virtual void initializeGL();
    virtual void paintGL();
    virtual void resizeGL(int w, int h);

    virtual void  mouseMoveEvent(QMouseEvent*);
    virtual void  mousePressEvent(QMouseEvent*);
    virtual void  mouseReleaseEvent(QMouseEvent*);

    void SetRendererType(const QString&);

    QWidget *GetSettings();
    /*
    virtual void contextMenuEvent(QContextMenuEvent*); 

    void SetData(eavlDataSet *d);
    void SetColorTable(const QString &ct);
    void SetVariable(int,int);
    void SetCellSet(int);
    void SetShowGhosts(bool);
    void SetShowMesh(bool);
    */
  signals:

  private:
    QMenu     *popup;
    bool       mousedown, shiftKey;
    int        lastx, lasty;
    bool       showghosts;
    bool       showmesh;

    eavl3DWindow *window;
    eavlScene    *scene;

  public slots:
    void CurrentPipelineChanged(int index);
    void PipelineUpdated(Pipeline *p);
    void ResetView();
    bool UpdatePlots();

    void SomethingChanged();
};



#endif
