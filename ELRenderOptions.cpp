// Copyright 2012-2013 UT-Battelle, LLC.  See LICENSE.txt for more information.
#include "ELRenderOptions.h"

#include <QPushButton>
#include <QGridLayout>

ELRenderOptions::ELRenderOptions(QWidget *parent)
    : ELAttributeControl(parent, Qt::Window)
{
}

void ELRenderOptions::ConnectAttributes(Attribute *a)
{
    if (created)
    {
        ELAttributeControl::ConnectAttributes(a);
    }
    else
    {
        ELAttributeControl::ConnectAttributes(a);
        
        QPushButton *dismissButton = new QPushButton("Dismiss", this);
        connect(dismissButton, SIGNAL(clicked()),
                this, SLOT(hide()));
        layout->addWidget(dismissButton, atts->GetNumFields(), 1);
    }
}
