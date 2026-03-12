#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QProcess>//终端库
#include <QDebug>//按键库

#include <QWebEngineView>
#include <QVBoxLayout>

#include <QUrl>//new
#include <QTimer>//new
#include <QSplitter>//new
#include <QHBoxLayout>//new

#include <QSplitter>//new
#include <QResizeEvent>//new

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    process(new QProcess(this)),
    currentMode(EmergencyStop),  // 默认模式为急停
    isAutoPrepared(false)  // 默认未完成自动准备
{
    ui->setupUi(this);

    //
    // 让 topBar 里的四个按钮真正跟随 topBar 等比例横向伸缩
    QHBoxLayout *topLayout = new QHBoxLayout(ui->topBar);
    topLayout->setContentsMargins(0, 0, 0, 0);
    topLayout->setSpacing(8);

    // 把按钮从原来的 horizontalLayoutWidget 脱离出来
    ui->showMapButton->setParent(nullptr);
    ui->showVideoButton->setParent(nullptr);
    ui->publishButton->setParent(nullptr);
    ui->emergencyStopButton->setParent(nullptr);

    // 按钮横向可扩展，纵向固定
    ui->showMapButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    ui->showVideoButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    ui->publishButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    ui->emergencyStopButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    // 加进新的布局
    topLayout->addWidget(ui->showMapButton, 1);
    topLayout->addWidget(ui->showVideoButton, 1);
    topLayout->addWidget(ui->publishButton, 1);
    topLayout->addWidget(ui->emergencyStopButton, 1);

    // 原来那层固定大小的承载控件隐藏掉
    ui->horizontalLayoutWidget->hide();
    //

    updateControlButtons();  // 初始化按钮状态
    this->setWindowTitle("远程桌面控制系统");

    // 先不加载地图，等点击按钮再显示
    mapView = nullptr;
    videoView = nullptr;

    // 连接按钮点击事件，点击时启动终端并执行 ls 命令  重复了
    //connect(ui->publishButton, &QPushButton::clicked, this, &MainWindow::on_publishButton_clicked);

//    manualTimer = new QTimer(this);
//    manualTimer->setInterval(100);  // 每100ms发布一次命令

//    // 定时器触发时发布当前手动命令
//    connect(manualTimer, &QTimer::timeout, this, [=]() {
//        if (currentMode == Manual && !manualCmd.isEmpty()) {
//            runManualCommand(manualCmd);
//        }
//    });
//    forwardTimer = new QTimer(this);
//    connect(forwardTimer, &QTimer::timeout, this, [=]() {
//        runManualCommand("sshpass -p 'shihe321' ssh -t -p 2347 rpp@localhost \"bash -ic 'cd ~/rpp_tools && ./forward.sh'\"");
//    });

//    backwardTimer = new QTimer(this);
//    connect(backwardTimer, &QTimer::timeout, this, [=]() {
//        runManualCommand("sshpass -p 'shihe321' ssh -t -p 2347 rpp@localhost \"bash -ic 'cd ~/rpp_tools && ./backward.sh'\"");
//    });

//    leftTimer = new QTimer(this);
//    connect(leftTimer, &QTimer::timeout, this, [=]() {
//        runManualCommand("sshpass -p 'shihe321' ssh -t -p 2347 rpp@localhost \"bash -ic 'cd ~/rpp_tools && ./turn_left.sh'\"");
//    });

//    rightTimer = new QTimer(this);
//    connect(rightTimer, &QTimer::timeout, this, [=]() {
//        runManualCommand("sshpass -p 'shihe321' ssh -t -p 2347 rpp@localhost \"bash -ic 'cd ~/rpp_tools && ./turn_right.sh'\"");
//    });
//    MainWindow::setFixedSize(950, 650);
    //全局 QSS 主题(美化)
    this->setStyleSheet(R"(

    /* 主背景 */
    QMainWindow {
        background-color: #13151c;
        color: #ffffff;
        font-family: "Microsoft YaHei";
    }

    /* ===== 顶部控制栏 ===== */
    QFrame#topBar {
        background-color: #1c1f2b;
        border-bottom: 2px solid #2f3345;
    }

    /* 通用按钮 */
    QPushButton {
        background-color: #2c3144;
        color: #dfe6ff;
        border-radius: 6px;
        padding: 6px 6px;
        font-size: 12px;
    }

    QPushButton:hover {
        background-color: #3a3f57;
    }

    QPushButton:pressed {
        background-color: #4f5dff;
    }

    /* 急停按钮 */
    QPushButton#emergencyStopButton {
        background-color: #d32f2f;
        border-radius: 8px;
        font-weight: bold;
        font-size: 15px;
        padding: 6px 18px;
    }

    QPushButton#emergencyStopButton:hover {
        background-color: #ff3b3b;
    }

    QPushButton#emergencyStopButton:pressed {
        background-color: #9a0007;
    }

    /* 连接按钮 */
    QPushButton#publishButton {
        background-color: #00695c;
    }

    QPushButton#publishButton:hover {
        background-color: #009688;
    }

    /* 分区面板 */
    QFrame {
        background-color: #1b1e2a;
        border-radius: 10px;
    }

    /* 输入框 */
    QPlainTextEdit {
        background-color: #25293a;
        border: 1px solid #3a3f57;
        border-radius: 6px;
        padding: 6px;
        color: #ffffff;
        font-size: 14px;
    }
    QPlainTextEdit:focus {
        border: 1px solid #4f5dff;
    }

    /* 标签 */
    QLabel {
        color: #cfd8ff;
        font-size: 13px;
    }

    /* 分组标题 */
    QGroupBox {
        border: 1px solid #3a3f57;
        border-radius: 8px;
        margin-top: 10px;
    }

    QGroupBox:title {
        subcontrol-origin: margin;
        subcontrol-position: top left;
        left: 15px;
        padding: 0 8px;
        color: #9fa8ff;
        font-weight: bold;
    }

    /* ===== 手动控制分区 ===== */
    QGroupBox#manualGroup {
        border: 2px solid #3a3f57;
        border-radius: 10px;
        margin-top: 15px;
        padding-top: 15px;
    }

    /* ===== 自动控制分区 ===== */
    QGroupBox#autoGroup {
        border: 2px solid #2f6df6;
        border-radius: 10px;
        margin-top: 15px;
        padding-top: 15px;
    }

