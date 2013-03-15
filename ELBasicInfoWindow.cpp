// Copyright 2012-2013 UT-Battelle, LLC.  See LICENSE.txt for more information.
#include "ELBasicInfoWindow.h"

#include "Pipeline.h"


// ****************************************************************************
// Constructor:  ELBasicInfoWindow::ELBasicInfoWindow
//
// Programmer:  Jeremy Meredith
// Creation:    August  3, 2012
//
// Modifications:
// ****************************************************************************
ELBasicInfoWindow::ELBasicInfoWindow(ELWindowManager *parent)
    : QWidget(parent)
{
    QGridLayout *topLayout = new QGridLayout(this);
    topLayout->setContentsMargins(0,0,0,0);

    info = new QTextEdit(this);
    info->setReadOnly(true);
    info->setAcceptRichText(true);
    //info->installEventFilter(this); <- to catch focus.  Sadly,
    // this doesn't work; events like mouse press aren't getting to the 
    // filter (and it's not just because of read-only!)
    topLayout->addWidget(info);
}


// ****************************************************************************
// Method:  ELBasicInfoWindow::SetPipeline
//
// Purpose:
///   Set the pipeline this window is showing.
//
// Arguments:
//   index      the index of the pipeline
//   p          the pipeline to show
//
// Programmer:  Jeremy Meredith
// Creation:    August  3, 2012
//
// Modifications:
// ****************************************************************************
void
ELBasicInfoWindow::PipelineUpdated(int, Pipeline *p)
{
    FillFromPipeline(p);
}

// ****************************************************************************
// Method:  ELBasicInfoWindow::FillFromPipeline
//
// Purpose:
///   Fill the contents of this window from a given pipeline.
//
// Arguments:
//   p          the Pipeline to fill from
//
// Programmer:  Jeremy Meredith
// Creation:    August 21, 2012
//
// Modifications:
// ****************************************************************************
void
ELBasicInfoWindow::FillFromPipeline(Pipeline *p)
{
    info->clear();
    if (p->source->file.empty())
        return;

    Source *s = p->source;
    if (s->sourcetype != Source::File)
    {
        ///\todo
        cerr << "Only supporting file sources for now\n";
        return;
    }

    eavlImporter *importer = s->source_file;
    /*
    if (!s->var.empty())
    {
        info->insertHtml(("<b>Variable:</b> " + s->var + "<br>").c_str());
        info->insertHtml(("<b>in mesh:</b> " + s->mesh + "<br>").c_str());
        info->insertHtml(("<b>in file:</b> " + s->file + "<br>").c_str());
        info->insertHtml("Contains "
                         + QString::number(importer->GetNumChunks(s->mesh)) 
                          + " chunks<br>");
    }
    else*/
    if (!s->mesh.empty())
    {
        info->insertHtml(("<b>Mesh:</b> " + s->mesh + "<br>").c_str());
        info->insertHtml(("<b>in file:</b> " + s->file + "<br>").c_str());
        info->insertHtml("Contains "
                         + QString::number(importer->GetNumChunks(s->mesh))
                          + " chunks<br>");
    }
    else
    {
        info->insertHtml(("<b>File:</b> " + s->file + "<br>").c_str());
        info->insertHtml("Contains " +
                         QString::number(importer->GetMeshList().size()) 
                          + " meshes<br>");
    }
    if (p->results.size() > 0)
    {
        info->insertHtml("<br><b>Execution Result Follows:</b><br><br>");
        ostringstream out;
        p->results.back()->PrintSummary(out);
        info->insertPlainText(out.str().c_str());
    }
}

// ****************************************************************************
// Method:  ELBasicInfoWindow::CurrentPipelineChanged
//
// Purpose:
///   Tell this window what the currently-edited pipeline index is.
//
// Arguments:
//   index      the new pipeline being edited
//
// Programmer:  Jeremy Meredith
// Creation:    August 16, 2012
//
// Modifications:
// ****************************************************************************
void
ELBasicInfoWindow::CurrentPipelineChanged(int)
{
}
