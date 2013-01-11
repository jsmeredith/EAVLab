// Copyright 2012-2013 UT-Battelle, LLC.  See LICENSE.txt for more information.
#include "ELWindowFrame.h"

#include "ELWindowManager.h"

#include <QPainter>
#include <QMouseEvent>
#include <QMenu>
#include <QPushButton>

// ****************************************************************************
// Constructor:  ELWindowFrame::ELWindowFrame
//
// Programmer:  Jeremy Meredith
// Creation:    August 15, 2012
//
// Modifications:
// ****************************************************************************
ELWindowFrame::ELWindowFrame(int i, ELWindowManager *parent)
    : QWidget(parent)
{
    index = i;
    win = NULL;
    manager = parent;
    setContentsMargins(3,3,3,3);

    topLayout = new QGridLayout(this);
    topLayout->setContentsMargins(0,0,0,0);

    activateButton = new QPushButton("Settings",this);
    activateButton->setCheckable(true);
    connect(activateButton, SIGNAL(toggled(bool)),
            this, SLOT(activeToggled(bool)));
    topLayout->addWidget(activateButton, 0,0);

    changeTypeList = new QComboBox(this);
    changeTypeList->addItem("(empty)");
    changeTypeList->addItem("Text Summary");
    changeTypeList->addItem("Curve View");
    changeTypeList->addItem("2D View");
    changeTypeList->addItem("3D View");
    connect(changeTypeList, SIGNAL(currentIndexChanged(const QString &)),
            this, SLOT(WindowTypeChanged(const QString &)));
    topLayout->addWidget(changeTypeList, 0,1);

    //
    // pipeline selection
    //
    QPushButton *pipeButton = new QPushButton("Visible Pipelines", this);
    topLayout->addWidget(pipeButton, 0,2);

    ///\todo: hack: assuming 4 pipelines
    QMenu *pipelineMenu = new QMenu();
    QAction *p_cur = pipelineMenu->addAction("current pipeline");
    p_cur->setCheckable(true);
    p_cur->setChecked(true);
    p_cur->setData(-1);
    pipelineMenu->addSeparator();
    QAction *p_0 = pipelineMenu->addAction("pipeline #1's name");
    QAction *p_1 = pipelineMenu->addAction("pipeline #2's name");
    QAction *p_2 = pipelineMenu->addAction("pipeline #3's name");
    QAction *p_3 = pipelineMenu->addAction("pipeline #4's name");
    p_0->setCheckable(true);
    p_0->setData(0);
    p_1->setCheckable(true);
    p_1->setData(1);
    p_2->setCheckable(true);
    p_2->setData(2);
    p_3->setCheckable(true);
    p_3->setData(3);
    connect(p_cur, SIGNAL(toggled(bool)),
            this, SLOT(pipelineChoiceToggled(bool)));
    connect(p_0, SIGNAL(toggled(bool)),
            this, SLOT(pipelineChoiceToggled(bool)));
    connect(p_1, SIGNAL(toggled(bool)),
            this, SLOT(pipelineChoiceToggled(bool)));
    connect(p_2, SIGNAL(toggled(bool)),
            this, SLOT(pipelineChoiceToggled(bool)));
    connect(p_3, SIGNAL(toggled(bool)),
            this, SLOT(pipelineChoiceToggled(bool)));
    watchedPipelines.resize(5, false); // the current one + 4 others
    watchedPipelines[0] = true;

    pipeButton->setMenu(pipelineMenu);

    SetActive(false);
}


// ****************************************************************************
// Method:  ELWindowFrame::SetActive
//
// Purpose:
///   Set whether or not this window is active.
//
// Arguments:
//   a          the bool active state
//
// Programmer:  Jeremy Meredith
// Creation:    August  3, 2012
//
// Modifications:
// ****************************************************************************
void
ELWindowFrame::SetActive(bool a)
{
    active = a;
    if (active)
    {
        activateButton->blockSignals(true);
        activateButton->setChecked(true);
        //activateButton->setText("ACTIVE");
        activateButton->blockSignals(false);
        //activateButton->setEnabled(false);
    }
    else
    {
        activateButton->blockSignals(true);
        activateButton->setChecked(false);
        //activateButton->setText("Activate");
        activateButton->blockSignals(false);
        activateButton->setEnabled(true);
    }
    repaint();
}