//QWidget#mapContainer {
//    border: 2px solid #3a3f57;
//}

//QWidget#videoContainer {
//    border: 2px solid #3a3f57;
//}

    )");
    ui->publishButton->setText("连接");//new
    ui->showMapButton->setText("启动地图");//new
    ui->showVideoButton->setText("启动视频");//new

    setupResizableLayout();

}

MainWindow::~MainWindow()
{
    stopTunnel();
    delete ui;
}

void MainWindow::on_publishButton_clicked()//new
{
    if (!tunnelConnected) {
        if (startTunnel()) {
            ui->publishButton->setText("断开");
            appendLog("服务器到机器人连接成功");

            bool ok8080 = probeLocalPort(8080);
            bool ok8081 = probeLocalPort(8081);
            bool ok9090 = probeLocalPort(9090);

            appendLog(QString("端口检查：8080=%1 8081=%2 9090=%3")
                      .arg(ok8080 ? "OK" : "FAIL")
                      .arg(ok8081 ? "OK" : "FAIL")
                      .arg(ok9090 ? "OK" : "FAIL"));
        } else {
            appendLog("连接失败：请先确认机器人到服务器的反向 SSH 已建立");
        }
    } else {
        if (mapView) {
            mapView->setUrl(QUrl("about:blank"));
        }
        if (videoView) {
            videoView->setUrl(QUrl("about:blank"));
        }

        stopTunnel();

        mapStarted = false;
        videoStarted = false;

        ui->publishButton->setText("连接");
        ui->showMapButton->setText("启动地图");
        ui->showVideoButton->setText("启动视频");

        appendLog("连接已断开");
    }
}
// 按钮点击时打开终端并运行 ls 命令 连接
//void MainWindow::on_publishButton_clicked()
//{
//    QString targetDir = "/home";

//    // 拼接命令：进入目录 -> 执行命令 -> 保留终端
//    QString fullCommand = "cd " + targetDir + " && " + "sshpass -p 'shihe321' ssh -t -p 2347 rpp@localhost" + "; exec bash";

//    QProcess *process = new QProcess(this);
//    process->start("gnome-terminal", QStringList() << "--" << "bash" << "-c" << fullCommand);

//    ui->logView->appendPlainText("连接");


//}

// 公用函数：在 /home下执行命令
//void MainWindow::runCommand(const QString &cmd)
//{
//    QString targetDir = "/home";

//    // 拼接命令：进入目录 -> 执行命令 -> 保留终端
//    QString fullCommand = "cd " + targetDir + " && " + cmd + "; exec bash";

//    QProcess *process = new QProcess(this);
//    process->start("gnome-terminal", QStringList() << "--" << "bash" << "-c" << fullCommand);

//}

//void MainWindow::runCommand(const QString &cmd)
//{
//    QString targetDir = "/home";
//    QString fullCommand = "cd " + targetDir + " && " + cmd;

//    // 如果之前的进程还在运行，先杀掉
//    if (process && process->state() == QProcess::Running) {
//        process->kill();
//        process->waitForFinished();
//    }

//    process = new QProcess(this);

