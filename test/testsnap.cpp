#include "testsnap.h"

#ifdef TEST

#include <stdexcept>
#include <cstdlib>          // for rand() and RAND_MAX def

#include <QtTest/QtTest>
#include <QDir>
#include "snap.h"
#include "trickdatamodel.h"
#include "utils.h"

class TestSnap: public QObject
{
    Q_OBJECT

public:
    TestSnap();

private:
    QString _testdir;

    bool _is_generate_rundata;

    TrickDataModel _log_frame;
    TrickDataModel _log_trickjobs;
    TrickDataModel _log_userjobs;

    QString _run(int ii); //  return test_dir/RUN_ii

    bool _write_logs(// writes log_frame/userjobs/trickjobs.trk/header
            const QString& rundir);

    void _create_log_userjobs(TrickDataModel &model,
                              double start, double stop, double freq, int njobs);
    void _create_log_frame(TrickDataModel &model,
                              double start, double stop, double freq);
    void _create_log_trickjobs(TrickDataModel &model,
                               double start, double stop, double freq);
    void _add_param(TrickDataModel &model,
                    int col,
                    const QString &pname,
                    const QString &type,
                    const QString& unit);

private slots:
    void initTestCase();
    void empty_run1();
    void empty_run2();
    void is_realtime0();
    void is_realtime1();
    void rundir();
    void range1();
    void range2();
    void range3();
    void range4();
    void range5();
    void range6();
    void range7();
    void range8();
    void range9();
    void range10();
    void range11();
    void range12();
    void range13();
    void range14();
    void range15();
    void num_jobs1();
    void num_jobs2();
    void num_frames1();
    void num_frames2();
    void num_overruns1();
    void num_overruns2();
    void num_overruns3();
    void num_overruns4();
    void num_overruns5();
    void num_overruns6();
    void num_overruns7();
    void frame_rate1();
    void frame_rate2();
    void frame_avg1();
    void frame_avg2();
    void frame_stddev1();
    void cleanupTestCase() {}
};

TestSnap::TestSnap()
{
}

QString TestSnap::_run(int ii)
{
    QString rundir = _testdir + QString("/RUN_%1").arg(ii);
    return rundir;
}

bool TestSnap::_write_logs(const QString &rundir)
{
    if (! _is_generate_rundata ) {
        return false;
    }

    _log_frame.write_log_header("log_frame",rundir);
    _log_frame.write_binary_trk("log_frame",rundir);

    _log_userjobs.write_log_header("log_userjobs",rundir);
    _log_userjobs.write_binary_trk("log_userjobs",rundir);

    _log_trickjobs.write_log_header("log_trickjobs",rundir);
    _log_trickjobs.write_binary_trk("log_trickjobs",rundir);

    return true;
}

void TestSnap::_create_log_userjobs(TrickDataModel &model,
                                    double start,
                                    double stop,
                                    double freq,
                                    int njobs)
{

    model.insertColumns(0,1);
    model.setHeaderData(
        0,Qt::Horizontal,
        QVariant(QString("sys.exec.out.time")),
        TrickDataModel::TrickRoleParamName);

    model.insertColumns(1,njobs);
    for ( int ii = 1; ii < njobs+1; ++ii) {
        QString jobname = QString("JOB_s1.x.a_C1.2500.%1(scheduled_1.000)")
                          .arg(ii);
        model.setHeaderData(
                    ii,Qt::Horizontal,
                    QVariant(jobname), TrickDataModel::TrickRoleParamName);
        model.setHeaderData(
                    ii,Qt::Horizontal,QVariant(QString("s")),
                    TrickDataModel::TrickRoleUnit);
        model.setHeaderData(
                    ii,Qt::Horizontal,QVariant(QString("double")),
                    TrickDataModel::TrickRoleType);
    }

    // Timestamp,sin(time)
    double tt = start;
    QModelIndex idx[njobs];
    int rc = 0 ;
    while ( tt <= stop+1.0e-9 ) {
        model.insertRows(rc,1);
        for ( int ii = 0; ii < njobs+1; ++ii) {
            idx[ii] = model.index(rc,ii);
            if ( ii == 0 ) {
                model.setData(idx[ii],QVariant(tt));
            } else {
                model.setData(idx[ii],QVariant(sin(tt)));
            }
        }
        tt += freq;
        rc++;
    }
}

