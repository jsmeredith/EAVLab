// Copyright 2012-2013 UT-Battelle, LLC.  See LICENSE.txt for more information.
#include "ELMainWindow.h"

#include <QCheckBox>
#include <QFileDialog>
#include <QGridLayout>
#include <QInputDialog>
#include <QMenu>
#include <QMenuBar>
#include <QPushButton>
#include <QSplitter>

#include <sstream>
#include <cfloat>

#include "eavlImporterFactory.h"

#include "ELPipelineBuilder.h"
#include "ELWindowManager.h"
#include "ELBasicInfoWindow.h"

// ****************************************************************************
// Constructor:  ELMainWindow::ELMainWindow
//
// Programmer:  Jeremy Meredith
// Creation:    July 30, 2012
//
// Modifications:
// ****************************************************************************
ELMainWindow::ELMainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    statusBar();

    QMenu *file = new QMenu("File",this);

    QAction *open = file->addAction(tr("Open"));
    open->setShortcut(QString(tr("Ctrl+O")));
    QAction *exit = file->addAction(tr("Exit"));
    exit->setShortcut(QString(tr("Ctrl+X")));
    menuBar()->addMenu(file);

    connect(open, SIGNAL(triggered()),
            this, SLOT(OpenFile()));
    connect(exit, SIGNAL(triggered()),
            this, SLOT(Exit()));

    topSplitter = new QSplitter(Qt::Horizontal, this);

    //
    // workspace
    //
    pipelineBuilder = new ELPipelineBuilder(topSplitter);
    connect(pipelineBuilder, SIGNAL(pipelineUpdated(Pipeline*)),
            this, SLOT(PipelineUpdated(Pipeline*)));

    //
    // window settings
    //
    windowSettingsGroup = new QGroupBox("Window Settings", topSplitter);
    /*QGridLayout *windowSettingsLayout =*/ new QGridLayout(windowSettingsGroup);
    windowSettingsGroup->hide();
    activeSettingsWidget = NULL;

    //
    // windows
    //
    windowMgr = new ELWindowManager(topSplitter);
    connect(windowMgr, SIGNAL(WindowAdded(QWidget*)),
            this, SLOT(WindowAdded(QWidget*)));
    connect(windowMgr, SIGNAL(SettingsActivated(QWidget*)),
            this, SLOT(SettingsActivated(QWidget*)));

    topSplitter->setStretchFactor(0,40);
    topSplitter->setStretchFactor(1,1);
    topSplitter->setStretchFactor(2,100);
    setCentralWidget(topSplitter);

    // I guess we want to start with a 3D window
    windowMgr->ChangeWindowType(0, "2D View");
}

// ****************************************************************************
// Destructor:  ELMainWindow::~ELMainWindow
//
// Programmer:  Jeremy Meredith
// Creation:    July 30, 2012
//
// Modifications:
// ****************************************************************************
ELMainWindow::~ELMainWindow()
{
}

// ****************************************************************************
// Method:  ELMainWindow::Exit
//
// Purpose:
///   Slot for when File -> Exit is chosen.
//
// Programmer:  Jeremy Meredith
// Creation:    July 30, 2012
//
// Modifications:
// ****************************************************************************
void ELMainWindow::Exit()
{
    close();
}


// ****************************************************************************
// Method:  ELMainWindow::OpenFile
//
// Purpose:
///   Slot for File -> Open.  Let user choose a file, then open it.
//
// Programmer:  Jeremy Meredith
// Creation:    July 30, 2012
//
// Modifications:
// ****************************************************************************
void
ELMainWindow::OpenFile()
{
    QString extensions = "*.vtk *.bov *.pdb *.png";
#ifdef HAVE_SILO
    extensions += " *.silo";
    extensions += " *.chi";
#endif
#ifdef HAVE_NETCDF
    extensions += " *.nc";
#endif
#ifdef HAVE_HDF5
    extensions += " *.h5";
#endif
#ifdef HAVE_ADIOS
    extensions += " *.bp";
    extensions += " *.pixie";
#endif
    QString filename =  QFileDialog::getOpenFileName(this,
                                                     "Select File",
                                                     QString(),
                                                     extensions);
    if (filename.isNull())
        return;

    OpenFile(filename);
}

// ****************************************************************************
// Method:  ELMainWindow::OpenFile
//
// Purpose:
///   Actual method to open a file given a filename.
//
// Programmer:  Jeremy Meredith
// Creation:    July 30, 2012
//
// Modifications:
// ****************************************************************************
void
ELMainWindow::OpenFile(const QString &filename)
{
    eavlImporter *importer =
        eavlImporterFactory::GetImporterForFile(filename.toStdString());

    if (!importer)
    {
        cerr << "Error: unknown file extension\n";
        return;
    }
    
    pipelineBuilder->addSource(filename.toStdString(), importer);
}


// ****************************************************************************
// Method:  ELMainWindow::SetPipeline
//
// Purpose:
///   Set the pipeline this window is showing.
//
// Arguments:
//   index      the index of the pipeline
//   p          the pipeline to show
//
// Programmer:  Jeremy Meredith
// Creation:    August 16, 2012
//
// Modifications:
// ****************************************************************************
void
ELMainWindow::PipelineUpdated(Pipeline *)
{
    // currently useless
}

// ****************************************************************************
// Method:  ELMainWindow::WindowAdded
//
// Purpose:
///   Slot for when a new window is added.
//
// Arguments:
//   w          the new window (contained in the window frame)
//
// Programmer:  Jeremy Meredith
// Creation:    August  7, 2012
//
// Modifications:
// ****************************************************************************
void
ELMainWindow::WindowAdded(QWidget *w)
{
    connect(pipelineBuilder, SIGNAL(pipelineUpdated(Pipeline*)),
            w, SLOT(PipelineUpdated(Pipeline*)));    
    connect(pipelineBuilder, SIGNAL(CurrentPipelineChanged(int)),
            w, SLOT(CurrentPipelineChanged(int)));
}

// ****************************************************************************
// Method:  ELMainWindow::SettingsActivated
//
// Purpose:
///   Show a window's settings, or if NULL, just hide the current one.
//
// Arguments:
//   settings   the settings widget to show
//
// Programmer:  Jeremy Meredith
// Creation:    August  7, 2012
//
// Modifications:
// ****************************************************************************
void
ELMainWindow::SettingsActivated(QWidget *settings)
{
    if (activeSettingsWidget)
        activeSettingsWidget->hide();
    while (windowSettingsGroup->layout()->count() > 0)
        windowSettingsGroup->layout()->removeItem(windowSettingsGroup->layout()->itemAt(0));

    activeSettingsWidget = settings;

    if (settings)
    {
        windowSettingsGroup->layout()->addWidget(settings);
        settings->show();
        windowSettingsGroup->show();
    }
    else
    {
        windowSettingsGroup->hide();
    }
}