//    // 绑定输出到 logView
//    connect(process, &QProcess::readyReadStandardOutput, [=]() {
//        ui->logView->appendPlainText(QString::fromLocal8Bit(process->readAllStandardOutput()));
//    });
//    connect(process, &QProcess::readyReadStandardError, [=]() {
//        ui->logView->appendPlainText(QString::fromLocal8Bit(process->readAllStandardError()));
//    });

//    // 启动命令
//    process->start("bash", QStringList() << "-c" << fullCommand);
//}
// 手动模式：每次只能有一个进程
void MainWindow::runManualCommand(const QString &cmd)
{
    QString targetDir = "/home";
    QString fullCommand = "cd " + targetDir + " && " + cmd;

    // 如果之前的进程还在运行，先杀掉
    if (process && process->state() == QProcess::Running) {
        process->kill();
        process->waitForFinished();
        ui->logView->appendPlainText("上一组进程已终止！");
    }

    process = new QProcess(this);

    // 绑定输出到 logView
    connect(process, &QProcess::readyReadStandardOutput, [=]() {
        ui->logView->appendPlainText(QString::fromLocal8Bit(process->readAllStandardOutput()));
    });
    connect(process, &QProcess::readyReadStandardError, [=]() {
        ui->logView->appendPlainText(QString::fromLocal8Bit(process->readAllStandardError()));
    });

    // 启动命令
    process->start("bash", QStringList() << "-c" << fullCommand);
}

// 自动模式：允许多个进程同时运行
void MainWindow::runAutoCommand(const QString &cmd)
{
    QString targetDir = "/home";
    QString fullCommand = "cd " + targetDir + " && " + cmd;

    QProcess *autoProcess = new QProcess(this);

    // 绑定输出到 logView
    connect(autoProcess, &QProcess::readyReadStandardOutput, [=]() {
        ui->logView->appendPlainText(QString::fromLocal8Bit(autoProcess->readAllStandardOutput()));
    });
    connect(autoProcess, &QProcess::readyReadStandardError, [=]() {
        ui->logView->appendPlainText(QString::fromLocal8Bit(autoProcess->readAllStandardError()));
    });

    // 进程结束时自动清理
    connect(autoProcess,
            static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
            [=](int, QProcess::ExitStatus){
                autoProcesses.removeOne(autoProcess);
                autoProcess->deleteLater();
            });



    autoProcesses.append(autoProcess); // 加入自动进程列表
    autoProcess->start("bash", QStringList() << "-c" << fullCommand);
}





// 前进 forwardButton
//void MainWindow::on_forwardButton_clicked()
//{
////    runCommand("ros2 topic pub /motor_lift/set_position std_msgs/msg/Float32 \"data: 0.1\" --once");//w

////    // 在 logView 中追加日志
////    ui->logView->appendPlainText("前进");
//    if (currentMode == Manual) {
//            runManualCommand("ros2 topic pub /cmd_vel geometry_msgs/msg/Twist \"{linear: {x: 0.1}, angular: {z: 0.0}}\" --once");
//            ui->logView->appendPlainText("前进");
//        }

//}

void MainWindow::on_forwardButton_pressed()
{
    if (currentMode == Manual) {
        // 按下时发布前进命令（一次性发布）
//        runManualCommand("ros2 topic pub /cmd_vel geometry_msgs/msg/Twist \"{linear: {x: 0.1}, angular: {z: 0.0}}\" --once");
//        runManualCommand("ls");
        runManualCommand(buildControlSshCommand("cd ~/rpp_tools && ./forward.sh"));
//        forwardTimer->start(200); // 每 100ms 发送一次前进
        ui->logView->appendPlainText("前进中...");
    }
}


void MainWindow::on_forwardButton_released()
{
    if (currentMode == Manual) {
        // 松开时发布停止命令（发布零速度）
//        runManualCommand("ros2 topic pub /cmd_vel geometry_msgs/msg/Twist \"{linear: {x: 0.0}, angular: {z: 0.0}}\" --once");
//        runManualCommand("ls");
//        forwardTimer->stop();
        runManualCommand(buildControlSshCommand("cd ~/rpp_tools && ./stop.sh"));
        ui->logView->appendPlainText("停止前进");
    }
}



// 后退 backwardButton
//void MainWindow::on_backwardButton_clicked()
//{
////    runCommand("echo x");
////    ui->logView->appendPlainText("后退");
//    if (currentMode == Manual) {
//            runManualCommand("ls");
//            ui->logView->appendPlainText("后退");
//        }
//}

void MainWindow::on_backwardButton_pressed()
{
    if (currentMode == Manual) {
        // 按下时发布前进命令（一次性发布）
//        runManualCommand("ros2 topic pub /cmd_vel geometry_msgs/msg/Twist \"{linear: {x: 0.1}, angular: {z: 0.0}}\" --once");
//        runManualCommand("ls");
        runManualCommand(buildControlSshCommand("cd ~/rpp_tools && ./backward.sh"));
//        backwardTimer->start(100);
        ui->logView->appendPlainText("后退中...");
    }
}