void TestSnap::_create_log_frame(
                                    TrickDataModel &model,
                                    double start,
                                    double stop,
                                    double freq)
{
    _add_param(model,
               0,
               QString("sys.exec.out.time"),
               QString("double"),
               QString("s"));

    _add_param(model,
               1,
               QString("real_time.rt_sync.frame_sched_time"),
               QString("double"),
               QString("--"));

    _add_param(model,
               2,
               QString("real_time.rt_sync.frame_overrun_time"),
               QString("double"),
               QString("--"));

    _add_param(model,
               3,
               QString("JOB_data_record_group.userjobs.89.01(end_of_frame)"),
               QString("double"),
               QString("s"));

    _add_param(model,
               4,
               QString("JOB_data_record_group.trickjobs.90.01(end_of_frame)"),
               QString("double"),
               QString("s"));


    // Timestamp+4params
    const int nparams = 5;
    double tt = start;
    QModelIndex idx[nparams];
    int rc = 0 ;
    while ( tt <= stop+1.0e-9 ) {
        model.insertRows(rc,1);
        for ( int ii = 0; ii < nparams; ++ii) {
            idx[ii] = model.index(rc,ii);
            if ( ii == 0 ) {
                model.setData(idx[ii],QVariant(tt));
            } else {
                model.setData(idx[ii],QVariant(sin(tt)));
            }
        }
        tt += freq;
        rc++;
    }
}

void TestSnap::_create_log_trickjobs(TrickDataModel &model,
                                     double start, double stop, double freq)
{
    _add_param(model,
               0,
               QString("sys.exec.out.time"),
               QString("double"),
               QString("s"));


    _add_param(model,
               1,
               QString("JOB_trick_sys.sched.advance_sim_time.23.18(atime_1.000)"),
               QString("double"),
               QString("s"));

    _add_param(model,
               2,
               QString("JOB_trick_ip.ip.checkpoint.27.04(checkpoint)"),
               QString("double"),
               QString("s"));


    _add_param(model,
               3,
               QString("JOB_data_record_group.userjobs.89.01(end_of_frame)"),
               QString("double"),
               QString("s"));

    _add_param(model,
               4,
               QString("JOB_data_record.drd.signal_thread.29.03(end_of_frame)"),
               QString("double"),
               QString("s"));

    _add_param(model,
               5,
               QString("JOB_trick_ip.ip.process_event.27.02(automatic_1.000)"),
               QString("double"),
               QString("s"));

    _add_param(model,
               6,
               QString("JOB_real_time.rt_sync.rt_monitor.30.10(end_of_frame)"),
               QString("double"),
               QString("s"));

    // Timestamp+5params
    const int nparams = 6;
    double tt = start;
    QModelIndex idx[nparams];
    int rc = 0 ;
    while ( tt <= stop+1.0e-9 ) {
        model.insertRows(rc,1);
        for ( int ii = 0; ii < nparams; ++ii) {
            idx[ii] = model.index(rc,ii);
            switch (ii)
            {
            case 0:  model.setData(idx[ii],QVariant(tt)); break;
            default: model.setData(idx[ii],QVariant(1.0e-5*sin(tt))); break;
            };
        }
        tt += freq;
        rc++;
    }
}


void TestSnap::_add_param(TrickDataModel &model, int col,
                          const QString &pname, const QString &type,
                          const QString &unit)
{
    model.insertColumns(col,1);
    model.setHeaderData(
            col,Qt::Horizontal,
                QVariant(pname), TrickDataModel::TrickRoleParamName);
    model.setHeaderData(
            col,Qt::Horizontal,QVariant(unit), TrickDataModel::TrickRoleUnit);
    model.setHeaderData(
            col,Qt::Horizontal,QVariant(type), TrickDataModel::TrickRoleType);
}

void TestSnap::initTestCase()
{
    _is_generate_rundata = true;   // turn this to false once happy with
                                   // RUN data that is generated by this test

    _testdir = QDir::currentPath() + "/test";


    _create_log_frame(_log_frame,0,100,0.1);
    _create_log_trickjobs(_log_trickjobs,0,100,0.1);
    _create_log_userjobs(_log_userjobs,0,100,0.1,5);
}

void TestSnap::empty_run1()
{
    QString rundir = _testdir + QString("/RUN_empty1");
    try {
        Snap snap(rundir);
    } catch (std::invalid_argument &e) {
        return;         // Should throw this exception
    }

    QCOMPARE(false,true);  // if exception fails this fails
}

void TestSnap::empty_run2()
{
    QString rundir = _testdir + QString("/RUN_empty2");
    try {
        Snap snap(rundir);
    } catch (std::invalid_argument &e) {
        return;         // Should throw this exception
    }

    QCOMPARE(false,true);  // if exception fails this fails
}

