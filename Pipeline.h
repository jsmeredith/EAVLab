// Copyright 2012 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef PIPELINE_H
#define PIPELINE_H

#include "STL.h"
#include "eavlImporter.h"
#include "Operation.h"
#include <QFileInfo>

///\todo: hack; we're defining a fixed number of global pipelines
#define NUMPIPES 4
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
    enum SourceType { File, Geometry, Pipe };
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
    eavlDataSet *result;

  public:
    ///\todo: hack: everyone needs to access these
    static vector<Pipeline*> allPipelines;

  public:
    Pipeline() : source(new Source), result(NULL)
    {
    }

    vector<string> GetVariables()
    {
        if (source->sourcetype != Source::File)
            throw eavlException("can only execute from source file");

        if (!source->source_file)
            throw eavlException("no source file selected");

        vector<string> vars;
        vars.push_back(source->mesh);

        vector<string> sourcevars = source->source_file->GetFieldList(source->mesh);
        vars.insert(vars.end(), sourcevars.begin(), sourcevars.end());
        return vars;

        ///\todo: ask each operator what vars it can create, too.
        /// how does it know what vars will be input to it?
        /// well, now we need the metadata (forward part of contract?)
    }

    void Execute()
    {
        ///\todo: cache some stuff, at least the reading

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
        /// we need to change this so it only reads what's asked of it
        std::vector<std::string> vars = source->source_file->GetFieldList(source->mesh);
#endif


        // read the mesh and vars
        ///\todo: only reading chunk 0 for now
        eavlDataSet *ds = source->source_file->GetMesh(source->mesh, 0);
        for (size_t i=0; i<vars.size(); i++)
        {
            eavlField *f = source->source_file->GetField(vars[i], source->mesh, 0);
            ds->fields.push_back(f);
        }

        // execute each operation
        for (size_t i=0; i<ops.size(); i++)
        {
            ops[i]->SetInput(ds);
            ops[i]->Execute();
            ds = ops[i]->GetOutput();
        }

        result = ds;
    }
};


#endif
