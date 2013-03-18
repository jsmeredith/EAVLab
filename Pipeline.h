// Copyright 2012-2013 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef PIPELINE_H
#define PIPELINE_H

#include "STL.h"
#include "eavlImporter.h"
#include "Operation.h"
#include <QFileInfo>
#include "DSInfo.h"

struct Pipeline;

// ****************************************************************************
// Struct:  Source
//
// Purpose:
///   Encapsulates settings for some type of source.
//
// Programmer:  Jeremy Meredith
// Creation:    August 3, 2012
//
// Modifications:
// ****************************************************************************
struct Source
{
    enum SourceType { File, Pipe, Geometry };
    SourceType    sourcetype;

    Pipeline     *source_pipe;
    
    eavlImporter *source_file;
    std::string   file;
    std::string   mesh;
    //std::string   var;

  public:
    Source()
        : sourcetype(File),
          source_pipe(NULL),
          source_file(NULL),
          file(""), mesh("")
    {
    }
    string GetSourceType()
    {
        if (sourcetype == File && (file=="" || mesh==""))
            return "(no source)";
        else if (sourcetype == File)
            return "Read";
        else if (sourcetype == Geometry)
            return "Generate";
        else // sourcetype == Pipe
            return "Pipeline";
    }
    string GetSourceInfo()
    {
        if (sourcetype == File && (file=="" || mesh==""))
            return "";
        else if (sourcetype == File)
        {
            return QFileInfo(file.c_str()).fileName().toStdString() + ":" + mesh;
        }
        else if (sourcetype == Geometry)
        {
            return "(some shape?)";
        }
        else // sourcetype == Pipe
        {
            return "(some pipe ID)";
        }
    }
};

// ****************************************************************************
// Struct:  Pipeline
//
// Purpose:
///   Encapsulates a source and some operators with settings.
//
// Programmer:  Jeremy Meredith
// Creation:    August 3, 2012
//
// Modifications:
// ****************************************************************************
struct Pipeline
{
    Source *source;
    std::vector<Operation*> ops;
    /// results should have one more item in it than the ops array.
    /// e.g. ops[i] uses results[i] as input and outputs to results[i+1].
    /// result[0] is the initial data set.
    std::vector<eavlDataSet*> results;

  public:
    ///\todo: hack: everyone needs to access these
    static vector<Pipeline*> allPipelines;

  public:
    Pipeline() : source(new Source)
    {
    }

    string GetName()
    {
        return "testname";
    }

    DSInfo GetVariables(int index)
    {
        DSInfo dsinfo;
        try
        {
            Execute();
        }
        catch (const eavlException &e)
        {
            cerr << "Error: " <<e.GetErrorText() << endl;
            return dsinfo;
        }

        if (results.size() == 0)
            return dsinfo;

        eavlDataSet *ds = results.back();
        for (int j=0; j<ds->GetNumFields(); ++j)
        {
            eavlField *f = ds->GetField(j);
            if (f->GetAssociation() == eavlField::ASSOC_POINTS)
                dsinfo.nodalfields.push_back(f->GetArray()->GetName());
        }


        for (int i=0; i<ds->GetNumCellSets(); ++i)
        {
            eavlCellSet *cs = ds->GetCellSet(i);
            dsinfo.cellsets.push_back(cs->GetName());
            for (int j=0; j<ds->GetNumFields(); ++j)
            {
                eavlField *f = ds->GetField(j);
                if (f->GetAssociation() == eavlField::ASSOC_CELL_SET &&
                    f->GetAssocCellSet() == i)
                    dsinfo.cellsetfields[cs->GetName()].push_back(f->GetArray()->GetName());
            }
        }

        //cerr << ">> GetVariables <<\n"; dsinfo.Print(cerr);
        return dsinfo;
    }

    void Execute()
    {
        results.clear();

        if (results.size() == 0)
        {
            if (source->sourcetype != Source::File)
                throw eavlException("can only execute from source file");

            if (!source->source_file)
                throw eavlException("no source file selected");

#if 0
            // find the variables needed for each operation
            std::vector<std::string> vars;
            for (int i=ops.size()-1; i>=0; --i)
            {
                std::vector<std::string> newvars;
                newvars = ops[i]->GetNeededVariables();
                vars.insert(vars.end(), newvars.begin(), newvars.end());
            }
#else
            ///\todo: big hack: always read everything from the file;
            /// we eventually should change this so it only reads what's
            /// asked of it
            std::vector<std::string> vars = source->source_file->GetFieldList(source->mesh);
#endif


            // read the mesh and vars
            ///\todo: only reading chunk 0 for now
            eavlDataSet *ds = source->source_file->GetMesh(source->mesh, 0);

            ds = ds->CreateShallowCopy();
            for (size_t i=0; i<vars.size(); i++)
            {
                eavlField *f = source->source_file->GetField(vars[i], source->mesh, 0);
                ds->AddField(f);
            }
            results.push_back(ds);
        }

        while (results.size() <= ops.size())
        {
            eavlDataSet *ds = results.back();

            // \todo: hack: create a new data set structure so our mutators
            // don't quite so easily mess with the one in the importer.
            ds = ds->CreateShallowCopy();

            // execute each operation
            Operation *op = ops[results.size()-1];
            op->SetInput(ds);
            op->Execute();
            results.push_back(op->GetOutput());

            //cerr << "Executed next op, summary = \n";
            //op->GetOutput()->PrintSummary(cerr);
        }
    }
};


#endif
