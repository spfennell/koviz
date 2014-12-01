#include <QApplication>
#include <QObject>
#include <QThread>

#include <string>
using namespace std;

#include <QDebug>
#include <QDir>
#include <stdio.h>

#include "libopts/options.h"
#include "libsnapdata/runs.h"
#include "libqplot/plotmainwindow.h"

QStandardItemModel* createVarsModel(MonteModel *monteModel);

Option::FPresetQStringList presetRunDirs;
Option::FPresetUInt presetBeginRun;
Option::FPresetUInt presetEndRun;

class SnapOptions : public Options
{
  public:
    QStringList runDirs;
    unsigned int beginRun;
    unsigned int endRun;
};

SnapOptions opts;


int main(int argc, char *argv[])
{

    bool ok;

    opts.add("<RUN_dir>:1", &opts.runDirs, QStringList(),
             "RUN_directory with RUNs",
             presetRunDirs);
    opts.add("-beginRun",&opts.beginRun,0,
             "begin run (inclusive) in set of RUNs to plot",
             presetBeginRun);
    opts.add("-endRun",&opts.endRun,100000,
             "end run (inclusive) in set of RUNs to plot",
             presetEndRun);
    opts.parse(argc,argv, QString("snapq"), &ok);

    if ( !ok ) {
        fprintf(stderr,"%s\n",opts.usage().toAscii().constData());
        exit(-1);
    }

    try {
        QApplication::setGraphicsSystem("raster");
        QApplication a(argc, argv);

        Runs runs(opts.runDirs);
        MonteModel monteModel(&runs);
        QStandardItemModel* varsModel = createVarsModel(&monteModel);

        PlotMainWindow w(opts.runDirs.at(0), &monteModel, varsModel);

        w.show();
        return a.exec();
    } catch (std::exception &e) {
        fprintf(stderr,"\n%s\n",e.what());
        fprintf(stderr,"%s\n",opts.usage().toAscii().constData());
        exit(-1);
    }

    return 0;
}

void presetRunDirs(QStringList* defRunDirs,
                   const QStringList& runDirs,bool* ok)
{
    Q_UNUSED(defRunDirs);

    foreach ( QString dirName, runDirs ) {
        QDir dir(dirName);
        if ( ! dir.exists() ) {
            fprintf(stderr,"snapq [error] : couldn't find directory: \"%s\".\n",
                    dirName.toAscii().constData());
            *ok = false;
            return;
        }
    }
}

void presetBeginRun(uint* beginRunId, uint runId, bool* ok)
{
    Q_UNUSED(beginRunId);

    *ok = true;

    if ( *ok ) {
        // Start time should be less than stop time
        if ( runId > opts.endRun ) {
            fprintf(stderr,"snap [error] : option -beginRun, set to %d, "
                    "must be greater than "
                    " -endRun which is set to %d\n",
                    runId, opts.endRun);
            *ok = false;
        }
    }
}

void presetEndRun(uint* endRunId, uint runId, bool* ok)
{
    Q_UNUSED(endRunId);

    *ok = true;

    if ( *ok ) {
        // Start time should be less than stop time
        if ( runId < opts.beginRun ) {
            fprintf(stderr,"snap [error] : option -endRun, set to %d, "
                    "must be greater than "
                    "-beginRun which is set to %d\n",
                    runId,opts.beginRun);
            *ok = false;
        }
    }
}

//
// List of vars from the MonteModel column headerData
//
QStandardItemModel* createVarsModel(MonteModel *monteModel)
{
    QStandardItemModel* varsModel = new QStandardItemModel(0,1);

    QStringList varList;
    for ( int c = 1; c < monteModel->columnCount(); ++c) {
        QString var = monteModel->headerData(c,Qt::Horizontal).toString();
        varList.append(var);
    }

    varList.sort();

    QStandardItem *rootItem = varsModel->invisibleRootItem();
    for ( int i = 0; i < varList.size(); ++i) {
        QStandardItem *varItem = new QStandardItem(varList.at(i));
        rootItem->appendRow(varItem);
    }

    return varsModel;
}
