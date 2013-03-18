// Copyright 2012-2013 UT-Battelle, LLC.  See LICENSE.txt for more information.
#include "ELWindowManager.h"

#include "STL.h"

#include <QFileInfo>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QGridLayout>
#include <QMenu>
#include <QPaintEvent>
#include <QPushButton>

#include "ELBasicInfoWindow.h"
#include "EL3DWindow.h"
#include "EL2DWindow.h"
//#include "EL1DWindow.h"
#include "ELEmptyWindow.h"

struct Arrangement
{
    int n;
    const char *name;
    const char *xpm[16];
    int gw, gh;
    int x[4], y[4], w[4], h[4];
};

#define ax1 {    \
"13 13 2 1",     \
"  c #None",      \
"x c #FFFFFF",   \
"             ", \
" xxxxxxxxxxx ", \
" xxxxxxxxxxx ", \
" xxxxxxxxxxx ", \
" xxxxxxxxxxx ", \
" xxxxxxxxxxx ", \
" xxxxxxxxxxx ", \
" xxxxxxxxxxx ", \
" xxxxxxxxxxx ", \
" xxxxxxxxxxx ", \
" xxxxxxxxxxx ", \
" xxxxxxxxxxx ", \
"             "  \
}

#define ax2h {   \
"13 13 2 1",     \
"  c #None",      \
"x c #FFFFFF",   \
"             ", \
" xxxxx xxxxx ", \
" xxxxx xxxxx ", \
" xxxxx xxxxx ", \
" xxxxx xxxxx ", \
" xxxxx xxxxx ", \
" xxxxx xxxxx ", \
" xxxxx xxxxx ", \
" xxxxx xxxxx ", \
" xxxxx xxxxx ", \
" xxxxx xxxxx ", \
" xxxxx xxxxx ", \
"             "  \
}

#define ax2v {   \
"13 13 2 1",     \
"  c #None",      \
"x c #FFFFFF",   \
"             ", \
" xxxxxxxxxxx ", \
" xxxxxxxxxxx ", \
" xxxxxxxxxxx ", \
" xxxxxxxxxxx ", \
" xxxxxxxxxxx ", \
"             ", \
" xxxxxxxxxxx ", \
" xxxxxxxxxxx ", \
" xxxxxxxxxxx ", \
" xxxxxxxxxxx ", \
" xxxxxxxxxxx ", \
"             "  \
}

#define ax2a {   \
"13 13 2 1",     \
"  c #None",      \
"x c #FFFFFF",   \
"             ", \
" xxxxxxxxxxx ", \
" xxxxxxxxxxx ", \
" xxxxxxxxxxx ", \
" xxxxxxxxxxx ", \
" xxxxxxxxxxx ", \
" xxxxxxxxxxx ", \
" xxxxxxxxxxx ", \
" xxxxxxxxxxx ", \
"             ", \
" xxxxxxxxxxx ", \
" xxxxxxxxxxx ", \
"             "  \
}

#define ax3h {   \
"13 13 2 1",     \
"  c #None",      \
"x c #FFFFFF",   \
"             ", \
" xxx xxx xxx ", \
" xxx xxx xxx ", \
" xxx xxx xxx ", \
" xxx xxx xxx ", \
" xxx xxx xxx ", \
" xxx xxx xxx ", \
" xxx xxx xxx ", \
" xxx xxx xxx ", \
" xxx xxx xxx ", \
" xxx xxx xxx ", \
" xxx xxx xxx ", \
"             "  \
}

#define ax3v {   \
"13 13 2 1",     \
"  c #None",      \
"x c #FFFFFF",   \
"             ", \
" xxxxxxxxxxx ", \
" xxxxxxxxxxx ", \
" xxxxxxxxxxx ", \
"             ", \
" xxxxxxxxxxx ", \
" xxxxxxxxxxx ", \
" xxxxxxxxxxx ", \
"             ", \
" xxxxxxxxxxx ", \
" xxxxxxxxxxx ", \
" xxxxxxxxxxx ", \
"             "  \
}

#define ax3a {   \
"13 13 2 1",     \
"  c #None",      \
"x c #FFFFFF",   \
"             ", \
" xxxxxxxxxxx ", \
" xxxxxxxxxxx ", \
" xxxxxxxxxxx ", \
" xxxxxxxxxxx ", \
" xxxxxxxxxxx ", \
"             ", \
" xxxxx xxxxx ", \
" xxxxx xxxxx ", \
" xxxxx xxxxx ", \
" xxxxx xxxxx ", \
" xxxxx xxxxx ", \
"             "  \
}

