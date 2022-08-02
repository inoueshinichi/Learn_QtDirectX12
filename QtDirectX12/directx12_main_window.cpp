#include "directx12_main_window.h"

#include "ui_DirectX12MainWindow.h"

#include <QTime>
#include <QCoreApplication>
#include <QCloseEvent>



DirectX12MainWindow::DirectX12MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_pUi(new Ui::DirectX12MainWindow())
{
    // UI
    m_pUi->setupUi(this);

 

    connect(m_pUi->directX12Widget, &DirectX12Widget::initialized,
        this, &DirectX12MainWindow::slotInitialize);
    connect(m_pUi->directX12Widget, &DirectX12Widget::ticked,
        this, &DirectX12MainWindow::slotTick);
    connect(m_pUi->directX12Widget, &DirectX12Widget::rendered,
        this, &DirectX12MainWindow::slotRender);

}

DirectX12MainWindow::~DirectX12MainWindow()
{
    if (m_pUi) {
        delete m_pUi;
        m_pUi = nullptr;
    }
}

void DirectX12MainWindow::closeEvent(QCloseEvent *event)
{
    // Qtイベントにこれ以上イベント通知が送られて来ないようにする.
    event->ignore();

    m_pUi->directX12Widget->release();

    QTime dieTime = QTime::currentTime().addMSecs(500);
    while (QTime::currentTime() < dieTime)
		QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

    // Qtイベントの通知再開
	event->accept();
}

void DirectX12MainWindow::slotInitialize(bool isSuccess)
{
    QTimer::singleShot(500, this, [&] { m_pUi->directX12Widget->run(); });
}

void DirectX12MainWindow::slotTick()
{

}

void DirectX12MainWindow::slotRender()
{

}