// Test is_realtime() when run is non realtime
void TestSnap::is_realtime0()
{
    QString rundir = _run(0);

    for ( int ii = 0; ii < _log_frame.rowCount(); ++ii) {
        for ( int jj = 1; jj < _log_frame.columnCount(); ++jj) {
            QModelIndex idx = _log_frame.index(ii,jj);
            _log_frame.setData(idx,QVariant(0.0));
        }
    }
    _write_logs(rundir);

    Snap snap(rundir);
    bool expected = snap.is_realtime();
    QCOMPARE(expected,false);
}

// Test is_realtime() when run is realtime
void TestSnap::is_realtime1()
{
    QString rundir = _run(1);

    // col=1 if any point is non-zero for this param it's realtime
    QModelIndex idx = _log_frame.index(500,1);
    _log_frame.setData(idx,QVariant(100.0));

    // Set an overrun time too
    idx = _log_frame.index(500,2);
    _log_frame.setData(idx,QVariant(200.0));

    _write_logs(rundir);

    Snap snap(rundir);
    bool expected = snap.is_realtime();
    QCOMPARE(expected,true);
}

void TestSnap::rundir()
{
    QString rundir = _run(1);
    Snap snap(rundir);
    QCOMPARE(snap.rundir(),rundir);
}

void TestSnap::range1()
{
    Snap snap(_run(1));         // default 1.0
    QCOMPARE(snap.start(),1.0);
}

void TestSnap::range2()
{
    Snap snap(_run(1));
    QCOMPARE(snap.start(),1.0);
}

void TestSnap::range3()
{
    try {
        Snap snap(_run(1),101.0);  // exceed timestamp again
    } catch (std::range_error &e) {
        return;         // Should throw this exception
    }

    QCOMPARE(false,true);  // if exception fails this fails
}

void TestSnap::range4()
{
    Snap snap(_run(1),0.0);  // zero-th timestamp
    QCOMPARE(snap.start(),0.0);
}

void TestSnap::range5()
{
    Snap snap(_run(1),100.0);  // last timestamp
    QCOMPARE(snap.start(),100.0);
}

void TestSnap::range6()
{
    Snap snap(_run(1),-100.0);  // negative timestamp
    QCOMPARE(snap.start(),0.0);
}

void TestSnap::range7()
{
    Snap snap(_run(1),-100.0,200.0);  // big stop
    QCOMPARE(snap.start(),0.0);
    QCOMPARE(snap.stop(),100.0);
}

void TestSnap::range8()
{
    Snap snap(_run(1),10.0,100.0);  // last stop
    QCOMPARE(snap.start(),10.0);
    QCOMPARE(snap.stop(),100.0);
}

void TestSnap::range9()
{
    Snap snap(_run(1),49.0,50.0);  // mid stop
    QCOMPARE(snap.start(),49.0);
    QCOMPARE(snap.stop(),50.0);
}

void TestSnap::range10()
{
    Snap snap(_run(1),49.0,49.0);  // start/stop on same point
    QCOMPARE(snap.start(),49.0);
    QCOMPARE(snap.stop(),49.0);
    QCOMPARE(snap.num_frames(),1);
}

void TestSnap::range11()
{
    Snap snap(_run(1),40.001,40.001);  // start/stop on same point off cycle
    QCOMPARE(snap.start(),40.1);
    QCOMPARE(snap.stop(),40.1);
    QCOMPARE(snap.num_frames(),1);
}

void TestSnap::range12()
{
    Snap snap(_run(1),39.999,39.999);  // start/stop on same point off cycle
    QCOMPARE(snap.start(),40.0);
    QCOMPARE(snap.stop(),40.0);
    QCOMPARE(snap.num_frames(),1);
}

void TestSnap::range13()
{
    Snap snap(_run(1),99.999,99.999);  // start/stop on same point off cycle
                                      // on end bound point
    QCOMPARE(snap.start(),100.0);
    QCOMPARE(snap.stop(),100.0);
    QCOMPARE(snap.num_frames(),1);
}

void TestSnap::range14()
{
    try {
        Snap snap(_run(1),40.0,30.00);  // stop > start
    } catch (std::range_error &e) {
        return;         // Should throw this exception
    }

    QCOMPARE(false,true);  // if exception fails this fails
}

void TestSnap::range15()
{
    Snap snap(_run(1),100.0,100.0001);  // start/stop on same point off cycle
                                    // on end bound point
    QCOMPARE(snap.start(),100.0);
    QCOMPARE(snap.stop(),100.0);
    QCOMPARE(snap.num_frames(),1);
}

void TestSnap::num_jobs1()
{
    Snap snap(_run(0));
    QCOMPARE(snap.num_jobs(),11);
}

void TestSnap::num_jobs2()
{
    Snap snap(_run(1));
    QCOMPARE(snap.num_jobs(),11);
}