// ****************************************************************************
// Method:  ELWindowFrame::paintFrame
//
// Purpose:
///   Paints the frame of this widget differently based on whether
///   or not it is active.
//
// Arguments:
//   none
//
// Programmer:  Jeremy Meredith
// Creation:    August  3, 2012
//
// Modifications:
// ****************************************************************************
void
ELWindowFrame::paintFrame()
{
    QRect r(rect());
    QPainter painter(this);

    if (active)
        painter.setPen(QColor(0,0,255));
    else
        painter.setPen(QColor(80,80,80));
    painter.drawRect(r.x()+0, r.y()+0, r.width()-1, r.height()-1);

    if (active)
        painter.setPen(QColor(88,88,255));
    else
        painter.setPen(QColor(150,150,150));
    painter.drawRect(r.x()+1, r.y()+1, r.width()-3, r.height()-3);

    if (active)
        painter.setPen(QColor(128,128,255));
    else
        painter.setPen(QColor(200,200,200));
    painter.drawRect(r.x()+2, r.y()+2, r.width()-5, r.height()-5);

}

// ****************************************************************************
// Method:  ELWindowFrame::paintEvent
//
// Purpose:
///   Reimplemented to allow painting of the frame based on active state.
//
// Arguments:
//   ev         the paint event
//
// Programmer:  Jeremy Meredith
// Creation:    August  3, 2012
//
// Modifications:
// ****************************************************************************
void
ELWindowFrame::paintEvent(QPaintEvent *ev)
{
    paintFrame();
    QWidget::paintEvent(ev);
}


// ****************************************************************************
// Method:  ELWindowFrame::SetWindow
//
// Purpose:
///   Replaces the window in this frame with a new one.
///   Note that we have to connect up some signals between the window
///   and this frame.
//
// Arguments:
//   ev         the paint event
//
// Programmer:  Jeremy Meredith
// Creation:    August  3, 2012
//
// Modifications:
// ****************************************************************************
void
ELWindowFrame::SetWindow(QWidget *w)
{
    if (win)
    {
        win->hide();
        delete win;
    }
    win = w;
    topLayout->addWidget(w, 1, 0, 2, 3);

    // is this really the cleanest way to tell a new window
    // which pipelines it's watching??
    connect(this, SIGNAL(watchedPipelinesChanged(vector<bool>)),
            win, SLOT(watchedPipelinesChanged(vector<bool>)));
    emit watchedPipelinesChanged(watchedPipelines);
}

// ****************************************************************************
// Method:  ELWindowFrame::GetWindow
//
// Purpose:
///   Simply returns the window contained in this frame.
//
// Arguments:
//   none
//
// Programmer:  Jeremy Meredith
// Creation:    August  3, 2012
//
// Modifications:
// ****************************************************************************
QWidget *
ELWindowFrame::GetWindow()
{
    return win;
}


// ****************************************************************************
// Method:  ELWindowFrame::activeToggled
//
// Purpose:
///   Slot for when the "active" (currently "settings") toggle is toggled.
//
// Arguments:
//   checked    the new state
//
// Programmer:  Jeremy Meredith
// Creation:    August  3, 2012
//
// Modifications:
// ****************************************************************************
void
ELWindowFrame::activeToggled(bool checked)
{
    if (checked)
        manager->SetActiveWindowFrame(this);
    else
        manager->SetActiveWindowFrame(NULL);
}

// ****************************************************************************
// Method:  ELWindowFrame::
//
// Purpose:
///   Slot for when one of the piplelines watch state is changed.
//
// Arguments:
//   checked    the new state
//
// Programmer:  Jeremy Meredith
// Creation:    August  3, 2012
//
// Modifications:
// ****************************************************************************
void
ELWindowFrame::pipelineChoiceToggled(bool checked)
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (!action)
        return;

    if (action->data().isValid())
    {
        int pipe = action->data().toInt();
        //cerr << "setting pipe "<<pipe<<" watch to "<<checked<<endl;
        watchedPipelines[pipe+1] = checked;
        emit watchedPipelinesChanged(watchedPipelines);
    }
}


// ****************************************************************************
// Method:  ELWindowFrame::WindowTypeChanged
//
// Purpose:
///   Slot for when the window type callback is changed.
///   \todo: Currently defers to a signal to the main window to 
///   change the type, which is a bit messy.
//
// Arguments:
//   type       the new type name as chosen in the combo box
//
// Programmer:  Jeremy Meredith
// Creation:    August  3, 2012
//
// Modifications:
// ****************************************************************************
void
ELWindowFrame::WindowTypeChanged(const QString &type)
{
    if (type == "(empty)")
        return;

    emit ChangeWindowType(index, type);
    ///\todo: a bit of a hack here, just to remove the "empty" designator. 
    /// this also lets us set the combo box index from a caller, though.
    changeTypeList->blockSignals(true);
    changeTypeList->clear();
    changeTypeList->addItem("Text Summary");
    changeTypeList->addItem("Curve View");
    changeTypeList->addItem("2D View");
    changeTypeList->addItem("3D View");
    for (int i=0; i<changeTypeList->count(); ++i)
    {
        if (changeTypeList->itemText(i) == type)
        {
            changeTypeList->setCurrentIndex(i);
            break;
        }
    }
    changeTypeList->blockSignals(false);
}

