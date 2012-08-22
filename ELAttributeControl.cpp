// Copyright 2012 UT-Battelle, LLC.  See LICENSE.txt for more information.
#include "ELAttributeControl.h"

// ****************************************************************************
// Constructor:  
//
// Programmer:  Jeremy Meredith
// Creation:    August 13, 2012
//
// Modifications:
// ****************************************************************************
ELAttributeControl::ELAttributeControl(QWidget *parent)
    : QWidget(parent)
{
    atts = NULL;
    created = false;
}

// ****************************************************************************
// Method:  ELAttributeControl::ConnectAttributes
//
// Purpose:
///   Create the window controls given an Attribute.
//
// Arguments:
//   a          the new Attribute to start watching
//
// Programmer:  Jeremy Meredith
// Creation:    August 13, 2012
//
// Modifications:
// ****************************************************************************
void
ELAttributeControl::ConnectAttributes(Attribute *a)
{
    atts = a;

    if (created)
        return;

    created = true;
    QGridLayout *layout = new QGridLayout(this);

    if (!atts || atts->GetNumFields() == 0)
    {
        layout->addWidget(new QLabel("No settings for operator", this));
        return;
    }

    for (int i=0; i<atts->GetNumFields(); i++)
    {
        bool addlabel = true;
        QLineEdit *le = NULL;
        QCheckBox *cb = NULL;
        switch (atts->GetFieldTypeCategory(i))
        {
          case CategoryBoolean:
            ///\todo: this can't handle an array/vector of bools
            cb = new QCheckBox(atts->GetFieldName(i).c_str(), this);
            layout->addWidget(cb, i, 0,  1,2);
            addlabel = false;
            break;

          case CategoryIntegral:
          case CategoryReal:
          case CategoryString:
            ///\todo: this isn't really great for arr/vec of strings
            le = new QLineEdit(this);
            layout->addWidget(le, i, 1);
            break;

          default:
            // don't support other types yet
            break;
        }

        lineEdits.push_back(le);
        checkBoxes.push_back(cb);

        if (addlabel)
        {
            QLabel *lbl = new QLabel(atts->GetFieldName(i).c_str(),
                                     this);
            layout->addWidget(lbl, i, 0);
        }
    }

    applyButton = new QPushButton("Apply", this);
    layout->addWidget(applyButton, atts->GetNumFields(), 0);
    connect(applyButton, SIGNAL(clicked()),
            this, SLOT(UpdateAttsFromWindow()));
}

// ****************************************************************************
// Method:  ELAttributeControl::UpdateWindowFromAtts
//
// Purpose:
///   Set the state/contents of our widgets given the settings in the
///   currently watched Attribute.
//
// Arguments:
//   none
//
// Programmer:  Jeremy Meredith
// Creation:    August 13, 2012
//
// Modifications:
// ****************************************************************************
void
ELAttributeControl::UpdateWindowFromAtts()
{
    if (!atts || atts->GetNumFields() == 0)
        return;

    for (int i=0; i<atts->GetNumFields(); i++)
    {
        QLineEdit *le = lineEdits[i];
        QCheckBox *cb = checkBoxes[i];
        int len = atts->GetFieldLength(i);
        if (le)
        {
            if (atts->GetFieldTypeCategory(i) == CategoryString)
            {
                if (len == 1)
                    le->setText(atts->GetFieldAsString(i).c_str());
                else
                    cerr << "Error: don't yet handle arr/vec of strings\n";
            }
            else
            {
                QString r;
                for (int j=0; j<len; j++)
                {
                    if (j>0)
                        r += "  ";
                    r += QString().setNum(atts->GetFieldAsDouble(i,j));
                }
                le->setText(r);
            }
        }
        else if (cb)
        {
            if (len == 1)
                cb->setChecked(atts->GetFieldAsLong(i));
            else
                cerr << "Error: don't yet handle arr/vec of bools\n";
        }
    }

}

// ****************************************************************************
// Method:  ELAttributeControl::UpdateAttsFromWindow
//
// Purpose:
///   Update the currently watched Attribute from the state/contents
///   of the widgets.
//
// Arguments:
//   none
//
// Programmer:  Jeremy Meredith
// Creation:    August 13, 2012
//
// Modifications:
// ****************************************************************************
void
ELAttributeControl::UpdateAttsFromWindow()
{
    if (!atts || atts->GetNumFields() == 0)
        return;

    for (int i=0; i<atts->GetNumFields(); i++)
    {
        QLineEdit *le = lineEdits[i];
        QCheckBox *cb = checkBoxes[i];
        int len = atts->GetFieldLength(i);
        if (le)
        {
            if (atts->GetFieldTypeCategory(i) == CategoryString)
            {
                if (len == 1)
                    atts->SetFieldFromString(le->text().toStdString(), i);
                else
                    cerr << "Error: don't yet handle arr/vec of strings\n";
            }
            else
            {
                QStringList sl  = le->text().split(QRegExp("[, \t]"),
                                                   QString::SkipEmptyParts);
                // try to resize the field length (wors only for vectors)
                try { atts->SetFieldLength(i, sl.size()); } catch(...) { }
                len = atts->GetFieldLength(i);
                for (int j=0; j<sl.size(); j++)
                {
                    if (j < len)
                        atts->SetFieldFromDouble(sl[j].toDouble(), i, j);
                }
            }
        }
        else if (cb)
        {
            if (len == 1)
                atts->SetFieldFromLong(cb->isChecked(), i);
            else
                cerr << "Error: don't yet handle arr/vec of bools\n";
        }
    }

    // debug
    //cout << "\n\n -- after update, the new settings are: --\n\n";
    //atts->XMLSerialize(cout);
    //cout << endl;

    // They might have e.g. put chars into a numerical field, or added
    // values to a non-resizable field; we don't want to allow that.
    UpdateWindowFromAtts();

    emit settingsChanged(atts);
}

