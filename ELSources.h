// Copyright 2012-2013 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef EL_SOURCES_H
#define EL_SOURCES_H

#include "eavlImporter.h"
#include <QTabWidget>
#include "STL.h"

class QComboBox;
class QTreeWidget;
class QTreeWidgetItem;
class Source;

// ****************************************************************************
// Class:  ELSources
//
// Purpose:
///   Contain a set of open files and display their meshes.
//
// Programmer:  Jeremy Meredith
// Creation:    August  2, 2012
//
// Modifications:
// ****************************************************************************
class ELSources : public QTabWidget
{
    Q_OBJECT
  protected:
    std::map<std::string, eavlImporter*> openFiles;

    enum roles {
        fileRole = Qt::UserRole+0,
        meshRole,
        varRole
    };

    QComboBox *combo;
    Source *source;

  public:
    ELSources(QWidget *parent);
    void addSource(const std::string &fn, eavlImporter *imp);        
    eavlImporter *getImporter(const std::string &fn) { return openFiles[fn]; }
    void ConnectSettings(Source *s);
    void UpdateWindowFromSettings();

  public slots:
    void fileMeshChanged(int);
    void tabChanged(int);

  signals:
    void sourceChanged();
};

#endif