#define ax3b {   \
"13 13 2 1",     \
"  c #None",      \
"x c #FFFFFF",   \
"             ", \
" xxxxx xxxxx ", \
" xxxxx xxxxx ", \
" xxxxx xxxxx ", \
" xxxxx xxxxx ", \
" xxxxx xxxxx ", \
" xxxxx       ", \
" xxxxx xxxxx ", \
" xxxxx xxxxx ", \
" xxxxx xxxxx ", \
" xxxxx xxxxx ", \
" xxxxx xxxxx ", \
"             "  \
}

#define ax4 {    \
"13 13 2 1",     \
"  c #None",      \
"x c #FFFFFF",   \
"             ", \
" xxxxx xxxxx ", \
" xxxxx xxxxx ", \
" xxxxx xxxxx ", \
" xxxxx xxxxx ", \
" xxxxx xxxxx ", \
"             ", \
" xxxxx xxxxx ", \
" xxxxx xxxxx ", \
" xxxxx xxxxx ", \
" xxxxx xxxxx ", \
" xxxxx xxxxx ", \
"             "  \
}

#define ax4v {   \
"13 13 2 1",     \
"  c #None",      \
"x c #FFFFFF",   \
"             ", \
" xxxxxxxxxxx ", \
" xxxxxxxxxxx ", \
"             ", \
" xxxxxxxxxxx ", \
" xxxxxxxxxxx ", \
"             ", \
" xxxxxxxxxxx ", \
" xxxxxxxxxxx ", \
"             ", \
" xxxxxxxxxxx ", \
" xxxxxxxxxxx ", \
"             "  \
}

#define ax4a {   \
"13 13 2 1",     \
"  c #None",      \
"x c #FFFFFF",   \
"             ", \
" xxxxxxxxxxx ", \
" xxxxxxxxxxx ", \
" xxxxxxxxxxx ", \
" xxxxxxxxxxx ", \
" xxxxxxxxxxx ", \
"             ", \
" xxx xxx xxx ", \
" xxx xxx xxx ", \
" xxx xxx xxx ", \
" xxx xxx xxx ", \
" xxx xxx xxx ", \
"             "  \
}

#define ax4b {   \
"13 13 2 1",     \
"  c #None",      \
"x c #FFFFFF",   \
"             ", \
" xxxxx xxxxx ", \
" xxxxx xxxxx ", \
" xxxxx xxxxx ", \
" xxxxx       ", \
" xxxxx xxxxx ", \
" xxxxx xxxxx ", \
" xxxxx xxxxx ", \
" xxxxx       ", \
" xxxxx xxxxx ", \
" xxxxx xxxxx ", \
" xxxxx xxxxx ", \
"             "  \
}

#define ax4c {   \
"13 13 2 1",     \
"  c #None",      \
"x c #FFFFFF",   \
"             ", \
" xxxxx xxxxx ", \
" xxxxx xxxxx ", \
" xxxxx xxxxx ", \
" xxxxx xxxxx ", \
" xxxxx xxxxx ", \
" xxxxx       ", \
" xxxxx xx xx ", \
" xxxxx xx xx ", \
" xxxxx xx xx ", \
" xxxxx xx xx ", \
" xxxxx xx xx ", \
"             "  \
}


Arrangement arrangements[] = 
{
  // n,  id,   xpm,  W,H, x[], y[], w[], h[]
    {1, "1",   ax1,  1,1, {0}, {0}, {1}, {1}},
    {2, "2h",  ax2h, 2,1, {0,1}, {0,0}, {1,1}, {1,1}},
    {2, "2v",  ax2v, 1,2, {0,0}, {0,1}, {1,1}, {1,1}},
    {2, "2a",  ax2a, 1,6, {0,0}, {0,5}, {1,1}, {5,1}},
    {3, "3h",  ax3h, 3,1, {0,1,2}, {0,0,0}, {1,1,1}, {1,1,1}},
    {3, "3v",  ax3v, 1,3, {0,0,0}, {0,1,2}, {1,1,1}, {1,1,1}},
    {3, "3a",  ax3a, 2,2, {0,0,1}, {0,1,1}, {2,1,1}, {1,1,1}},
    {3, "3b",  ax3b, 2,2, {0,1,1}, {0,0,1}, {1,1,1}, {2,1,1}},
    {4, "4",   ax4,  2,2, {0,1,0,1}, {0,0,1,1}, {1,1,1,1}, {1,1,1,1}},
    {4, "4v",  ax4v, 1,4, {0,0,0,0}, {0,1,2,3}, {1,1,1,1}, {1,1,1,1}},
    {4, "4a",  ax4a, 3,2, {0,0,1,2}, {0,1,1,1}, {3,1,1,1}, {1,1,1,1}},
    {4, "4b",  ax4b, 3,3, {0,2,2,2}, {0,0,1,2}, {2,1,1,1}, {3,1,1,1}},
    {4, "4c",  ax4c, 4,4, {0,2,2,3}, {0,0,2,2}, {2,2,1,1}, {4,2,2,2}},
    {0, NULL, {}, 0,0, {}, {}, {}, {}}
};
    

