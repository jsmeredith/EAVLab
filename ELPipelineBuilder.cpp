// Copyright 2012 UT-Battelle, LLC.  See LICENSE.txt for more information.
#include "ELPipelineBuilder.h"

#include <QFileInfo>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QGridLayout>
#include <QMenu>
#include <QPushButton>
#include <QSplitter>
#include <QGroupBox>
#include <QComboBox>
#include <QLabel>
#include <QMessageBox>

#include "Operation.h"
#include "ELAttributeControl.h"
#include "ELSources.h"

#include "ExternalFaceOperation.h"
#include "IsosurfaceOperation.h"
#include "SurfaceNormalsOperation.h"

// ****************************************************************************
// Constructor:  ELPipelineBuilder::ELPipelineBuilder
//
// Programmer:  Jeremy Meredith
// Creation:    August  2, 2012
//
// Modifications:
// ****************************************************************************
ELPipelineBuilder::ELPipelineBuilder(QWidget *parent)
    : QWidget(parent)
{
    currentPipeline = -1;

    // Top layout
    QGridLayout *topLayout = new QGridLayout(this);
    QComboBox *pipelineChooser = new QComboBox(this);
    pipelineChooser->addItem("first pipeline");
    pipelineChooser->addItem("second pipeline");
    pipelineChooser->addItem("some other pipeline");
    pipelineChooser->addItem("yes, I want some names here");
    connect(pipelineChooser, SIGNAL(activated(int)), 
            this, SLOT(activatePipeline(int)));
    topLayout->addWidget(new QLabel("Pipeline: ", this), 0,0, 1,1);
    topLayout->addWidget(pipelineChooser, 0,1, 1,1);

    QSplitter *topSplitter = new QSplitter(Qt::Vertical, this);
    topLayout->addWidget(topSplitter, 1, 0, 1, 2);

    QGroupBox *pipelineGroup = new QGroupBox("Pipeline",
                                             topSplitter);

    QGridLayout *pipelineLayout = new QGridLayout(pipelineGroup);

    //
    // The pipeline tree
    //
    tree = new QTreeWidget(pipelineGroup);
    tree->setHeaderLabels(QStringList() << "Operation" << "Settings");
    //tree->setHeaderHidden(true);
    pipelineLayout->addWidget(tree, 0,0);
    connect(tree, SIGNAL(itemSelectionChanged()),
            this, SLOT(rowSelected()));

    //
    // The operator menu
    //
    QMenu *opMenu = new QMenu();
    ///\todo: these choice names must currently match the exact text in
    /// Operation::GetOperationName.  We should loosed this restriction.
    const char *operations[] = {
        "Isosurface",
        "ExternalFace",
        "SurfaceNormals",
        NULL
    };
    for (int i=0; operations[i] != NULL; i++)
    {
        QAction *op= opMenu->addAction(operations[i]);
        op->setData(QString(operations[i]));
        connect(op, SIGNAL(triggered()), this, SLOT(newOperation()));
    }
    QPushButton *addOpButton = new QPushButton("Add Operation", pipelineGroup);
    addOpButton->setMenu(opMenu);
    pipelineLayout->addWidget(addOpButton, 1,0);

    //
    // add execute button (probably not the best place for it)
    //
    QPushButton *deleteOpButton = new QPushButton("Delete Operation", pipelineGroup);
    pipelineLayout->addWidget(deleteOpButton, 2, 0);
    connect(deleteOpButton, SIGNAL(clicked()),
            this, SLOT(deleteCurrentOp()));


    //
    // add execute button (probably not the best place for it)
    //
    QPushButton *executeButton = new QPushButton("Execute", pipelineGroup);
    pipelineLayout->addWidget(executeButton, 3, 0);
    connect(executeButton, SIGNAL(clicked()),
            this, SLOT(executePipeline()));

    //
    // Settings
    //
    settingsGroup = new QGroupBox("Settings", topSplitter);
    QGridLayout *settingsLayout = new QGridLayout(settingsGroup);

    //
    // sources widgets
    //
    sourceSettings = new ELSources(settingsGroup);
    connect(sourceSettings, SIGNAL(sourceChanged()),
            this, SLOT(sourceUpdated()));
    settingsLayout->addWidget(sourceSettings);

    // ---
    ///\todo: for now, let's create a fixed number of pipelines
    Pipeline::allPipelines.push_back(new Pipeline);
    Pipeline::allPipelines.push_back(new Pipeline);
    Pipeline::allPipelines.push_back(new Pipeline);
    Pipeline::allPipelines.push_back(new Pipeline);
    activatePipeline(0);

    topSplitter->setStretchFactor(0,30);
    topSplitter->setStretchFactor(1,50);
}


