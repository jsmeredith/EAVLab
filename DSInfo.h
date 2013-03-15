#ifndef DS_INFO_H
#define DS_INFO_H

struct DSInfo
{
    vector<string> nodalfields;
    vector<string> cellsets;
    map<string, vector<string> > cellsetfields;
};

#endif