void MainWindow::on_backwardButton_released()
{
    if (currentMode == Manual) {
        // 松开时发布停止命令（发布零速度）
//        runManualCommand("ros2 topic pub /cmd_vel geometry_msgs/msg/Twist \"{linear: {x: 0.0}, angular: {z: 0.0}}\" --once");
//        runManualCommand("ls");
//        backwardTimer->stop();
        runManualCommand(buildControlSshCommand("cd ~/rpp_tools && ./stop.sh"));
        ui->logView->appendPlainText("停止后退");
    }
}

// 左转 leftButton
//void MainWindow::on_leftButton_clicked()
//{
//    if (currentMode == Manual) {
//            runManualCommand("ls");
//            ui->logView->appendPlainText("左转");
//        }
//}

void MainWindow::on_leftButton_pressed()
{
    if (currentMode == Manual) {
        // 按下时发布前进命令（一次性发布）
//        runManualCommand("ros2 topic pub /cmd_vel geometry_msgs/msg/Twist \"{linear: {x: 0.1}, angular: {z: 0.0}}\" --once");
//        runManualCommand("ls");
        runManualCommand(buildControlSshCommand("cd ~/rpp_tools && ./turn_left.sh"));
//        leftTimer->start(100);
        ui->logView->appendPlainText("左转中...");
    }
}


void MainWindow::on_leftButton_released()
{
    if (currentMode == Manual) {
        // 松开时发布停止命令（发布零速度）
//        runManualCommand("ros2 topic pub /cmd_vel geometry_msgs/msg/Twist \"{linear: {x: 0.0}, angular: {z: 0.0}}\" --once");
//        runManualCommand("ls");
//        leftTimer->stop();
        runManualCommand(buildControlSshCommand("cd ~/rpp_tools && ./stop.sh"));
        ui->logView->appendPlainText("停止左转");
    }
}

// 右转 rightButton
//void MainWindow::on_rightButton_clicked()
//{
//    if (currentMode == Manual) {
//            runManualCommand("ls");
//            ui->logView->appendPlainText("右转");
//        }
//}

void MainWindow::on_rightButton_pressed()
{
    if (currentMode == Manual) {
        // 按下时发布前进命令（一次性发布）
//        runManualCommand("ros2 topic pub /cmd_vel geometry_msgs/msg/Twist \"{linear: {x: 0.1}, angular: {z: 0.0}}\" --once");
//        runManualCommand("ls");
        runManualCommand(buildControlSshCommand("cd ~/rpp_tools && ./turn_right.sh"));
//        rightTimer->start(200);
        ui->logView->appendPlainText("右转中...");
    }
}


void MainWindow::on_rightButton_released()
{
    if (currentMode == Manual) {
        // 松开时发布停止命令（发布零速度）
//        runManualCommand("ros2 topic pub /cmd_vel geometry_msgs/msg/Twist \"{linear: {x: 0.0}, angular: {z: 0.0}}\" --once");
//        runManualCommand("ls");
//        rightTimer->stop();
        runManualCommand(buildControlSshCommand("cd ~/rpp_tools && ./stop.sh"));
        ui->logView->appendPlainText("停止右转");
    }
}

// 停止 stopButton
void MainWindow::on_stopButton_clicked()
{
    if (currentMode == Manual) {
            runManualCommand(buildControlSshCommand("cd ~/rpp_tools && ./stop.sh"));
            ui->logView->appendPlainText("停止");
        }
}

// 升降//改echo j        liftButton
void MainWindow::on_liftButton_clicked()
{
    if (currentMode == Manual) {
    // 从输入框获取参数
    QString param = ui->liftParamEdit->text();

    if (param.isEmpty()) {
        ui->logView->appendPlainText("升降：未输入参数！");
        return;
    }

    // 转换为浮点数
    bool ok;
    double value = param.toDouble(&ok);

    if (!ok) {
        ui->logView->appendPlainText("升降：请输入数字参数！");
        return;
    }

    // 判断范围
    if (value < 0.0 || value > 0.3) {
        ui->logView->appendPlainText("升降：参数要在 0-0.3 之间！");
        return;
    }

    // 拼接命令（这里只是 echo 示例，你换成实际命令）
    QString valueStr = QString::number(value);  // 将 double 转换为 QString

    QString cmd = buildRobotSshCommand("./up_down.sh " + valueStr);


//    QString cmd = "sshpass -p 'shihe321' ssh -t -p 2347 rpp@localhost \"bash -ic 'cd ~/rpp_tools && ./start_all.sh'\"";

//    QString cmd = "gnome-terminal --title=\"工控机控制台\" -- bash -c \"sshpass -p 'shihe321' ssh -t -p 2347 rpp@localhost 'bash -ic \\\"gnome-terminal -- bash -c \\\"ros2 launch motor_lift_ros motor_lift_ros.launch.py & sleep 2; ros2 topic pub /motor_lift/set_position std_msgs/msg/Float32 \\\\\\\"data: " + valueStr + " \\\\\\\" --once; exec bash\\\"\\\"'\"";
    runManualCommand(cmd);

    // 在日志中显示
    ui->logView->appendPlainText("升降：" + QString::number(value) + "米");
    }
}


