 // Copyright 2012-2013 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef EL_2D_WINDOW_H
#define EL_2D_WINDOW_H

#include "ELWindowManager.h"

#include <QTextEdit>
#include <QGLWidget>
#include <QTreeWidget>
#include <QGroupBox>

#include <eavlView.h>
#include <eavlDataSet.h>
#include <Plot.h>

class eavl2DWindow;
class eavlScene;
class Pipeline;
class eavlRenderer;

// ****************************************************************************
// Class:  
//
// Purpose:
///   
//
// Programmer:  Jeremy Meredith
// Creation:    March 12, 2013
//
// Modifications:
// ****************************************************************************
class EL2DPlotSettings : public QWidget
{
    Q_OBJECT
  protected:
    QComboBox *pipelineCombo;
    QComboBox *varCombo;
    QComboBox *ctCombo;
    Plot *plot;
    vector<string> cellsets_for_var;
    vector<string> fields_for_var;
  public:
    EL2DPlotSettings(QWidget *p) : QWidget(p)
    {
        plot = NULL;

        QGridLayout *topLayout = new QGridLayout(this);

        int srow = 0;
        pipelineCombo = new QComboBox(this);
        topLayout->addWidget(new QLabel("Pipeline Source:", this), srow,0);
        srow++;
        topLayout->addWidget(pipelineCombo, srow,0);
        srow++;
        connect(pipelineCombo, SIGNAL(activated(const QString&)),
                this, SLOT(PipelineChanged(const QString&)));

        varCombo = new QComboBox(this);
        connect(varCombo, SIGNAL(activated(const QString&)),
                this, SLOT(VarChanged(const QString&)));
        topLayout->addWidget(new QLabel("Field:", this), srow,0);
        srow++;
        topLayout->addWidget(varCombo, srow,0);
        srow++;

        ctCombo = new QComboBox(this);
        ctCombo->addItem("default");
        ctCombo->addItem("dense");
        ctCombo->addItem("sharp");
        ctCombo->addItem("thermal");
        ctCombo->addItem("blue");
        ctCombo->addItem("orange");
        ctCombo->addItem("levels");
        topLayout->addWidget(new QLabel("Color Table:", this), srow,0);
        srow++;
        topLayout->addWidget(ctCombo, srow,0);
        srow++;
        connect(ctCombo, SIGNAL(activated(const QString&)),
                this, SLOT(ColorTableChanged(const QString&)));

    }
    void PipelineUpdated(Pipeline *p)
    {
        // rebuild the pipeline combo box
        // NOTE: this doesn't try to keep it the same!
        int index = 0;
        pipelineCombo->clear();
        for (int i=0; i<Pipeline::allPipelines.size(); i++)
        {
            pipelineCombo->addItem(Pipeline::allPipelines[i]->GetName().c_str());
        }

        // NOTE: this is currently keyed on the ACTIVE pipeline,
        // not the one selected in the pipeline combo!  FIX THIS!
        RebuildVarCombo();
    }
    void RebuildVarCombo()
    {
        // rebuild the field combo box
        int newindex = -1;
        varCombo->clear();

        if (!plot)
            return;
        Pipeline *p = plot->pipe;
        if (!p)
            return;

        DSInfo dsinfo = p->GetVariables(-1);
        varCombo->addItem("Points");
        cellsets_for_var.push_back("");
        fields_for_var.push_back("");

        for (int i=0; i<dsinfo.nodalfields.size(); ++i)
        {
            varCombo->addItem(QString("      ") + dsinfo.nodalfields[i].c_str());
            cellsets_for_var.push_back("");
            fields_for_var.push_back(dsinfo.nodalfields[i]);
        }

        for (int k=0; k<dsinfo.cellsets.size(); ++k)
        {
            string csname = dsinfo.cellsets[k];
            varCombo->addItem(csname.c_str());
            cellsets_for_var.push_back(dsinfo.cellsets[k]);
            fields_for_var.push_back("");
            for (int i=0; i<dsinfo.nodalfields.size(); ++i)
            {
                varCombo->addItem(QString("      ") + dsinfo.nodalfields[i].c_str());
                cellsets_for_var.push_back(csname);
                fields_for_var.push_back(dsinfo.nodalfields[i]);
            }
            for (int i=0; i<dsinfo.cellsetfields[csname].size(); ++i)
            {
                varCombo->addItem(QString("      ") + dsinfo.cellsetfields[csname][i].c_str());
                cellsets_for_var.push_back(csname);
                fields_for_var.push_back(dsinfo.cellsetfields[csname][i]);
            }
        }
        /*
        for (size_t i = 0; i < vars.size(); i++)
        {
            varCombo->addItem(vars[i].c_str());
            if (plot->field == vars[i].c_str())
                newindex = i;
        }
        */
        if (newindex >= 0)
            varCombo->setCurrentIndex(newindex);

        emit SomethingChanged();
    }
    void NewPlotSelected(Plot *p)
    {
        plot = p;
        RebuildVarCombo();
        bool found = false;
        for (int i=0; i<ctCombo->count(); ++i)
        {
            if (ctCombo->itemText(i) == p->colortable.c_str())
            {
                ctCombo->setCurrentIndex(i);
                found = true;
                break;
            }
        }
        if (!found)
            ctCombo->setCurrentIndex(0);
    }
  public slots:
    void PipelineChanged(const QString &newpipe)
    {
        cerr << "PipelineChanged\n";
        int index = pipelineCombo->currentIndex();
        if (index < 0)
            return;
        if (!plot)
            return;
        if (index >= Pipeline::allPipelines.size())
            return;

        plot->pipe = Pipeline::allPipelines[index];
        RebuildVarCombo();
    }
    void ColorTableChanged(const QString &ct)
    {
        if (!plot)
            return;
        ///\todo: we're on track to have a bunch of this junk
        /// any time we change stuff; fix it:
        delete plot->renderer;
        plot->renderer = NULL;

        plot->colortable = ct.toStdString();
        emit SomethingChanged();
    }
    void VarChanged(const QString &var)
    {
        if (!plot)
            return;

        // here, we set the field index and cell index given a field name
        delete plot->renderer;
        plot->renderer = NULL;
        int index = varCombo->currentIndex();
        if (index < 0 || index >= cellsets_for_var.size())
            plot->cellset = "";
        else
            plot->cellset = cellsets_for_var[index];
        if (index < 0 || index >= fields_for_var.size())
            plot->field = "";
        else
            plot->field = fields_for_var[index];

        emit SomethingChanged();
    }
  signals:
    void SomethingChanged();
};

