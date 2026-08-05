#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QTimer>
#include <QWebEngineView>
#include <QVBoxLayout>  //new
#include <QThread>

#include <QSplitter>
#include <QResizeEvent>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
   //手动控制
    void on_publishButton_clicked();    //连接
//    void on_forwardButton_clicked();   // 前
    void on_forwardButton_pressed();
    void on_forwardButton_released();
//    void on_backwardButton_clicked();  // 后
    void on_backwardButton_pressed();
    void on_backwardButton_released();
//    void on_leftButton_clicked();      // 左
    void on_leftButton_pressed();
    void on_leftButton_released();
//    void on_rightButton_clicked();     // 右
    void on_rightButton_pressed();
    void on_rightButton_released();
    void on_stopButton_clicked();      // 停止
    void on_liftButton_clicked();      // 升降

    //自动控制
    void on_autoPrepareButton_clicked();    // 自动准备
    void on_singlePointNavButton_clicked(); // 单点导航
    void on_grabButton_clicked();           // 抓取

    //急停
    void on_emergencyStopButton_clicked();

    // 控制模式切换按钮
    void on_manualControlButton_clicked();//手动控制
    void on_autoControlButton_clicked();//自动控制

    //地图显示
    void on_showMapButton_clicked();
    //视频显示
    void on_showVideoButton_clicked();

private:
    Ui::MainWindow *ui;
    QProcess *process; // 用于执行命令
    QProcess *tunnelProcess = nullptr; // 服务器本地 SSH 隧道 new

    QWebEngineView *mapView;//用于地图显示
    QWebEngineView *videoView;//用于显示视频
    QVBoxLayout *mapLayout = nullptr;//new
    QVBoxLayout *videoLayout = nullptr;//new

    bool tunnelConnected = false;//new
    bool mapStarted = false;//new
    bool videoStarted = false;//new

    QString robotUser = "<ROBOT_USER>";
    QString robotPassword = "<ROBOT_PASSWORD>";   // 改成你的机器人密码//new
    int reverseSshPort = 2345;            // 服务器本地回连机器人的端口//new
    int controlSshPort = 2345;   // 旧控制链路端口（手动/自动控制命令用）//new

//    QTimer *manualTimer;     // 循环发布定时器
//    QString manualCmd;       // 当前手动命令

//    QTimer *forwardTimer;
//    QTimer *backwardTimer;
//    QTimer *leftTimer;
//    QTimer *rightTimer;

    //优先级控制（3个模式切换）
    enum ControlMode {
        Manual = 0,
        Auto = 1,
        EmergencyStop = 2
    };

    ControlMode currentMode;  // 当前控制模式
    bool isAutoPrepared;  // 用于标记自动准备是否完成

    QProcess *manualProcess;               // 手动模式唯一进程
    QList<QProcess*> autoProcesses;        // 自动模式允许多个进程
    bool isManualMode;                     // 当前是否处于手动模式

    void runManualCommand(const QString &cmd);//手动公用函数
    void runAutoCommand(const QString &cmd);//自动公用函数
//    void runCommand(const QString &cmd);  // 公用函数：执行命令（手动控制：前后左右升降停止；自动控制：准备，导航，抓取；急停）
    void changeMode(ControlMode newMode);//
    void updateControlButtons();//

    void appendLog(const QString &text);//new
    QString buildRobotSshCommand(const QString &remoteCmd) const;//new
    QString buildControlSshCommand(const QString &remoteCmd) const;//new
    bool startTunnel();//new
    void stopTunnel();//new
    void ensureMapView();//new
    void ensureVideoView();//new
    bool probeLocalPort(int port);//new
    bool probeUrl(const QString &url);//new
    QSplitter *mainSplitter = nullptr;   // 左右伸缩：leftPanel / rightPanel
    QSplitter *leftSplitter = nullptr;   // 上下伸缩：mapContainer / videoContainer
    QSplitter *outerSplitter = nullptr;    // topBar / 主体区
    QSplitter *contentSplitter = nullptr;  // leftPanel / rightPanel

    QWidget *contentArea = nullptr;        // 下半部分容器
    //拉伸
    void setupResizableLayout();
    void updateSplitterGeometry();


protected:
    void resizeEvent(QResizeEvent *event) override;

};

#endif // MAINWINDOW_H