// ****************************************************************************
// Method:  ELPipelineBuilder::rebuildPipelineDisplay
//
// Purpose:
///   Recreate the widget showing the pipeline.
///   This also sets the highlighted item to the source (the first
///   item in the list).
//
// Arguments:
//   
//
// Programmer:  Jeremy Meredith
// Creation:    August  7, 2012
//
// Modifications:
// ****************************************************************************
void
ELPipelineBuilder::rebuildPipelineDisplay()
{
    tree->clear();

    if (currentPipeline < 0 || currentPipeline >= (int)Pipeline::allPipelines.size())
        return;
    Pipeline *pipeline = Pipeline::allPipelines[currentPipeline];

    QTreeWidgetItem *sourceItem = new QTreeWidgetItem;
    sourceItem->setText(0, pipeline->source->GetSourceType().c_str());
    sourceItem->setText(1, pipeline->source->GetSourceInfo().c_str());
    tree->addTopLevelItem(sourceItem);

    for (unsigned int i=0; i<pipeline->ops.size(); ++i)
    {
        QTreeWidgetItem *opitem = new QTreeWidgetItem;
        opitem->setText(0, pipeline->ops[i]->GetOperationName().c_str());
        opitem->setText(1, pipeline->ops[i]->GetOperationInfo().c_str());
        tree->addTopLevelItem(opitem);
    }

    tree->setCurrentItem(sourceItem);
}

// ****************************************************************************
// Method:  ELPipelineBuilder::activatePipeline
//
// Purpose:
///   Slot to change the pipeline being edited.
//
// Arguments:
//   index      the index of the pipeline
//
// Programmer:  Jeremy Meredith
// Creation:    August  7, 2012
//
// Modifications:
// ****************************************************************************
void
ELPipelineBuilder::activatePipeline(int index)
{
    currentPipeline = index;
    rebuildPipelineDisplay();
    emit CurrentPipelineChanged(index);
}


// ****************************************************************************
// Method:  ELPipelineBuilder::newOperation
//
// Purpose:
///   Insert an operation at the end of the pipeline.
//
// Programmer:  Jeremy Meredith
// Creation:    August  7, 2012
//
// Modifications:
// ****************************************************************************
void
ELPipelineBuilder::newOperation()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (!action || !action->data().isValid())
        return;

    if (currentPipeline < 0 || currentPipeline >= (int)Pipeline::allPipelines.size())
        return;
    Pipeline *pipeline = Pipeline::allPipelines[currentPipeline];

    QString actionname = action->data().toString();

    QWidget *opSettingsWidget = opSettingsWidgets[actionname];
    if (opSettingsWidget == NULL)
    {
        opSettingsWidget = new ELAttributeControl(settingsGroup);
        opSettingsWidgets[actionname] = opSettingsWidget;
        connect(opSettingsWidget, SIGNAL(settingsChanged(Attribute*)),
                this, SLOT(operatorUpdated(Attribute*)));
    }

    if (actionname == "Isosurface")
        pipeline->ops.push_back(new IsosurfaceOperation);
    else if (actionname == "ExternalFace")
        pipeline->ops.push_back(new ExternalFaceOperation);
    else if (actionname == "SurfaceNormals")
        pipeline->ops.push_back(new SurfaceNormalsOperation);
    else
        throw "Unexpected operation";

    opSettingsWidget->hide();

    rebuildPipelineDisplay();
    tree->setCurrentItem(tree->topLevelItem(tree->topLevelItemCount()-1));
}


// ****************************************************************************
// Method:  ELPipelineBuilder::addSource
//
// Purpose:
///   When the user opens a file, add its meshes to the source list.
//
// Programmer:  Jeremy Meredith
// Creation:    August  7, 2012
//
// Modifications:
// ****************************************************************************
void
ELPipelineBuilder::addSource(const std::string &fn, eavlImporter *imp)
{
    sourceSettings->addSource(fn,imp);
}


// ****************************************************************************
// Method:  ELPipelineBuilder::rowSelected
//
// Purpose:
///   Qt slot called when pipeline row selection changes.
//
// Programmer:  Jeremy Meredith
// Creation:    August  7, 2012
//
// Modifications:
// ****************************************************************************
void
ELPipelineBuilder::rowSelected()
{
    if (currentPipeline < 0 || currentPipeline >= (int)Pipeline::allPipelines.size())
        return;

    Pipeline *pipeline = Pipeline::allPipelines[currentPipeline];

    QList<QTreeWidgetItem*> s = tree->selectedItems();
    int n = s.size();
    if (n == 0)
    {
        return;
    }
    else if (n > 1)
    {
        cerr << "ERROR: more than one item selected\n";
    }
    else
    {
        QTreeWidgetItem *item = s[0];
        QLayoutItem *oldSettingsLI = settingsGroup->layout()->takeAt(0);
        if (oldSettingsLI)
        {
            QWidget *oldSettings = oldSettingsLI->widget();
            if (oldSettings)
                oldSettings->hide();
        }
        int rowindex = tree->indexOfTopLevelItem(item);
        if (rowindex == 0)
        {
            settingsGroup->layout()->addWidget(sourceSettings);
            sourceSettings->ConnectSettings(pipeline->source);
            sourceSettings->UpdateWindowFromSettings();
            sourceSettings->show();
        }
        else
        {
            int opindex = rowindex-1;
            QString name = item->text(0);
            QWidget *newSettingsWidget = opSettingsWidgets[name];
            if (!newSettingsWidget)
                cerr << "FATAL Error: didn't get settings with name="<<name.toStdString()<<endl;
            ELAttributeControl *controls = dynamic_cast<ELAttributeControl*>(newSettingsWidget);
            if (!controls)
                throw "eh?";

            Operation *op = pipeline->ops[opindex];

            if (!op)
                return;

            controls->ConnectAttributes(op->GetSettings());
            controls->UpdateWindowFromAtts();
            settingsGroup->layout()->addWidget(newSettingsWidget);
            newSettingsWidget->show();
        }
    }
    
}

