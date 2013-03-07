// Copyright 2012-2013 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef EL_3D_WINDOW_H
#define EL_3D_WINDOW_H

#include "ELWindowManager.h"

#include <QTextEdit>
#include <QGLWidget>

#include <eavlView.h>
#include <eavlDataSet.h>
#include <Plot.h>

class eavl3DWindow;
class eavlScene;
class Pipeline;
class eavlRenderer;
class eavlColorBarAnnotation;
class eavlBoundingBoxAnnotation;
class eavl3DAxisAnnotation;

// ****************************************************************************
// Class:  EL3DWindowSettings
//
// Purpose:
///   The settings widget to control options for the EL3DWindow.
//
// Programmer:  Jeremy Meredith
// Creation:    August 17, 2012
//
// Modifications:
// ****************************************************************************
class EL3DWindowSettings : public QWidget
{
    Q_OBJECT
  protected:
    QComboBox *varCombo;
  public:
    EL3DWindowSettings() : QWidget(NULL)
    {
        QGridLayout *topLayout = new QGridLayout(this);

        QComboBox *ctCombo = new QComboBox(this);
        ctCombo->addItem("dense");
        ctCombo->addItem("sharp");
        ctCombo->addItem("thermal");
        ctCombo->addItem("blue");
        ctCombo->addItem("orange");
        ctCombo->addItem("levels");
        topLayout->addWidget(new QLabel("Color Table:", this), 0,0);
        topLayout->addWidget(ctCombo, 1,0);
        connect(ctCombo, SIGNAL(activated(const QString&)),
                this, SIGNAL(ColorTableChanged(const QString&)));

        varCombo = new QComboBox(this);
        connect(varCombo, SIGNAL(activated(const QString&)),
                this, SIGNAL(VarChanged(const QString&)));
        topLayout->addWidget(new QLabel("Variable:", this), 2,0);
        topLayout->addWidget(varCombo, 3,0);

        topLayout->setRowStretch(4, 100);
    }
    void UpdateFromPipeline(Pipeline *p)
    {
        QString oldvar = varCombo->currentText();
        int newindex = -1;
        varCombo->clear();
        vector<string> vars = p->GetVariables();
        for (size_t i = 0; i < vars.size(); i++)
        {
            varCombo->addItem(vars[i].c_str());
            if (oldvar == vars[i].c_str())
                newindex = i;
        }
        if (newindex >= 0)
            varCombo->setCurrentIndex(newindex);
        emit VarChanged(varCombo->currentText());
    }
  signals:
    void ColorTableChanged(const QString &);
    void VarChanged(const QString &);
};

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
    int currentPipeline;
    std::vector<bool> watchedPipelines;
    EL3DWindowSettings *settings;
  public:
    EL3DWindow(ELWindowManager *parent);
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

    eavl3DWindow *window;
    eavlScene    *scene;

    vector<Plot> plots;

  public slots:
    void CurrentPipelineChanged(int index);
    void PipelineUpdated(int index, Pipeline *p);
    void watchedPipelinesChanged(vector<bool>);
    void ResetView();
    bool UpdatePlots();

    void SettingsColorTableChanged(const QString&);
    void SettingsVarChanged(const QString&);
};



#endif