//状态切换函数
void MainWindow::changeMode(ControlMode newMode)
{
    // 如果当前是急停模式，且想切换到其他模式
    if (currentMode == EmergencyStop && newMode != EmergencyStop) {
        currentMode = newMode;  // 切换到新模式
        updateControlButtons();  // 更新按钮状态

        // 输出当前切换的模式到 logView
        if (newMode == Auto) {
            ui->logView->appendPlainText("切换到自动控制模式");
        } else if (newMode == Manual) {
            ui->logView->appendPlainText("切换到手动控制模式");

            QTimer::singleShot(1000, this, [=]() {
                runAutoCommand(buildControlSshCommand("cd ~/rpp_tools && ./start_robot_driver.sh"));
                ui->logView->appendPlainText("手动控制已准备");

            });


        }

    } else if (currentMode != EmergencyStop) {
        // 非急停模式下才能切换，且当前模式与目标模式不同
        currentMode = newMode;
        updateControlButtons();

        // 输出当前切换的模式到 logView
        if (newMode == Auto) {
            ui->logView->appendPlainText("切换到自动控制模式");
        } else if (newMode == Manual) {
            ui->logView->appendPlainText("切换到手动控制模式");
        }
    }
}



//更新控制按钮的可用性
void MainWindow::updateControlButtons()
{
    // 根据当前模式更新按钮的可用状态
    bool manualEnabled = (currentMode == Manual || currentMode == EmergencyStop);
    bool autoEnabled = (currentMode == Auto || currentMode == EmergencyStop);

    ui->forwardButton->setEnabled(manualEnabled);
    ui->backwardButton->setEnabled(manualEnabled);
    ui->leftButton->setEnabled(manualEnabled);
    ui->rightButton->setEnabled(manualEnabled);
    ui->stopButton->setEnabled(manualEnabled);
    ui->liftButton->setEnabled(manualEnabled);

    ui->autoPrepareButton->setEnabled(autoEnabled);
    ui->singlePointNavButton->setEnabled(autoEnabled);
    ui->grabButton->setEnabled(autoEnabled);

    ui->emergencyStopButton->setEnabled(true);  // 急停按钮始终可用

    // 只有在急停模式下才能启用控制模式切换按钮
    ui->manualControlButton->setEnabled(currentMode == EmergencyStop);
    ui->autoControlButton->setEnabled(currentMode == EmergencyStop);
}

void MainWindow::appendLog(const QString &text)//new
{
    ui->logView->appendPlainText(text);
}

QString MainWindow::buildRobotSshCommand(const QString &remoteCmd) const//new
{
    return QString(
        "sshpass -p '%1' ssh "
        "-o StrictHostKeyChecking=no "
        "-p %2 %3@localhost "
        "\"bash -lc 'cd ~/rpp_tools && %4'\""
    ).arg(robotPassword)
     .arg(reverseSshPort)
     .arg(robotUser)
     .arg(remoteCmd);
}

QString MainWindow::buildControlSshCommand(const QString &remoteCmd) const//new
{
    return QString(
        "sshpass -p '%1' ssh "
        "-o StrictHostKeyChecking=no "
        "-t -p %2 %3@localhost "
        "\"bash -lc '%4'\""
    ).arg(robotPassword)
     .arg(controlSshPort)
     .arg(robotUser)
     .arg(remoteCmd);
}

