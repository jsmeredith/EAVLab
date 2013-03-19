// Copyright 2012-2013 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef EL_1D_WINDOW_H
#define EL_1D_WINDOW_H

#include "ELWindowManager.h"

#include <QTextEdit>
#include <QGLWidget>

#include <eavlView.h>
#include <eavlDataSet.h>
#include <Plot.h>

#include "ELPlotList.h"

class eavl1DWindow;
class eavlScene;
class Pipeline;
class eavlRenderer;

// ****************************************************************************
// Class:  EL1DWindow
//
// Purpose:
///   Output window containing 1D view.
//
// Programmer:  Jeremy Meredith
// Creation:    January 16, 2013
//
// Modifications:
// ****************************************************************************
class EL1DWindow : public QGLWidget
{
    Q_OBJECT
  protected:
    ELPlotList *settings;
  public:
    EL1DWindow(ELWindowManager *parent);
    virtual void initializeGL();
    virtual void paintGL();
    virtual void resizeGL(int w, int h);

    virtual void  mouseMoveEvent(QMouseEvent*);
    virtual void  mousePressEvent(QMouseEvent*);
    virtual void  mouseReleaseEvent(QMouseEvent*);


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
    bool       barstyle;

    eavl1DWindow *window;
    eavlScene    *scene;

  public slots:
    void CurrentPipelineChanged(int index);
    void PipelineUpdated(Pipeline *p);
    void ResetView();
    bool UpdatePlots();

    void SomethingChanged();
};



#endif
