// Copyright 2012-2013 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef EL_PLOT_LIST_H
#define EL_PLOT_LIST_H

#include "ELWindowManager.h"

#include <QTreeWidget>
#include <QPushButton>
#include <QGroupBox>
#include <QBrush>

#include "ELSurfacePlotSettings.h"


// ****************************************************************************
// Class:  ELPlotList
//
// Purpose:
///   The main settings widget for 1D/2D/3D windows.  Since this really
///   only contains the plot list for now, that's what we're calling it.
//
// Programmer:  Jeremy Meredith
// Creation:    January 10, 2013
//
// Modifications:
// ****************************************************************************
class ELPlotList : public QWidget
{
    Q_OBJECT
  public: ///\todo: HACK, no public
    bool oneDimensional;
    vector<Plot> plots;
  protected:
    Pipeline *latestUsedPipeline;

    int currentPlotIndex;
    QTreeWidget *plotList;
    QGroupBox *settingsGroup;
    ELSurfacePlotSettings *plotSettings;
    QPushButton *newPlotBtn;
    QPushButton *delPlotBtn;
    QPushButton *upPlotBtn;
    QPushButton *downPlotBtn;
  public:
    ELPlotList() : QWidget(NULL)
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
                this, SLOT(plotSelectionChanged()));

        newPlotBtn = new QPushButton("New", this);
        connect(newPlotBtn, SIGNAL(clicked()),
                this, SLOT(NewPlot()));
        topLayout->addWidget(newPlotBtn, trow,0);
        delPlotBtn = new QPushButton("Del", this);
        connect(delPlotBtn, SIGNAL(clicked()),
                this, SLOT(DelPlot()));
        topLayout->addWidget(delPlotBtn, trow,1);
        upPlotBtn = new QPushButton("Up", this);
        connect(upPlotBtn, SIGNAL(clicked()),
                this, SLOT(UpPlot()));
        topLayout->addWidget(upPlotBtn, trow,2);
        downPlotBtn = new QPushButton("Down", this);
        connect(downPlotBtn, SIGNAL(clicked()),
                this, SLOT(DownPlot()));
        topLayout->addWidget(downPlotBtn, trow,3);
        trow++;

        newPlotBtn->setEnabled(false);
        upPlotBtn->setEnabled(false);
        downPlotBtn->setEnabled(false);
        delPlotBtn->setEnabled(false);

        //
        // Settings
        //
        settingsGroup = new QGroupBox("Settings", this);
        QGridLayout *settingsLayout = new QGridLayout(settingsGroup);
        topLayout->addWidget(settingsGroup, trow, 0, 1, 4);
        topLayout->setRowStretch(trow, 100);
        trow++;

        plotSettings = new ELSurfacePlotSettings(settingsGroup);
        connect(plotSettings, SIGNAL(SomethingChanged()),
                this, SLOT(PlotChanged()));
        settingsLayout->addWidget(plotSettings);
        plotSettings->setEnabled(false);

        // other initialization
        oneDimensional = false;
        currentPlotIndex = -1;
        latestUsedPipeline = NULL;
    }
    void SetItemTextFromPlot(QTreeWidgetItem *item, Plot &p)
    {
        item->setText(0, "plot");
        string cs = p.cellset;
        if (cs.empty())
            cs = "Points";
        if (p.field.empty())
            item->setText(1, cs.c_str());
        else
            item->setText(1, (p.field + " ("+cs+")").c_str());
        item->setText(2, p.colortable.c_str());
    }
    void UpdatePlotList()
    {
        //plotList->clear();
        for (int i=0; i<plots.size(); ++i)
        {
            Plot &p = plots[i];
            QTreeWidgetItem *item = plotList->topLevelItem(i);
            if (!item)
            {
                item = new QTreeWidgetItem;
                plotList->addTopLevelItem(item);
            }
            SetItemTextFromPlot(item, p);
            /*
            QBrush brush;
            if (p.valid)
            {
                item->setForeground(0,brush);
                item->setForeground(1,brush);
                item->setForeground(2,brush);
            }
            else
            {
                brush.setColor(Qt::red);
                item->setForeground(0,brush);
                item->setForeground(1,brush);
                item->setForeground(2,brush);
            }
            */
        }
        for (int i=plots.size(); i<plotList->topLevelItemCount(); ++i)
            delete plotList->takeTopLevelItem(i);
    }
    void PipelineUpdated(Pipeline *pipe)
    {
        latestUsedPipeline = pipe;

        newPlotBtn->setEnabled(true);
        // hackish: add a plot with the current pipeline if
        // they hit execute and don't have any plots yet.
        if (plots.size() == 0)
        {
            Plot plot;
            plot.oneDimensional = oneDimensional; ///<\todo:hac!
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
    void PlotChanged()
    {
        if (currentPlotIndex >= 0)
        {
            Plot &p = plots[currentPlotIndex];
            QTreeWidgetItem *item = plotList->topLevelItem(currentPlotIndex);
            SetItemTextFromPlot(item, p);
        }

        emit SomethingChanged();
    }
    void NewPlot()
    {
        Plot p;
        p.oneDimensional = oneDimensional; ///<\todo:hack!
        p.pipe = latestUsedPipeline;
        plots.push_back(p);
        UpdatePlotList();
        // select the new plot (and not any others)
        plotList->setCurrentItem(plotList->topLevelItem(plotList->topLevelItemCount()-1));
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
    void plotSelectionChanged()
    {
        currentPlotIndex = -1;
        upPlotBtn->setEnabled(false);
        downPlotBtn->setEnabled(false);
        delPlotBtn->setEnabled(false);

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
        currentPlotIndex = plotList->indexOfTopLevelItem(item);
        delPlotBtn->setEnabled(true);
        if (currentPlotIndex > 0)
            upPlotBtn->setEnabled(true);
        if (currentPlotIndex < plots.size() - 1)
            downPlotBtn->setEnabled(true);


        ///\todo: ensure 0<=index<nplots
        plotSettings->setEnabled(true);
        plotSettings->NewPlotSelected(&(plots[currentPlotIndex]));
    }
  signals:
    void SomethingChanged();
};



#endif
