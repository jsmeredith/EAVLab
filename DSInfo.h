#ifndef DS_INFO_H
#define DS_INFO_H

struct FieldInfo
{
    string name;
    int    ncomp;
    double minval;
    double maxval;
    double minmag;
    double maxmag;
};

///\todo: we're not using PointsInfo yet in DSInfo
struct PointsInfo
{
    int spatialdim;
};

struct CellSetInfo
{
    string name;
    int topodim;
};

struct DSInfo
{
    vector<FieldInfo> nodalfields;
    vector<CellSetInfo> cellsets;
    map<string, vector<FieldInfo> > cellsetfields;

    void Print(std::ostream &out)
    {
        out << "Nodal:\n";
        for (int i=0; i<nodalfields.size(); i++)
        {
            out << "  " << nodalfields[i].name << endl;
        }

        out << "CellSets:\n";
        for (int i=0; i<cellsets.size(); i++)
        {
            out << "  " << cellsets[i].name << endl;
        }

        for (map<string,vector<FieldInfo> >::iterator it = cellsetfields.begin();
             it != cellsetfields.end() ; ++it)
        {
            cerr << "CellSet "<<it->first<<endl;
            for (int i=0; i<it->second.size(); i++)
            {
                out << "  " << it->second[i].name << endl;
            }

            
        }
        
    }
};

#endif