// ****************************************************************************
// Method:  ELPipelineBuilder::executePipeline
//
// Purpose:
///   Execute the active pipeline and update any watchers via a signal.
//
// Arguments:
//   none
//
// Programmer:  Jeremy Meredith
// Creation:    August  7, 2012
//
// Modifications:
// ****************************************************************************
void
ELPipelineBuilder::executePipeline()
{
    if (currentPipeline < 0 || currentPipeline >= (int)Pipeline::allPipelines.size())
        return;
    Pipeline *pipeline = Pipeline::allPipelines[currentPipeline];

    try {
        pipeline->Execute();
    }
    catch (eavlException &e)
    {
        QMessageBox::critical(this,
                              "Error executing pipeline",
                              e.GetErrorText().c_str());
        return;
    }

    emit pipelineUpdated(currentPipeline, pipeline);
}

// ****************************************************************************
// Method:  ELPipelineBuilder::sourceUpdated
//
// Purpose:
///   Slot for when the source changes.
//
// Arguments:
//   none
//
// Programmer:  Jeremy Meredith
// Creation:    August 21, 2012
//
// Modifications:
// ****************************************************************************
void
ELPipelineBuilder::sourceUpdated()
{
    if (currentPipeline < 0 || currentPipeline >= (int)Pipeline::allPipelines.size())
        return;
    Pipeline *pipeline = Pipeline::allPipelines[currentPipeline];
    if (!pipeline || !pipeline->source)
        return;

    QTreeWidgetItem *sourceItem = tree->topLevelItem(0);
    sourceItem->setText(0, pipeline->source->GetSourceType().c_str());
    sourceItem->setText(1, pipeline->source->GetSourceInfo().c_str());
}

// ****************************************************************************
// Method:  ELPipelineBuilder::operatorUpdated
//
// Purpose:
///   Slot for when an operator's settings have been changed.
///   Ideally, we could (optionally) execute the pipeline every time.
//
// Arguments:
//   settings   the settings that were updated
//
// Programmer:  Jeremy Meredith
// Creation:    August 21, 2012
//
// Modifications:
// ****************************************************************************
void
ELPipelineBuilder::operatorUpdated(Attribute *settings)
{
    if (currentPipeline < 0 || currentPipeline >= (int)Pipeline::allPipelines.size())
        return;
    Pipeline *pipeline = Pipeline::allPipelines[currentPipeline];

    // find the operator with these settings; a bit of a hack
    // if we changed this to get info about which operator
    // these new settings came from, it would be cleaner
    int opindex = -1;
    for (unsigned int i=0; i<pipeline->ops.size(); ++i)
    {
        if (pipeline->ops[i]->GetSettings() == settings)
        {
            opindex = i;
            break;
        }
    }

    Operation *op = pipeline->ops[opindex];

    int rowindex = opindex + 1;
    QTreeWidgetItem *item = tree->topLevelItem(rowindex);
    item->setText(0, op->GetOperationName().c_str());
    item->setText(1, op->GetOperationInfo().c_str());
}


void
ELPipelineBuilder::deleteCurrentOp()
{
    if (currentPipeline < 0 || currentPipeline >= (int)Pipeline::allPipelines.size())
        return;

    Pipeline *pipeline = Pipeline::allPipelines[currentPipeline];

    QList<QTreeWidgetItem*> s = tree->selectedItems();
    int n = s.size();
    if (n == 0)
    {
        return;
    }
    else if (n > 1)
    {
        cerr << "ERROR: more than one item selected\n";
    }
    else
    {
        QTreeWidgetItem *item = s[0];
        int rowindex = tree->indexOfTopLevelItem(item);
        if (rowindex == 0)
            return;
        int opindex = rowindex - 1;
        for (int i = opindex; i < pipeline->ops.size()-1; ++i)
            pipeline->ops[i] = pipeline->ops[i+1];
        pipeline->ops.resize(pipeline->ops.size()-1);
        rebuildPipelineDisplay();
    }
}
