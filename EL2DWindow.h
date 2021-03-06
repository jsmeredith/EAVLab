 // Copyright 2012-2013 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef EL_2D_WINDOW_H
#define EL_2D_WINDOW_H

#include "ELWindowManager.h"

#include <QTextEdit>
#include <QGLWidget>
#include <QTreeWidget>
#include <QGroupBox>
#include <QHeaderView>
#include <QColorDialog>
#include <QCheckBox>

#include <eavlView.h>
#include <eavlDataSet.h>
#include <Plot.h>

#include "ELPlotList.h"

class eavl2DWindow;
class eavlScene;
class Pipeline;
class eavlRenderer;


// ****************************************************************************
// Class:  EL2DWindow
//
// Purpose:
///   Output window containing 2D view.
//
// Programmer:  Jeremy Meredith
// Creation:    January 10, 2013
//
// Modifications:
// ****************************************************************************
class EL2DWindow : public QGLWidget
{
    Q_OBJECT
  protected:
    ELPlotList *settings;
  public:
    EL2DWindow(ELWindowManager *parent);
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

    eavl2DWindow *window;
    eavlScene    *scene;

  public slots:
    void CurrentPipelineChanged(int index);
    void PipelineUpdated(Pipeline *p);
    void ResetView();
    bool UpdatePlots();

    void SomethingChanged();
};



#endif
