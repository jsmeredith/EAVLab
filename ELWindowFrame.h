// Copyright 2012-2013 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef EL_WINDOW_FRAME_H
#define EL_WINDOW_FRAME_H

#include <QFrame>
#include <QGridLayout>
#include <QLabel>

#include "Pipeline.h"

#include <QPushButton>
#include <QComboBox>

class ELWindowManager;
class ELWindow;

// ****************************************************************************
// Class:  ELWindowFrame
//
// Purpose:
///   Common widget container for an ELWindow.  It contains common
///   controls for all window types.
//
// Programmer:  Jeremy Meredith
// Creation:    August 15, 2012
//
// Modifications:
// ****************************************************************************
class ELWindowFrame : public QWidget
{
    Q_OBJECT
  protected:
    bool active;
    int index;
    ELWindowManager *manager;
    QWidget *win;
    QGridLayout *topLayout;
    QPushButton *activateButton;
    QComboBox *changeTypeList;
    QComboBox *rendererList;
  public:
    ELWindowFrame(int index, ELWindowManager *parent);
    void SetActive(bool);
    void paintEvent(QPaintEvent*);
    void paintFrame();
    void SetWindow(QWidget *w);
    QWidget *GetWindow();
  public slots:
    void activeToggled(bool);
    void WindowTypeChanged(const QString &);
    void RendererChanged(const QString &);
  signals:
    void ChangeWindowType(int i, const QString &);
};

#endif