bool MainWindow::startTunnel()//new
{
    // 已存在隧道则直接复用
    if (probeLocalPort(8080) && probeLocalPort(8081) && probeLocalPort(9090)) {
        tunnelConnected = true;
        appendLog("检测到现有 SSH 隧道，直接复用 8080 / 8081 / 9090");
        return true;
    }

    if (tunnelProcess) {
        tunnelProcess->deleteLater();
        tunnelProcess = nullptr;
    }

    tunnelProcess = new QProcess(this);

    QString cmd = QString(
        "sshpass -p '%1' ssh "
        "-o StrictHostKeyChecking=no "
        "-o ExitOnForwardFailure=yes "
        "-N -p %2 "
        "-L 8080:127.0.0.1:8080 "
        "-L 8081:127.0.0.1:8081 "
        "-L 9090:127.0.0.1:9090 "
        "%3@localhost"
    ).arg(robotPassword)
     .arg(reverseSshPort)
     .arg(robotUser);

    connect(tunnelProcess, &QProcess::readyReadStandardOutput, this, [this]() {
        appendLog(QString::fromLocal8Bit(tunnelProcess->readAllStandardOutput()));
    });

    connect(tunnelProcess, &QProcess::readyReadStandardError, this, [this]() {
        appendLog(QString::fromLocal8Bit(tunnelProcess->readAllStandardError()));
    });

    tunnelProcess->start("bash", QStringList() << "-lc" << cmd);

    if (!tunnelProcess->waitForStarted(3000)) {
        appendLog("隧道进程启动失败");
        tunnelConnected = false;
        return false;
    }

    bool ok = false;
    for (int i = 0; i < 12; ++i) {
        if (probeLocalPort(8080) && probeLocalPort(8081) && probeLocalPort(9090)) {
            ok = true;
            break;
        }
        QThread::msleep(300);
    }

    if (!ok) {
        appendLog("隧道启动失败：8080/8081/9090 未监听成功");
        if (tunnelProcess->state() == QProcess::Running) {
            tunnelProcess->kill();
            tunnelProcess->waitForFinished(1000);
        }
        tunnelConnected = false;
        return false;
    }

    tunnelConnected = true;
    appendLog("SSH 隧道已建立：8080 / 8081 / 9090");
    return true;
}

void MainWindow::stopTunnel()//new
{
    if (tunnelProcess && tunnelProcess->state() == QProcess::Running) {
        tunnelProcess->kill();
        tunnelProcess->waitForFinished(1500);
    }

    QProcess cleanup;
    cleanup.start("bash", QStringList() << "-lc"
                  << "pkill -f \"ssh .* -L 8080:127.0.0.1:8080 .* -L 8081:127.0.0.1:8081 .* -L 9090:127.0.0.1:9090 .* rpp@localhost\" || true");
    cleanup.waitForFinished(3000);

    tunnelConnected = false;
    appendLog("SSH 隧道已断开");
}

void MainWindow::ensureMapView()//new
{
    if (!mapView) {
        mapView = new QWebEngineView(ui->mapContainer);
        mapLayout = new QVBoxLayout(ui->mapContainer);
        mapLayout->setContentsMargins(0, 0, 0, 0);
        mapLayout->addWidget(mapView);
    }
}

void MainWindow::ensureVideoView()//new
{
    if (!videoView) {
        videoView = new QWebEngineView(ui->videoContainer);
        videoLayout = new QVBoxLayout(ui->videoContainer);
        videoLayout->setContentsMargins(0, 0, 0, 0);
        videoLayout->addWidget(videoView);
    }
}

bool MainWindow::probeLocalPort(int port)//new
{
    QProcess proc;
    QString cmd = QString("ss -ltn | grep -q ':%1 '").arg(port);
    proc.start("bash", QStringList() << "-lc" << cmd);
    proc.waitForFinished(2000);
    return proc.exitCode() == 0;
}

bool MainWindow::probeUrl(const QString &url)//new
{
    QProcess proc;
    QString cmd = QString("curl -I -s --max-time 3 %1 >/dev/null").arg(url);
    proc.start("bash", QStringList() << "-lc" << cmd);
    proc.waitForFinished(5000);
    return proc.exitCode() == 0;
}

//手动控制按钮点击事件
void MainWindow::on_manualControlButton_clicked()
{
    changeMode(Manual);  // 切换到手动模式
}

//自动控制按钮点击事件
void MainWindow::on_autoControlButton_clicked()
{
    changeMode(Auto);  // 切换到自动模式
}

//急停按钮点击事件
void MainWindow::on_emergencyStopButton_clicked()
{
    // 急停优先级最高，强制切换到急停模式
    changeMode(EmergencyStop);
    ui->logView->appendPlainText("急停！");

    // 可以在此处执行急停的实际命令
    runManualCommand(buildControlSshCommand("cd ~/rpp_tools && ./stop_all_terminals.sh"));
}

//自动准备按钮
void MainWindow::on_autoPrepareButton_clicked()
{
    if (currentMode != Auto) {
        return;
    }

    // 执行自动准备命令
//    QString cmd = "gnome-terminal --title=\"工控机控制台\" -- bash -c \"sshpass -p 'shihe321' ssh -t -p 2347 rpp@localhost 'bash -ic \\\"gnome-terminal -- bash -c \\\\\\\"ros2 launch motor_lift_ros motor_lift_ros.launch.py & sleep 2; ros2 topic pub /motor_lift/set_position std_msgs/msg/Float32 \\\\\\\\\\\"data: 0.1\\\\\\\\\\\" --once; exec bash\\\\\\\"\\\"'\"";
    QString cmd = buildControlSshCommand("cd ~/rpp_tools && ./start_all.sh");

    // 自动准备完成，更新状态
    isAutoPrepared = true;  // 标记自动准备已完成

    // 执行自动准备命令
    if (currentMode == Auto) {
        runAutoCommand(cmd);
        ui->logView->appendPlainText("执行自动准备");
        return;
    }


    changeMode(Auto);  // 切换到自动模式
}