// ****************************************************************************
// Class:  EL2DWindowSettings
//
// Purpose:
///   The settings widget to control options for the EL2DWindow.
//
// Programmer:  Jeremy Meredith
// Creation:    January 10, 2013
//
// Modifications:
// ****************************************************************************
class EL2DWindowSettings : public QWidget
{
    Q_OBJECT
  public: ///\todo: HACK, no public
    vector<Plot> plots;
  protected:

    QTreeWidget *plotList;
    QGroupBox *settingsGroup;
    EL2DPlotSettings *plotSettings;
  public:
    EL2DWindowSettings() : QWidget(NULL)
    {
        QGridLayout *topLayout = new QGridLayout(this);

        int trow=0;
        plotList = new QTreeWidget(this);
        plotList->setHeaderLabels(QStringList() << "Type" << "Vars" << "Settings");
        topLayout->addWidget(new QLabel("Plot List:", this), trow, 0, 1, 4);
        trow++;
        topLayout->addWidget(plotList, trow,0, 1, 4);
        trow++;
        connect(plotList, SIGNAL(itemSelectionChanged()),
                this, SLOT(plotSelected()));

        QPushButton *newPlotBtn = new QPushButton("New", this);
        connect(newPlotBtn, SIGNAL(clicked()),
                this, SLOT(NewPlot()));
        topLayout->addWidget(newPlotBtn, trow,0);
        QPushButton *delPlotBtn = new QPushButton("Del", this);
        connect(delPlotBtn, SIGNAL(clicked()),
                this, SLOT(DelPlot()));
        topLayout->addWidget(delPlotBtn, trow,1);
        QPushButton *upPlotBtn = new QPushButton("Up", this);
        connect(upPlotBtn, SIGNAL(clicked()),
                this, SLOT(UpPlot()));
        topLayout->addWidget(upPlotBtn, trow,2);
        QPushButton *downPlotBtn = new QPushButton("Down", this);
        connect(downPlotBtn, SIGNAL(clicked()),
                this, SLOT(DownPlot()));
        topLayout->addWidget(downPlotBtn, trow,3);
        trow++;

        //
        // Settings
        //
        settingsGroup = new QGroupBox("Settings", this);
        QGridLayout *settingsLayout = new QGridLayout(settingsGroup);
        topLayout->addWidget(settingsGroup, trow, 0, 1, 4);
        trow++;

        plotSettings = new EL2DPlotSettings(settingsGroup);
        connect(plotSettings, SIGNAL(SomethingChanged()),
                this, SIGNAL(SomethingChanged()));
        settingsLayout->addWidget(plotSettings);
        plotSettings->setEnabled(false);

        topLayout->setRowStretch(trow, 100);
    }
    void UpdatePlotList()
    {
        plotList->clear();
        for (int i=0; i<plots.size(); ++i)
        {
            Plot &p = plots[i];
            QTreeWidgetItem *item = new QTreeWidgetItem;
            item->setText(0, "plot");
            item->setText(1, p.field.c_str());
            item->setText(2, p.colortable.c_str());
            plotList->addTopLevelItem(item);
        }
    }
    void PipelineUpdated(Pipeline *pipe)
    {
        // hackish: add a plot with the current pipeline if
        // they hit execute and don't have any plots yet.
        if (plots.size() == 0)
        {
            Plot plot;
            plot.pipe = pipe;
            plots.push_back(plot);
        }

        for (int i=0; i<plots.size(); i++)
        {
            Plot &p = plots[i];
            if (p.pipe == pipe)
            {
                p.UpdateDataSet(pipe->results.back());
            }
        }

        UpdatePlotList();
        plotSettings->PipelineUpdated(pipe);
    }
  public slots:
    void NewPlot()
    {
        plots.push_back(Plot());
        UpdatePlotList();
        emit SomethingChanged();
    }
    void UpPlot()
    {
        int n = plots.size();
        int c = plotList->indexOfTopLevelItem(plotList->currentItem());
        if (c < 1 || c >= n)
            return;
        Plot p = plots[c-1];
        plots[c-1] = plots[c];
        plots[c] = p;
        UpdatePlotList();
        emit SomethingChanged();
    }
    void DownPlot()
    {
        int n = plots.size();
        int c = plotList->indexOfTopLevelItem(plotList->currentItem());
        if (c < 0 || c >= n-1)
            return;
        Plot p = plots[c+1];
        plots[c+1] = plots[c];
        plots[c] = p;
        UpdatePlotList();
        emit SomethingChanged();
    }
    void DelPlot()
    {
        int n = plots.size();
        int c = plotList->indexOfTopLevelItem(plotList->currentItem());
        if (c < 0 || c >= n)
            return;
        for (int i=c; i+1<n; i++)
            plots[i] = plots[i+1];
        plots.resize(n-1);
        UpdatePlotList();
        emit SomethingChanged();
    }
    void plotSelected()
    {
        QList<QTreeWidgetItem*> s = plotList->selectedItems();
        int n = s.size();
        if (n == 0)
        {
            //plotSettings->hide everything?
            plotSettings->setEnabled(false);
            return;
        }
        else if (n > 1)
        {
            plotSettings->setEnabled(false);
            cerr << "ERROR: more than one item selected\n";
            return;
        }
        QTreeWidgetItem *item = s[0];
        int rowindex = plotList->indexOfTopLevelItem(item);
        ///\todo: ensure 0<=index<nplots
        plotSettings->setEnabled(true);
        plotSettings->NewPlotSelected(&(plots[rowindex]));
    }
  signals:
    void SomethingChanged();
};

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
    EL2DWindowSettings *settings;
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
