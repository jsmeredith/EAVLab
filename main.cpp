// Copyright 2012 UT-Battelle, LLC.  See LICENSE.txt for more information.
#include <QtGui/QApplication>
#include "ELMainWindow.h"

#include <eavlDataSet.h>
#include <eavlException.h>
#include <eavlCUDA.h>

int main(int argc, char *argv[])
{
    try
    {
        eavlInitializeGPU();

        QApplication a(argc, argv);

        ELMainWindow w;
        
        // want a bigger font? hardcode it here
        if (false)
        {
            QFont f = w.font();
            f.setPixelSize(20);
            w.setFont(f);
        }

        if (argc == 1)
            ;// do nothing
        else if (argc == 2)
            w.OpenFile(argv[1]);
        else
            cerr << "Error: unexpected extra arguments\n";

        w.show();

        return a.exec();
    }
    catch (const eavlException &e)
    {
        cerr << e.GetErrorText() << endl;
        return -1;
    }
    return 0;
}