//单点导航按钮点击事件
void MainWindow::on_singlePointNavButton_clicked()
{
    // 检查是否处于自动模式，并且自动准备是否完成
    if (currentMode != Auto) {
        ui->logView->appendPlainText("请先执行自动准备！");
        return;
    }

    if (!isAutoPrepared) {
        ui->logView->appendPlainText("自动准备未完成，无法执行单点导航！");
        return;
    }

    bool xOk, yOk, wOk;

        // 获取用户输入
        double x = ui->xInput->text().toDouble(&xOk);
        double y = ui->yInput->text().toDouble(&yOk);
        double w = ui->wInput->text().toDouble(&wOk);

        // 参数合法性检查
        if (!xOk || !yOk || !wOk || w < 0 || w > 360) {
            ui->logView->appendPlainText("请输入正确的参数！(W 范围 0-360)");
            return;
        }

        // 把参数拼接成字符串
        QString params = QString("%1 %2 %3").arg(x).arg(y).arg(w);

        // 拼接完整命令 (这里用 ssh 远程执行示例)
        QString cmd = buildRobotSshCommand("./navigate_to_pose.sh " + params);

        // 日志显示
        ui->logView->appendPlainText("执行指令: " + cmd);

        // 执行命令
        runManualCommand(cmd);


    // 自动准备完成，可以执行单点导航
//    QString cmd = "ros2 launch robot_navigation single_point_navigation.launch.py";
//    QString cmd = "ls";

    // 执行单点导航命令
    runAutoCommand(cmd);
    ui->logView->appendPlainText("执行单点导航");
}


//抓取按钮点击事件
void MainWindow::on_grabButton_clicked()
{
    if (currentMode != Auto) {
        ui->logView->appendPlainText("请先执行自动准备！");
        return;
    }

    if (!isAutoPrepared) {
        ui->logView->appendPlainText("自动准备未完成，无法执行抓取！");
        return;
    }

    // 这里的命令根据你的实际情况来修改，假设命令是 ROS2 的抓取命令
    QString cmd = buildControlSshCommand("cd ~/rpp_tools && ./loop_scraping.sh");

//    QString cmd = "ls";

    // 执行命令
    runAutoCommand(cmd);
    ui->logView->appendPlainText("执行抓取");
}

//地图显示
void MainWindow::on_showMapButton_clicked()//new
{
    if (!tunnelConnected) {
        appendLog("请先点击“连接”建立隧道");
        return;
    }

    if (!mapStarted) {
        appendLog("正在启动地图服务...");
        runAutoCommand(buildRobotSshCommand("./start_map_service.sh"));

        ensureMapView();

        QTimer::singleShot(1800, this, [this]() {
            if (!probeUrl("http://127.0.0.1:8080/robot_map_local.html")) {
                appendLog("地图页面检查失败：http://127.0.0.1:8080/robot_map_local.html");
                return;
            }
            mapView->load(QUrl("http://127.0.0.1:8080/robot_map_local.html"));
            appendLog("地图已启动");
        });

        mapStarted = true;
        ui->showMapButton->setText("停止地图");
    } else {
        if (mapView) {
            mapView->setUrl(QUrl("about:blank"));
        }

        runAutoCommand(buildRobotSshCommand("./stop_map_runtime.sh"));
        mapStarted = false;
        ui->showMapButton->setText("启动地图");
        appendLog("地图已停止");
    }
}
//void MainWindow::on_showMapButton_clicked()
//{
//    if (!mapView) {
//        mapView = new QWebEngineView(ui->mapContainer);

//        // 设置填充 mapContainer
//        QVBoxLayout *layout = new QVBoxLayout(ui->mapContainer);
//        layout->setContentsMargins(0, 0, 0, 0);
//        layout->addWidget(mapView);

//        // 加载本地 HTML
//        mapView->load(QUrl::fromLocalFile("/home/ma/桌面/sh/robot_map_local.html"));//改
//    }
//    mapView->reload();

//}

