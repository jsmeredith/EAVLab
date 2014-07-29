// Copyright 2012-2013 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef EL_ATTRIBUTE_CONTROL_H
#define EL_ATTRIBUTE_CONTROL_H

#include <QWidget>
#include <QLineEdit>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>

#include "STL.h"
#include "Attribute.h"

// ****************************************************************************
// Class:  ELAttributeControl
//
// Purpose:
///   Creates a set of controls for any given Attribute.
//
// Programmer:  Jeremy Meredith
// Creation:    August 13, 2012
//
// Modifications:
// ****************************************************************************
class ELAttributeControl : public QWidget
{
    Q_OBJECT
  protected:
    Attribute *atts;
    vector<QLineEdit*> lineEdits;
    vector<QCheckBox*> checkBoxes;
    QPushButton *applyButton;
    QGridLayout *layout;
    bool created;
  public:
    ELAttributeControl(QWidget *parent, Qt::WindowFlags f = 0);
    virtual void ConnectAttributes(Attribute *a);
  public slots:
    void UpdateWindowFromAtts();
    void UpdateAttsFromWindow();
  signals:
    void settingsChanged(Attribute*);
};


#endif
