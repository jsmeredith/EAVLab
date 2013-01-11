// Copyright 2012-2013 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef EL_WINDOW_MANAGER_H
#define EL_WINDOW_MANAGER_H

#include <QFrame>
#include <QGridLayout>
#include <QLabel>

#include "ELWindowFrame.h"

// ****************************************************************************
// Class:  ELWindowManager
//
// Purpose:
///   Contains a layout of ELWindows.
//
// Programmer:  Jeremy Meredith
// Creation:    August  3, 2012
//
// Modifications:
// ****************************************************************************
class ELWindowManager : public QWidget
{
    Q_OBJECT
  protected:
    #define MAX_WINDOWS 4

    ELWindowFrame *windowframes[MAX_WINDOWS];
    QWidget *settings[MAX_WINDOWS];
    int arrangementIndex;

    QGridLayout *windowLayout;
    int activeWindow;

  signals:
    void WindowAdded(QWidget*);
    void SettingsActivated(QWidget*);

  public:
    ELWindowManager(QWidget *parent);
    void SetArrangement(const std::string &name);
    void SetActiveWindowFrame(ELWindowFrame *);
    void SetWindow(int index, QWidget *, QWidget *);
    QWidget *GetWindow(int index);

  public slots:
    void arrangementChosen();
    void ChangeWindowType(int, const QString &);
};

#endif