// ****************************************************************************
// Constructor:  ELWindowManager::ELWindowManager
//
// Programmer:  Jeremy Meredith
// Creation:    August  3, 2012
//
// Modifications:
// ****************************************************************************
ELWindowManager::ELWindowManager(QWidget *parent)
    : QWidget(parent)
{
    QGridLayout *topLayout = new QGridLayout(this);
    topLayout->setContentsMargins(0,0,0,0);

    QMenu *arrangementMenu = new QMenu();
    for (int i=0; arrangements[i].n > 0; i++)
    {
        if (i>0 && arrangements[i-1].name[0] != arrangements[i].name[0])
            arrangementMenu->addSeparator();
        QAction *action = arrangementMenu->addAction(QPixmap(arrangements[i].xpm),
                                                     arrangements[i].name);
        action->setIconVisibleInMenu(true);
        action->setData(QString(arrangements[i].name));
        connect(action, SIGNAL(triggered()),
                this, SLOT(arrangementChosen()));
    }

    QPushButton *arrangementButton = new QPushButton("Arrangement", this);
    arrangementButton->setMenu(arrangementMenu);
    topLayout->addWidget(arrangementButton, 0,0, 1,1);
    
    windowLayout = new QGridLayout();
    windowLayout->setSpacing(2);
    topLayout->addLayout(windowLayout, 1,0, 2,1);
    for (int i=0; i<MAX_WINDOWS; i++)
    {
        windowframes[i] = NULL;
        settings[i] = NULL;
    }

    activeWindow = -1;
    arrangementIndex = -1;
    SetArrangement("1");
}

// ****************************************************************************
// Method:  ELWindowManager::SetArrangement
//
// Purpose:
///   Sets an arrangement based on its short textual name (like "2h").
//
// Programmer:  Jeremy Meredith
// Creation:    August  3, 2012
//
// Modifications:
// ****************************************************************************
void
ELWindowManager::SetArrangement(const std::string &name)
{
    arrangementIndex = -1;
    for (int i=0; arrangements[i].n > 0; i++)
    {
        if (name == arrangements[i].name)
            arrangementIndex = i;
    }

    if (arrangementIndex < 0)
        return;

    Arrangement &a = arrangements[arrangementIndex];
    // hide existing windows that won't show up
    for (int i=a.n; i<MAX_WINDOWS; i++)
    {
        if (windowframes[i])
        {
            windowframes[i]->hide();            
        }
    }

    // remove all items from layout
    for (int i=0; i<windowLayout->count(); i++)
        windowLayout->removeItem(windowLayout->itemAt(i));

    // create new windows 
    for (int i=0; i<a.n; i++)
    {
        if (!windowframes[i])
        {
            windowframes[i] = new ELWindowFrame(i, this);
            windowframes[i]->SetWindow(new ELEmptyWindow(this));
            connect(windowframes[i], SIGNAL(ChangeWindowType(int, const QString &)),
                    this, SLOT(ChangeWindowType(int, const QString &)));
            emit WindowAdded(windowframes[i]->GetWindow());
        }

        windowLayout->addWidget(windowframes[i],
                                a.y[i],a.x[i], a.h[i],a.w[i]);
        windowframes[i]->show();
    }

    // set spacing
    int maxrow = 0;
    int maxcol = 0;
    for (int i=0; i<a.n; i++)
    {
        if (maxrow < a.y[i] + a.h[i]-1)
            maxrow = a.y[i] + a.h[i]-1;
        if (maxcol < a.x[i] + a.w[i]-1)
            maxcol = a.x[i] + a.w[i]-1;
    }
    for (int i=0; i<maxrow+1; i++)
        windowLayout->setRowStretch(i, 100);
    for (int i=maxrow+1; i<windowLayout->rowCount(); i++)
        windowLayout->setRowStretch(i, 0);
    for (int i=0; i<maxcol+1; i++)
        windowLayout->setColumnStretch(i, 100);
    for (int i=maxcol+1; i<windowLayout->columnCount(); i++)
        windowLayout->setColumnStretch(i, 0);
    windowLayout->invalidate();
}