//视频显示
void MainWindow::on_showVideoButton_clicked()//new
{
    if (!tunnelConnected) {
        appendLog("请先点击“连接”建立隧道");
        return;
    }

    if (!videoStarted) {
        appendLog("正在启动视频服务...");
        runAutoCommand(buildRobotSshCommand("./start_video_service.sh"));

        ensureVideoView();

        QTimer::singleShot(1800, this, [this]() {
            if (!probeUrl("http://127.0.0.1:8080/video.html")) {
                appendLog("视频页面检查失败：http://127.0.0.1:8080/video.html");
                return;
            }
            videoView->load(QUrl("http://127.0.0.1:8080/video.html"));
            appendLog("视频已启动");
        });

        videoStarted = true;
        ui->showVideoButton->setText("停止视频");
    } else {
        if (videoView) {
            videoView->setUrl(QUrl("about:blank"));
        }

        runAutoCommand(buildRobotSshCommand("./stop_video_service.sh"));
        videoStarted = false;
        ui->showVideoButton->setText("启动视频");
        appendLog("视频已停止");
    }
}

void MainWindow::setupResizableLayout()
{
    // ---------- 先把原控件从原父对象中拿出来 ----------
    ui->topBar->setParent(nullptr);
    ui->leftPanel->setParent(nullptr);
    ui->rightPanel->setParent(nullptr);
    ui->mapContainer->setParent(nullptr);
    ui->videoContainer->setParent(nullptr);

    // ---------- 左侧上下分割：地图 + 视频 ----------
    leftSplitter = new QSplitter(Qt::Vertical, this);
    leftSplitter->setChildrenCollapsible(false);
    leftSplitter->addWidget(ui->mapContainer);
    leftSplitter->addWidget(ui->videoContainer);
    leftSplitter->setStretchFactor(0, 6);
    leftSplitter->setStretchFactor(1, 4);

    // 把 leftSplitter 放进 leftPanel
    QVBoxLayout *leftLayout = new QVBoxLayout(ui->leftPanel);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(0);
    leftLayout->addWidget(leftSplitter);

    // ---------- 主体区左右分割：左显示区 + 右控制日志区 ----------
    contentSplitter = new QSplitter(Qt::Horizontal, this);
    contentSplitter->setChildrenCollapsible(false);

    // 右侧最小宽度，防止内容被切掉
    ui->rightPanel->setMinimumWidth(480);

    contentSplitter->addWidget(ui->leftPanel);
    contentSplitter->addWidget(ui->rightPanel);
    contentSplitter->setStretchFactor(0, 7);
    contentSplitter->setStretchFactor(1, 3);

    // 下半部分容器
    contentArea = new QWidget(this);
    QHBoxLayout *contentLayout = new QHBoxLayout(contentArea);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(0);
    contentLayout->addWidget(contentSplitter);

    // ---------- 外层上下分割：topBar + 主体区 ----------
    outerSplitter = new QSplitter(Qt::Vertical, this);
    outerSplitter->setChildrenCollapsible(false);

    // topBar 最小高度，避免按钮被压扁
    ui->topBar->setMinimumHeight(68);
    ui->topBar->setMaximumHeight(96);

    outerSplitter->addWidget(ui->topBar);
    outerSplitter->addWidget(contentArea);

    outerSplitter->setStretchFactor(0, 1);
    outerSplitter->setStretchFactor(1, 12);

    // ---------- 放回 centralwidget ----------
    QVBoxLayout *rootLayout = new QVBoxLayout(ui->centralwidget);
    rootLayout->setContentsMargins(6, 6, 6, 6);
    rootLayout->setSpacing(6);
    rootLayout->addWidget(outerSplitter);

    // 初始比例
    outerSplitter->setSizes({80, 720});
    contentSplitter->setSizes({760, 420});
    leftSplitter->setSizes({430, 290});
}

void MainWindow::updateSplitterGeometry()
{
    if (!mainSplitter || !leftSplitter) {
        return;
    }

    const int leftMargin = 10;
    const int rightMargin = 10;
    const int bottomMargin = 10;
    const int spacingBelowTopBar = 8;

    // topBar 的底部位置
    int topBarBottom = ui->topBar->geometry().y() + ui->topBar->geometry().height();

    // 主体区域从 topBar 下方开始
    int x = leftMargin;
    int y = topBarBottom + spacingBelowTopBar;
    int w = ui->centralwidget->width() - leftMargin - rightMargin;
    int h = ui->centralwidget->height() - y - bottomMargin;

    if (w < 100) w = 100;
    if (h < 100) h = 100;

    mainSplitter->setGeometry(x, y, w, h);

    // leftSplitter 放满 leftPanel
    leftSplitter->setGeometry(0, 0, ui->leftPanel->width(), ui->leftPanel->height());
}


//void MainWindow::on_showVideoButton_clicked()
//{
//    if (!videoView) {
//        videoView = new QWebEngineView(ui->videoContainer);

//        QVBoxLayout *layout = new QVBoxLayout(ui->videoContainer);
//        layout->setContentsMargins(0, 0, 0, 0);
//        layout->addWidget(videoView);

//        videoView->load(QUrl::fromLocalFile(
//            "/home/ma/桌面/sh/video.html"
//        ));
//    }
//    videoView->reload();
//}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    updateSplitterGeometry();
}