void TestSnap::num_frames1()
{
    Snap snap(_run(1),0,100);
    QCOMPARE(snap.num_frames(),10*100+1);
}

void TestSnap::num_frames2()
{
    Snap snap(_run(1),50);
    QCOMPARE(snap.num_frames(),10*50+1);
}

void TestSnap::num_overruns1()
{
    Snap snap(_run(1));
    QCOMPARE(snap.num_overruns(),1);
}

void TestSnap::num_overruns2()
{
    Snap snap(_run(1),0,50);
    QCOMPARE(snap.num_overruns(),1);
}

void TestSnap::num_overruns3()
{
    Snap snap(_run(1),0,49.1);
    QCOMPARE(snap.num_overruns(),0);
}

void TestSnap::num_overruns4()
{
    Snap snap(_run(1),50);
    QCOMPARE(snap.num_overruns(),1);
}

void TestSnap::num_overruns5()
{
    Snap snap(_run(1),50.1,53);
    QCOMPARE(snap.num_overruns(),0);
}

void TestSnap::num_overruns6()
{
    Snap snap(_run(0));
    QCOMPARE(snap.num_overruns(),0);
}

void TestSnap::num_overruns7()
{
    Snap snap(_run(1),49.99,49.99);
    QCOMPARE(snap.num_overruns(),1);
}

void TestSnap::frame_rate1()
{
    Snap snap(_run(1));
    QCOMPARE(snap.frame_rate(),QString("0.1"));
}

void TestSnap::frame_rate2()
{
    QString rundir = _run(2);

    _log_frame.insertRows(1,1);
    QModelIndex idx = _log_frame.index(1,0);
    _log_frame.setData(idx,QVariant(0.05));
    _write_logs(rundir);

    Snap snap(rundir,0);
    QCOMPARE(snap.num_frames(),10*100+1+1); // extra frame at 0.05
    QCOMPARE(snap.frame_rate(),QString("0.05,0.1"));
}

void TestSnap::frame_avg1()
{
    QString rundir = _run(3);

    _log_frame.removeRows(1,1);

    int cnt = 0 ;
    double sum = 0 ;
    double rt = 700000.0; // num microseconds as a double
    for ( int ii = 0; ii < _log_frame.rowCount(); ++ii) {
        if ( ii%10 == 0 ) {
            QModelIndex idx = _log_frame.index(ii,1);
            _log_frame.setData(idx,QVariant(rt));
            sum += rt;
        }
        cnt++;
    }
    double avg = sum/(double)cnt/1000000.0;
    _write_logs(rundir);

    Snap snap(rundir,0);

    // sanity to see if extra row added in frame_rate2() is gone
    QCOMPARE(snap.num_frames(),10*100+1);
    QCOMPARE(snap.frame_rate(),QString("0.1"));

    QCOMPARE(snap.frame_avg(),avg);
}

void TestSnap::frame_avg2()
{
    QString rundir = _run(4);

    int cnt = 0 ;
    double sum = 0 ;
    double rt; // rand num microseconds as a double
    for ( int ii = 0; ii < _log_frame.rowCount(); ++ii) {
        QModelIndex idx = _log_frame.index(ii,1);
        rt = 1000000.0*(qrand()/(double)RAND_MAX);
        _log_frame.setData(idx,QVariant(rt));
        sum += rt;
        cnt++;
    }
    double avg = sum/(double)cnt/1000000.0;
    _write_logs(rundir);

    Snap snap(rundir,0);

    QCOMPARE(snap.frame_avg(),avg);
}

void TestSnap::frame_stddev1()
{
    QString rundir = _run(4);

    double sum = 0.0 ;
    double rt;
    for ( int ii = 0; ii < _log_frame.rowCount(); ++ii) {
        QModelIndex idx = _log_frame.index(ii,1);
        rt = _log_frame.data(idx).toDouble();
        sum += rt;
    }
    double cnt = (double)_log_frame.rowCount();
    double avg = sum/cnt;

    // Take the square root of the avg
    // of the squared differences of the values
    // from their avg value
    double sum_sqr_diffs = 0.0;
    for ( int ii = 0; ii < _log_frame.rowCount(); ++ii) {
        QModelIndex idx = _log_frame.index(ii,1);
        rt = _log_frame.data(idx).toDouble();
        sum_sqr_diffs += (avg-rt)*(avg-rt);
    }
    double stddev = qSqrt(sum_sqr_diffs/cnt)/1000000.0;

    Snap snap(rundir,0);

    QCOMPARE(snap.frame_stddev(),stddev);
}

QTEST_APPLESS_MAIN(TestSnap)
#include "testsnap.moc"

#endif
