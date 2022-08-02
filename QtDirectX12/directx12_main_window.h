#pragma once



#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <D3Dcompiler.h>

#include <QMainWindow>
#include <Qt>
#include <QDrag>
#include <QMessageBox>
#include <QLineEdit>
#include <QFileDialog>
#include <QDialog>
#include <QInputDialog>
#include <QColorDialog>
#include <QImage>
#include <QString>
#include <QLabel>
#include <QWidget>

QT_BEGIN_NAMESPACE
    namespace Ui { class DirectX12MainWindow; }
QT_END_NAMESPACE

class DirectX12MainWindow : public QMainWindow
{
    Q_OBJECT
    Q_DISABLE_COPY(DirectX12MainWindow)

    Ui::DirectX12MainWindow* m_pUi;

public:
    DirectX12MainWindow(QWidget* parent = nullptr);
    virtual ~DirectX12MainWindow();

protected:
    virtual void closeEvent(QCloseEvent* event) override;

private slots:
    void slotInitialize(bool isSuccess);
    void slotTick();
    void slotRender();
};