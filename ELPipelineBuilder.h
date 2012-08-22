// Copyright 2012 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef EL_PIPELINE_BUILDER_H
#define EL_PIPELINE_BUILDER_H

#include <QWidget>
#include "eavlImporter.h"
#include "Pipeline.h"
class ELSources;
class QGroupBox;
class QTreeWidgetItem;
class QTreeWidget;

// ****************************************************************************
// Class:  ELPipelineBuilder
//
// Purpose:
///   Holds the results of operations on files, or on the other results
///   of other operations.
//
// Programmer:  Jeremy Meredith
// Creation:    August  1, 2012
//
// Modifications:
// ****************************************************************************
class ELPipelineBuilder : public QWidget
{
    Q_OBJECT
  public:
    int currentPipeline;
    std::map<QString, QWidget*> opSettingsWidgets;

  signals:
    void pipelineUpdated(int index, Pipeline *pipe);
    void CurrentPipelineChanged(int);

  public:
    ELPipelineBuilder(QWidget *parent);
    void addSource(const std::string &fn, eavlImporter *imp);
    void rebuildPipelineDisplay();

  public slots:
    void newOperation();
    void rowSelected();
    void executePipeline();
    void activatePipeline(int);
    void sourceUpdated();
    void operatorUpdated(Attribute*);

  protected:
    ELSources *sourceSettings;
    QTreeWidget *tree;
    QGroupBox *settingsGroup;
};

#endif