// ****************************************************************************
// Method:  ELWindowManager::arrangementChosen
//
// Purpose:
///   Qt slot for when a user chooses a new arrangement.
//
// Programmer:  Jeremy Meredith
// Creation:    August  3, 2012
//
// Modifications:
// ****************************************************************************
void
ELWindowManager::arrangementChosen()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (!action)
        return;

    if (action->data().isValid())
    {
        QString name = action->data().toString();
        SetArrangement(name.toStdString());
    }
}


// ****************************************************************************
// Method:  
//
// Purpose:
///   We set the old active window as inactive
///   and set the new active window as active.
///   No, this is not elegant.
//
// Arguments:
//   winframe   the new window to make active
//
// Programmer:  Jeremy Meredith
// Creation:    August  3, 2012
//
// Modifications:
// ****************************************************************************
void
ELWindowManager::SetActiveWindowFrame(ELWindowFrame *winframe)
{
    if (activeWindow >= 0 && windowframes[activeWindow])
        windowframes[activeWindow]->SetActive(false);
    activeWindow = -1;
    for (int i=0; i<MAX_WINDOWS; i++)
    {
        if (windowframes[i] && windowframes[i]==winframe)
        {
            activeWindow = i;
            windowframes[i]->SetActive(true);
            emit SettingsActivated(settings[i]);
            break;
        }
    }
    if (activeWindow == -1)
        emit SettingsActivated(NULL);
}


// ****************************************************************************
// Method:  ELWindowManager::SetWindow
//
// Purpose:
///   Replace a window with a new one (or set one if it didn't yet exist).
///   Set the new window as active if appropriate.
//
// Arguments:
//   index      the window index to return
//   w          the new window
//
// Programmer:  Jeremy Meredith
// Creation:    August  3, 2012
//
// Modifications:
// ****************************************************************************
void
ELWindowManager::SetWindow(int index, QWidget *win, QWidget *s)
{
    if (arrangementIndex < 0)
        return;

    Arrangement &a = arrangements[arrangementIndex];

    windowframes[index]->SetWindow(win);
    settings[index] = s;

    if (a.n >= index)
    {
        windowLayout->addWidget(windowframes[index],
                                a.y[index],a.x[index], a.h[index],a.w[index]);
    }
    else
    {
        win->hide();
    }
}


// ****************************************************************************
// Method:  ELWindowManager::GetWindow
//
// Purpose:
///   Return the window with the given index.
//
// Arguments:
//   index      the window index to return
//
// Programmer:  Jeremy Meredith
// Creation:    August  3, 2012
//
// Modifications:
// ****************************************************************************
QWidget *
ELWindowManager::GetWindow(int index)
{
    return windowframes[index]->GetWindow();
}


// ****************************************************************************
// Method:  ELWindowManager::ChangeWindowType
//
// Purpose:
///   Slot to change window type; actually create and hook up the window.
//
// Arguments:
//   index      the window index to change
//   type       the name of the new window type to create
//
// Programmer:  Jeremy Meredith
// Creation:    August  20, 2012
//
// Modifications:
// ****************************************************************************
void
ELWindowManager::ChangeWindowType(int index, const QString &type)
{
    if (type == "3D View")
    {
        EL3DWindow *newwin = new EL3DWindow(this);
        QWidget *newwinsettings = newwin->GetSettings();
        settings[index] = newwinsettings;
        windowframes[index]->SetWindow(newwin);
        emit WindowAdded(windowframes[index]->GetWindow());
    }
    else if (type == "2D View")
    {
        EL2DWindow *newwin = new EL2DWindow(this);
        QWidget *newwinsettings = newwin->GetSettings();
        settings[index] = newwinsettings;
        windowframes[index]->SetWindow(newwin);
        emit WindowAdded(windowframes[index]->GetWindow());
    }
    /*else if (type == "1D View")
    {
        EL1DWindow *newwin = new EL1DWindow(this);
        QWidget *newwinsettings = newwin->GetSettings();
        settings[index] = newwinsettings;
        windowframes[index]->SetWindow(newwin);
        emit WindowAdded(windowframes[index]->GetWindow());
    }*/
    else if (type == "Text Summary")
    {
        windowframes[index]->SetWindow(new ELBasicInfoWindow(this));
        emit WindowAdded(windowframes[index]->GetWindow());
    }
    else
    {
        cerr << "sorry, didn't implement window type "<<type.toStdString()<<" yet\n";
    }
    ///\todo: hack to set the combo box when called from a client
    /// instead o as a signal from the frame itself
    if (!sender())
        windowframes[index]->WindowTypeChanged(type);
}

