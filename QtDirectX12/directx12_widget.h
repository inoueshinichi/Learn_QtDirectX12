/**
 * @file directx12_widget.h
 * @author your name (you@domain.com)
 * @brief DirectX12 on Qt のテスト
 * @version 0.1
 * @date 2021-08-31
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#pragma once

// Windows関連
#include<tchar.h>


#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h> // 1_6: SwapChain4, 1_4: SwapChain3
#include <D3Dcompiler.h>

#include "d3dx12.h" // Microsoftオリジナルのヘルパー

#include "IsCommon/format_string.hpp"
#include "IsCommon/win32/win32_console.hpp"

#include <QWidget>
#include <QTimer>

#include <stdexcept>
#include <memory>

class DirectX12Widget : public QWidget
{
    Q_OBJECT

    std::shared_ptr<is::common::win32::Win32Console> m_pConsole;

public:
    DirectX12Widget(QWidget* parent);
    virtual ~DirectX12Widget();

    void release();
    void resetEnvironment();

    void run();
    void pauseFrames();
    void continueFrames();

private:
    bool init();
    void create3DDevice();
    void getHardwareAdapter(IDXGIFactory2* pFactory, IDXGIAdapter1** ppAdapter);
    void resizeSwapChain(int width, int height);
    void cleanupRenderTarget();
    void createRenderTarget();

    void beginScene();
    void endScene();

    void tick();
    void render();

    void waitForGpu();
    void moveToNextFrame();

    // Windows Procedure
    LRESULT WINAPI WndProc(MSG* pMsg);

public:
    ID3D12Device* device() const { return m_pDevice; }
    IDXGISwapChain* swapChain() { return m_pSwapChain; }
    ID3D12GraphicsCommandList* commandList() { return m_pCommandList; }

    void setRenderActive(bool isActive) { m_bRenderActive = isActive; }
    D3DCOLORVALUE* BackColor() { return &m_BackColor; }
  
protected:
    // Qt Event
    bool event(QEvent* event) override;
    void showEvent(QShowEvent* event) override;
    QPaintEngine* paintEngine() const override;
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;  

    // https://doc.qt.io/qt-5/qwidget.html#nativeEvent
    bool nativeEvent(const QByteArray& eventType, void* message, long* result) override;

private:
    // Pipeline objects.
    static constexpr int FRAME_COUNT = 3;
    UINT m_iCurrentFrameIndex;

    ID3D12Device* m_pDevice;
    IDXGIFactory4* m_pFactory;
    IDXGISwapChain4* m_pSwapChain;
    ID3D12CommandQueue* m_pCommandQueue;
    ID3D12CommandAllocator* m_pCommandAllocators[FRAME_COUNT];
    ID3D12GraphicsCommandList* m_pCommandList;

    // Render Target View
    ID3D12DescriptorHeap* m_pRTVDescHeap;
    UINT m_iRTVDescSize;
    ID3D12Resource* m_pRTVResources[FRAME_COUNT];
    D3D12_CPU_DESCRIPTOR_HANDLE m_RTVDescriptors[FRAME_COUNT];
    ID3D12DescriptorHeap* m_pSrvDescHeap;

    // Synchronization Object (VSYNC:垂直同期 for Display)
    HANDLE m_hSwapChainEvent;
    HANDLE m_hFenceEvent;
    ID3D12Fence* m_pFence;
    UINT64 m_iFenceValues[FRAME_COUNT];

    // Widget Object
    QTimer m_timer;
    HWND m_hWnd;

    bool m_bDeviceInitialized;
    bool m_bRenderActive;
    bool m_bStarted;
    D3DCOLORVALUE m_BackColor;


signals:
    void initialized(bool isSuccess);
    void eventHandled();
    void widgetResized();
    void ticked();
    void rendered(ID3D12GraphicsCommandList* pCommandList);
    void keyPressed(QKeyEvent* event);
    void mouseMoved(QMouseEvent* event);
    void mousePressed(QMouseEvent* event);
    void mouseReleased(QMouseEvent* event);

private slots:
    void onFrame();
    void onReset();

};


#define RELEASE_OBJECT(object)                                \
    if ((object) != nullptr) {                                \
        object->Release();                                    \
        object = nullptr;                                     \
    }

#define RELEASE_HANDLE(handle)                                \
    if ((handle) != nullptr) {                                \
        CloseHandle(handle);                                  \
        handle = NULL;                                        \
    }


inline std::string get_hr_error_string(HRESULT hr) {
    
    int size = std::snprintf(nullptr, 0, "HRESULT of 0x%08X", static_cast<UINT>(hr));
    if (size < 0) {
        std::printf("Fatal error: std::snprintf() in get_hr_error_string.\n");
        std::abort();
    }
    /* バッファサイズを算出(文字列長 + null文字サイズ) */
    size_t buf_size = size + sizeof(char);

    /* バッファサイズ分メモリ確保 */
    std::vector<char> buf(buf_size);

    /* 文字列のフォーマット */
    std::snprintf(buf.data(), buf_size, "HRESULT of 0x%08X", static_cast<UINT>(hr));
    
    /* 文字列をstd::string型に変換して出力 */
    return std::string(buf.data(), buf.data() + size);
}


///////////////////////////////////////////////////////////////////////
// HRESULT HANDLE Exception クラス
///////////////////////////////////////////////////////////////////////
#include <string>
#include <iostream>
#include <sstream>

class HrException : public std::exception
{
protected:
    std::string full_msg_; // 表示されるFullメッセージ
    std::string msg_;      // エラーメッセージ
    std::string func_;     // エラーが発生した関数名
    std::string file_;     // エラーが発生したファイル名
    int line_;        // エラーが発生した行番号

    HRESULT m_hResult;

public:
    HrException(HRESULT hr, const std::string &msg, const std::string &func, 
                const std::string &file, int line)
        : m_hResult(hr), msg_(msg), func_(func), file_(file), line_(line)
        {
            std::ostringstream ss;
            ss << get_hr_error_string(m_hResult) << " in "
               << func_ << std::endl
               << file_ << ":" << line_ << std::endl
               << msg_ << std::endl;
            full_msg_ = ss.str();
        }

    virtual ~HrException() throw() {}

    virtual const char *what() const throw() {
        return full_msg_.c_str();
    }

    HHANDLE error() const { return m_hResult; }
};


#define HR_ERROR(hr, msg, ...)                                                    \
    throw HrException(hr, is::common::format_string(msg, ##__VA_ARGS__),          \
        __func__, __FILE__, __LINE__);

#define HR_CHECK(hr, msg, ...)                                                    \
    if (FAILED(hr)) {                                                             \
        HR_ERROR(hr, std::string("Failed `" #hr "`: ") + msg, ##__VA_ARGS__)      \
    }

#define HR_FORCE_ASSERT(hr, msg, ...)                                             \
    if (FAILED(hr)) {                                                             \
        std::cerr << "Aborting: " << is::common::format_string(msg, ##__VA_ARGS__)\
                  << " at " << __func__                                           \
                  << " in " << __FILE__                                           \
                  << ":" << __LINE__                                              \
                  << std::endl;                                                   \
        ::abort();                                                                \
    }


#define DX_CALL(func) HR_CHECK(func, #func)
