// Copyright 2012-2013 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef EL_BASIC_INFO_WINDOW_H
#define EL_BASIC_INFO_WINDOW_H

#include "ELWindowManager.h"

#include <QTextEdit>

class Pipeline;

class ELPipelineChooser : public QWidget
{
    Q_OBJECT
  protected:
    QComboBox *pipelineCombo;
  public:
    ELPipelineChooser() : QWidget(NULL)
    {
        QGridLayout *topLayout = new QGridLayout(this);
        int srow = 0;
        pipelineCombo = new QComboBox(this);
        topLayout->addWidget(new QLabel("Pipeline Source:", this), srow,0, 1,2);
        srow++;
        topLayout->addWidget(pipelineCombo, srow,0, 1,2);
        srow++;
        connect(pipelineCombo, SIGNAL(activated(const QString&)),
                this, SLOT(PipelineSelected(const QString&)));

        topLayout->setRowStretch(srow, 100);

    }
    void PipelineUpdated(Pipeline */*pipe*/)
    {
        // rebuild the pipeline combo box
        while ((int)Pipeline::allPipelines.size() < pipelineCombo->count())
        {
            pipelineCombo->removeItem(pipelineCombo->count()-1);
        }
        while ((int)Pipeline::allPipelines.size() > pipelineCombo->count())
        {
            pipelineCombo->addItem("");
        }
        for (unsigned int i=0; i<Pipeline::allPipelines.size(); i++)
        {
            pipelineCombo->setItemText(i,
                                Pipeline::allPipelines[i]->GetName().c_str());
        }
    }
    Pipeline *GetPipeline()
    {
        return Pipeline::allPipelines[pipelineCombo->currentIndex()];
    }
  public slots:
    void PipelineSelected(const QString &)
    {
        emit SomethingChanged();
    }
  signals:
    void SomethingChanged();
};

// ****************************************************************************
// Class:  ELBasicInfoWindow
//
// Purpose:
///   Output window containing basic info about the source.
//
// Programmer:  Jeremy Meredith
// Creation:    August  3, 2012
//
// Modifications:
// ****************************************************************************
class ELBasicInfoWindow : public QWidget
{
    Q_OBJECT
  protected:
    QTextEdit     *info;
    ELPipelineChooser *settings;
  public:
    ELBasicInfoWindow(ELWindowManager *parent);
    void FillFromPipeline(Pipeline *p);
    QWidget *GetSettings();
  public slots:
    void CurrentPipelineChanged(int index);
    void PipelineUpdated(Pipeline *p);
    void SomethingChanged();
};



#endif
