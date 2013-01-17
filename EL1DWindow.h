// Copyright 2012-2013 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef EL_1D_WINDOW_H
#define EL_1D_WINDOW_H

#include "ELWindowManager.h"

#include <QTextEdit>
#include <QGLWidget>

#include <eavlView.h>
#include <eavlDataSet.h>
#include <eavlPlot.h>

class eavlWindow;
class Pipeline;
class eavlRenderer;
class eavl2DAxisAnnotation;
class eavl2DFrameAnnotation;

// ****************************************************************************
// Class:  EL1DWindowSettings
//
// Purpose:
///   The settings widget to control options for the EL1DWindow.
//
// Programmer:  Jeremy Meredith
// Creation:    January 16, 2013
//
// Modifications:
// ****************************************************************************
class EL1DWindowSettings : public QWidget
{
    Q_OBJECT
  protected:
    QComboBox *varCombo;
  public:
    EL1DWindowSettings() : QWidget(NULL)
    {
        QGridLayout *topLayout = new QGridLayout(this);

        QComboBox *styleCombo = new QComboBox(this);
        styleCombo->addItem("Curves");
        styleCombo->addItem("Bars");
        topLayout->addWidget(new QLabel("Style:", this), 0,0);
        topLayout->addWidget(styleCombo, 1,0);
        connect(styleCombo, SIGNAL(activated(const QString&)),
                this, SIGNAL(StyleChanged(const QString&)));

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
    void StyleChanged(const QString &);
    void VarChanged(const QString &);
};

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
    int currentPipeline;
    std::vector<bool> watchedPipelines;
    EL1DWindowSettings *settings;
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

    eavlWindow *window;
    eavl2DAxisAnnotation *haxis, *vaxis;
    eavl2DFrameAnnotation *frame;
    eavlView view;

    vector<eavlPlot> plots;

  public slots:
    void CurrentPipelineChanged(int index);
    void PipelineUpdated(int index, Pipeline *p);
    void watchedPipelinesChanged(vector<bool>);
    void ResetView();
    bool UpdatePlots();

    void SettingsStyleChanged(const QString&);
    void SettingsVarChanged(const QString&);
};



#endif
