// Copyright 2012-2013 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef EL_SURFACE_PLOT_SETTINGS_H
#define EL_SURFACE_PLOT_SETTINGS_H

#include "ELWindowManager.h"

#include <QGLWidget>
#include <QTreeWidget>
#include <QGroupBox>
#include <QHeaderView>
#include <QColorDialog>
#include <QCheckBox>
#include <Plot.h>

#include <eavlView.h>

// ****************************************************************************
// Class:  ELSurfacePlotSettings
//
// Purpose:
///   Settings needed for a surface-style plot (color tables, wireframe
///   flag, etc.).
//
// Programmer:  Jeremy Meredith
// Creation:    March 12, 2013
//
// Modifications:
// ****************************************************************************
class ELSurfacePlotSettings : public QWidget
{
    Q_OBJECT
  public: ///\todo: HACK, no public
    bool oneDimensional;
  protected:
    QComboBox *pipelineCombo;
    QTreeWidget *varChooser;
    QComboBox *ctCombo;
    QPushButton *colorBtn;
    QCheckBox *wireframeChk;
    QCheckBox *reverseCTChk;
    QCheckBox *logCTChk;
    QComboBox *styleCombo;
    Plot *plot;
    vector<string> cellsetList;
    vector< vector<string> > fieldList;
  public:
    ELSurfacePlotSettings(QWidget *p) : QWidget(p)
    {
        plot = NULL;

        QGridLayout *topLayout = new QGridLayout(this);

        int srow = 0;
        pipelineCombo = new QComboBox(this);
        topLayout->addWidget(new QLabel("Pipeline Source:", this), srow,0, 1,2);
        srow++;
        topLayout->addWidget(pipelineCombo, srow,0, 1,2);
        srow++;
        connect(pipelineCombo, SIGNAL(activated(const QString&)),
                this, SLOT(PipelineChanged(const QString&)));

        varChooser = new QTreeWidget(this);
        varChooser->setHeaderLabels(QStringList()<<"Variable"<<"Info");
        //varChooser->header()->hide();
        connect(varChooser, SIGNAL(itemSelectionChanged()),
                this, SLOT(VarSelectionChanged()));
        topLayout->addWidget(new QLabel("Field:", this), srow,0, 1,2);
        srow++;
        topLayout->addWidget(varChooser, srow,0, 1,2);
        topLayout->setRowStretch(srow, 100);
        srow++;

        ctCombo = new QComboBox(this);
        ctCombo->addItem("dense");
        ctCombo->addItem("sharp");
        ctCombo->addItem("thermal");
        ctCombo->addItem("blue");
        ctCombo->addItem("orange");
        ctCombo->addItem("levels");
        // perceptually linear
        ctCombo->addItem("IsoL");
        ctCombo->addItem("LinearL");
        ctCombo->addItem("LinLhot");
        ctCombo->addItem("CubicL");
        ctCombo->addItem("CubicYF");
        // colorbrewer
        ctCombo->addItem("Accent");
        ctCombo->addItem("Blues");
        ctCombo->addItem("BrBG");
        ctCombo->addItem("BuGn");
        ctCombo->addItem("BuPu");
        ctCombo->addItem("Dark2");
        ctCombo->addItem("GnBu");
        ctCombo->addItem("Greens");
        ctCombo->addItem("Greys");
        ctCombo->addItem("Oranges");
        ctCombo->addItem("OrRd");
        ctCombo->addItem("Paired");
        ctCombo->addItem("Pastel1");
        ctCombo->addItem("Pastel2");
        ctCombo->addItem("PiYG");
        ctCombo->addItem("PRGn");
        ctCombo->addItem("PuBu");
        ctCombo->addItem("PuBuGn");
        ctCombo->addItem("PuOr");
        ctCombo->addItem("PuRd");
        ctCombo->addItem("Purples");
        ctCombo->addItem("RdBu");
        ctCombo->addItem("RdGy");
        ctCombo->addItem("RdPu");
        ctCombo->addItem("RdYlBu");
        ctCombo->addItem("RdYlGn");
        ctCombo->addItem("Reds");
        ctCombo->addItem("Set1");
        ctCombo->addItem("Set2");
        ctCombo->addItem("Set3");
        ctCombo->addItem("Spectral");
        ctCombo->addItem("YlGnBu");
        ctCombo->addItem("YlGn");
        ctCombo->addItem("YlOrBr");
        ctCombo->addItem("YlOrRd");
        topLayout->addWidget(new QLabel("Color Table:", this), srow,0);
        topLayout->addWidget(ctCombo, srow,1);
        srow++;
        connect(ctCombo, SIGNAL(activated(const QString&)),
                this, SLOT(ColorTableChanged(const QString&)));

        reverseCTChk = new QCheckBox("Reverse Color Table", this);
        topLayout->addWidget(reverseCTChk, srow,0, 1,2);
        srow++;
        connect(reverseCTChk, SIGNAL(stateChanged(int)),
                this, SLOT(ReverseCTCheckChanged(int)));

        logCTChk = new QCheckBox("Log Color Table", this);
        topLayout->addWidget(logCTChk, srow,0, 1,2);
        srow++;
        connect(logCTChk, SIGNAL(stateChanged(int)),
                this, SLOT(LogCTCheckChanged(int)));

        colorBtn = new QPushButton(this);
        topLayout->addWidget(new QLabel("Solid Color:", this), srow,0);
        topLayout->addWidget(colorBtn, srow,1);
        srow++;
        connect(colorBtn, SIGNAL(clicked()),
                this, SLOT(ColorButtonClicked()));

        wireframeChk = new QCheckBox("Wireframe", this);
        topLayout->addWidget(wireframeChk, srow,0, 1,2);
        srow++;
        connect(wireframeChk, SIGNAL(stateChanged(int)),
                this, SLOT(WireframeCheckChanged(int)));

        styleCombo = new QComboBox(this);
        styleCombo->addItem("Curves");
        styleCombo->addItem("Bars");
        topLayout->addWidget(new QLabel("1D Style:", this), srow,0);
        topLayout->addWidget(styleCombo, srow,1);
        connect(styleCombo, SIGNAL(activated(const QString&)),
                this, SLOT(StyleChanged(const QString&)));
    }
    void PipelineUpdated(Pipeline *)
    {
        // rebuild the pipeline combo box
        int index = -1;
        pipelineCombo->clear();
        for (unsigned int i=0; i<Pipeline::allPipelines.size(); i++)
        {
            pipelineCombo->addItem(Pipeline::allPipelines[i]->GetName().c_str());
            if (plot && plot->pipe == Pipeline::allPipelines[i])
                index = i;
        }
        if (index >= 0)
            pipelineCombo->setCurrentIndex(index);

        RebuildVarChooser();

        emit SomethingChanged();
    }
    void RebuildVarChooser()
    {
        // rebuild the field combo box
        varChooser->clear();

        if (!plot)
            return;
        Pipeline *p = plot->pipe;
        if (!p)
            return;

        cellsetList.clear();
        fieldList.clear();

        varChooser->blockSignals(true);

        QTreeWidgetItem *selItem = NULL;

        DSInfo dsinfo = p->GetVariables(-1);
        QTreeWidgetItem *ptsItem = new QTreeWidgetItem(QStringList()<<"Points");
        varChooser->addTopLevelItem(ptsItem);
        cellsetList.push_back("");
        fieldList.push_back(vector<string>());
        if (plot->cellset == "" && plot->field == "")
            selItem = ptsItem;
        for (unsigned int i=0; i<dsinfo.nodalfields.size(); ++i)
        {
            QTreeWidgetItem *fItem = new QTreeWidgetItem(QStringList()
                                                         <<dsinfo.nodalfields[i].name.c_str()
                                                         <<(dsinfo.nodalfields[i].ncomp==1 ? "scalar" : "vector"));
            ptsItem->addChild(fItem);
            fieldList[fieldList.size()-1].push_back(dsinfo.nodalfields[i].name);
            if (plot->cellset == "" && plot->field == dsinfo.nodalfields[i].name)
                selItem = fItem;
        }
        ptsItem->setExpanded(true);


        for (unsigned int k=0; k<dsinfo.cellsets.size(); ++k)
        {
            string csname = dsinfo.cellsets[k].name;
            QTreeWidgetItem *csItem = new QTreeWidgetItem(QStringList()
                                                          <<csname.c_str()
                                                          <<QString().sprintf("%dD",dsinfo.cellsets[k].topodim));
            varChooser->addTopLevelItem(csItem);
            cellsetList.push_back(csname);
            fieldList.push_back(vector<string>());
            if (plot->cellset == csname && plot->field == "")
                selItem = csItem;
            for (unsigned int i=0; i<dsinfo.nodalfields.size(); ++i)
            {
                QTreeWidgetItem *fItem = new QTreeWidgetItem(QStringList()
                                                             <<dsinfo.nodalfields[i].name.c_str()
                                                             <<(dsinfo.nodalfields[i].ncomp==1 ? "scalar" : "vector"));
                csItem->addChild(fItem);
                fieldList[fieldList.size()-1].push_back(dsinfo.nodalfields[i].name);
                if (plot->cellset == csname && plot->field == dsinfo.nodalfields[i].name)
                    selItem = fItem;
            }
            for (unsigned int i=0; i<dsinfo.cellsetfields[csname].size(); ++i)
            {
                QTreeWidgetItem *fItem = new QTreeWidgetItem(QStringList()
                                                             <<dsinfo.cellsetfields[csname][i].name.c_str()
                                                             <<(dsinfo.cellsetfields[csname][i].ncomp==1 ? "scalar" : "vector"));
                csItem->addChild(fItem);
                fieldList[fieldList.size()-1].push_back(dsinfo.cellsetfields[csname][i].name);
                if (plot->cellset == csname && plot->field == dsinfo.cellsetfields[csname][i].name)
                    selItem = fItem;
            }
            csItem->setExpanded(true);
        }

        if (selItem)
            selItem->setSelected(true);
        else
        {
            cerr << "PLOT IS IN ERROR!\n";
            // ERROR
        }

        varChooser->blockSignals(false);

    }
    void NewPlotSelected(Plot *p)
    {
        plot = p;
        RebuildVarChooser();
        wireframeChk->setChecked(plot->wireframe);
        SetColorTableCombo(plot->colortable);
        reverseCTChk->setChecked(plot->reversect);
        logCTChk->setChecked(plot->logct);
        SetColorButtonColor(plot->color);
        for (unsigned int i=0; i<Pipeline::allPipelines.size(); i++)
        {
            if (plot->pipe == Pipeline::allPipelines[i])
            {
                pipelineCombo->setCurrentIndex(i);
                break;
            }
        }
    }
  public slots:
    void PipelineChanged(const QString &)
    {
        int index = pipelineCombo->currentIndex();
        if (index < 0)
            return;
        if (!plot)
            return;
        if (index >= (int)Pipeline::allPipelines.size())
            return;

        plot->pipe = Pipeline::allPipelines[index];
        RebuildVarChooser();
        emit SomethingChanged();
    }
    void SetColorTableCombo(string ct)
    {
        ctCombo->blockSignals(true);

        bool found = false;
        for (int i=0; i<ctCombo->count(); ++i)
        {
            if (ctCombo->itemText(i) == ct.c_str())
            {
                ctCombo->setCurrentIndex(i);
                found = true;
                break;
            }
        }
        if (!found)
            ctCombo->setCurrentIndex(0);

        ctCombo->blockSignals(false);
    }
    void SetColorButtonColor(eavlColor c)
    {
        QColor color(c.GetComponentAsByte(0),
                     c.GetComponentAsByte(1),
                     c.GetComponentAsByte(2));
        QImage img(16,16, QImage::Format_RGB32);
        img.fill(color);
        QPainter p(&img);
        if (c.c[0]+c.c[1]+c.c[2] < 1.0)
            p.setPen(Qt::white);
        else
            p.setPen(Qt::black);
        p.drawRect(0,0,15,15);
        colorBtn->setIcon(QPixmap::fromImage(img));
    }
    void StyleChanged(const QString &style)
    {
        if (!plot)
            return;

        plot->barsFor1D = (style == "Bars");

        emit SomethingChanged();
    }
    void WireframeCheckChanged(int state)
    {
        if (!plot)
            return;

        plot->wireframe = state;

        emit SomethingChanged();
    }
    void ColorButtonClicked()
    {
        if (!plot)
            return;

        eavlColor c = plot->color;
        QColor color(c.GetComponentAsByte(0),
                     c.GetComponentAsByte(1),
                     c.GetComponentAsByte(2));
        color = QColorDialog::getColor(color, this);

        plot->color = eavlColor(color.redF(),
                                color.greenF(),
                                color.blueF());

        SetColorButtonColor(plot->color);

        emit SomethingChanged();
    }
    void ColorTableChanged(const QString &ct)
    {
        if (!plot)
            return;

        plot->colortable = ct.toStdString();
        emit SomethingChanged();
    }
    void ReverseCTCheckChanged(int state)
    {
        if (!plot)
            return;

        plot->reversect = state;
        emit SomethingChanged();
    }
    void LogCTCheckChanged(int state)
    {
        if (!plot)
            return;

        plot->logct = state;
        emit SomethingChanged();
    }
    void VarSelectionChanged()
    {
        if (!plot)
            return;

        // here, we set the field index and cell index given a field name

        // need to force an update when the sel
        plot->UpdateDataSet(plot->pipe->results.back());

        string oldCS = plot->cellset;
        string oldF = plot->field;

        QList<QTreeWidgetItem*> s = varChooser->selectedItems();
        if (s.size() == 1)
        {
            int fieldIndex = -1;
            int cellSetIndex = varChooser->indexOfTopLevelItem(s[0]);
            if (cellSetIndex<0)
            {
                fieldIndex = s[0]->parent()->indexOfChild(s[0]);
                cellSetIndex = varChooser->indexOfTopLevelItem(s[0]->parent());
            }
            if (cellSetIndex < 0)
            {
                cerr << "Unknown error\n";
                return;
            }
            //cerr << "cellSetIndex="<<cellSetIndex<<endl;
            //cerr << "fieldIndex="<<fieldIndex<<endl;

            plot->cellset = cellsetList[cellSetIndex];
            plot->field = (fieldIndex<0) ? "" : fieldList[cellSetIndex][fieldIndex];
        }

        if (plot->cellset != oldCS ||
            plot->field != oldF)
            emit SomethingChanged();
    }
  signals:
    void SomethingChanged();
};

#endif
