#ifndef DS_INFO_H
#define DS_INFO_H

struct DSInfo
{
    vector<string> nodalfields;
    vector<string> cellsets;
    map<string, vector<string> > cellsetfields;

    void Print(std::ostream &out)
    {
        out << "Nodal:\n";
        for (int i=0; i<nodalfields.size(); i++)
        {
            out << "  " << nodalfields[i] << endl;
        }

        out << "CellSets:\n";
        for (int i=0; i<cellsets.size(); i++)
        {
            out << "  " << cellsets[i] << endl;
        }

        for (map<string,vector<string> >::iterator it = cellsetfields.begin();
             it != cellsetfields.end() ; ++it)
        {
            cerr << "CellSet "<<it->first<<endl;
            for (int i=0; i<it->second.size(); i++)
            {
                out << "  " << it->second[i] << endl;
            }

            
        }
        
    }
};

#endif
