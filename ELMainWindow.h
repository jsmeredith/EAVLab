// Copyright 2012-2013 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef EL_MAINWINDOW_H
#define EL_MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QGroupBox>
#include <QComboBox>
#include <QCheckBox>
#include <QSlider>
#include <QLineEdit>
#include <QSpinBox>

class eavlDataSet;
class eavlImporter;
class eavlFilter;

class ELPipelineBuilder;
class ELWindowManager;
class Pipeline;

class QSplitter;

// ****************************************************************************
// Class:  ELMainWindow
//
// Purpose:
///   Main window.
//
// Programmer:  Jeremy Meredith
// Creation:    July 30, 2012
//
// ****************************************************************************
class ELMainWindow : public QMainWindow
{
    Q_OBJECT
  public:
    explicit ELMainWindow(QWidget *parent = 0);
    ~ELMainWindow();
    void OpenFile(const QString &);

  public slots:
    void PipelineUpdated(int pipeIndex, Pipeline *pipe);
    void OpenFile();
    void Exit();
    void WindowAdded(QWidget*);
    void SettingsActivated(QWidget*);

  private:
    QSplitter *topSplitter;
    ELPipelineBuilder *pipelineBuilder;
    ELWindowManager *windowMgr;
    QGroupBox *windowSettingsGroup;
    QWidget *activeSettingsWidget;
};

#endif

