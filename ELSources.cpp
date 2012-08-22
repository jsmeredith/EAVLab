// Copyright 2012 UT-Battelle, LLC.  See LICENSE.txt for more information.
#include "ELSources.h"

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QFileInfo>
#include <QGridLayout>
#include <QComboBox>

#include "Pipeline.h"

// ****************************************************************************
// Constructor:  ELSources::ELSources
//
// Programmer:  Jeremy Meredith
// Creation:    August  2, 2012
//
// Modifications:
// ****************************************************************************
ELSources::ELSources(QWidget *parent)
    : QTabWidget(parent)
{
    // create tabs in the order of Source::SourceType
    source = NULL;
    connect(this, SIGNAL(currentChanged(int)),
            this, SLOT(tabChanged(int)));

    //
    // File source
    //
    QWidget *fileTab = new QWidget();
    QGridLayout *fileLayout = new QGridLayout(fileTab);

    combo = new QComboBox(fileTab);
    combo->addItem("(none)");
    fileLayout->addWidget(combo);

    connect(combo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(fileMeshChanged(int)));


    addTab(fileTab, "File");

    //
    // Pipeline source
    //
    QWidget *pipeTab = new QWidget();
    addTab(pipeTab, "Pipeline");

    //
    // Geometry source
    //
    QWidget *geomTab = new QWidget();
    addTab(geomTab, "Geometry");
}

// ****************************************************************************
// Method:  ELSources::addSource
//
// Purpose:
///   Adds a source, its meshes, and its variables to the tree.
///   Also keep track of the importer for the source.
//
// Programmer:  Jeremy Meredith
// Creation:    August  2, 2012
//
// Modifications:
// ****************************************************************************
void
ELSources::addSource(const std::string &fn, eavlImporter *imp)
{
    openFiles[fn] = imp;
    QString shortname = QFileInfo(fn.c_str()).fileName();

    bool firstSource = false;
    if (combo->count() == 1 && combo->itemText(0) == "(none)")
        firstSource = true;

    vector<string> meshes = imp->GetMeshList();
    for (unsigned int i=0; i<meshes.size(); i++)
    {
        combo->addItem(shortname + ":" + meshes[i].c_str(),
                       QStringList() <<
                       QString(fn.c_str()) <<
                       QString(meshes[i].c_str()));
    }

    if (firstSource)
        combo->setCurrentIndex(1);
}


// ****************************************************************************
// Method:  ELSources::fileMeshChanged
//
// Purpose:
///   Slot for when the file:mesh combo box active item changes.
//
// Arguments:
//   index      the new index in the combo
//
// Programmer:  Jeremy Meredith
// Creation:    August  2, 2012
//
// Modifications:
// ****************************************************************************
void
ELSources::fileMeshChanged(int index)
{
    if (!source)
        return;
    if (index < 0)
        return;

    QStringList data = combo->itemData(index).toStringList();
    if (data.size() < 2)
        return;

    source->file = data[0].toStdString();
    source->mesh = data[1].toStdString();
    source->source_file = openFiles[source->file];
    emit sourceChanged();
}

// ****************************************************************************
// Method:  ELSources::tabChanged
//
// Purpose:
///   Slot for when the tab (i.e. source type) changed.
//
// Arguments:
//   index      the index of the tab; we assume this correlates with SourceType
//
// Programmer:  Jeremy Meredith
// Creation:    August 21, 2012
//
// Modifications:
// ****************************************************************************
void
ELSources::tabChanged(int index)
{
    if (!source)
        return;

    source->sourcetype = Source::SourceType(index);
    emit sourceChanged();
}

// ****************************************************************************
// Method:  ELSources::ConnectSettings
//
// Purpose:
///   Set the source object we're now supposed to watch/update from.
//
// Arguments:
//   s          the new source object
//
// Programmer:  Jeremy Meredith
// Creation:    August 21, 2012
//
// Modifications:
// ****************************************************************************
void
ELSources::ConnectSettings(Source *s)
{
    source = s;
}

// ****************************************************************************
// Method:  ELSources::UpdateWindowFromSettings
//
// Purpose:
///   Updates this window's widgets to match the currently connected Source.
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
ELSources::UpdateWindowFromSettings()
{
    blockSignals(true);
    setCurrentIndex(int(source->sourcetype));
    blockSignals(false);

    // file tab
    int sourceindex = 0;
    for (int i=1; i<combo->count(); ++i)
    {
        QStringList data = combo->itemData(i).toStringList();
        if (data.size() == 2 &&
            data[0] == source->file.c_str() &&
            data[1] == source->mesh.c_str())
        {
            sourceindex = i;
            break;
        }
    }
    combo->blockSignals(true);
    combo->setCurrentIndex(sourceindex);
    combo->blockSignals(false);
}
