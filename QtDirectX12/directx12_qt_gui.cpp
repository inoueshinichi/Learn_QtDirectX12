#include "directx12_main_window.h"

#include <QApplication>

#include <iostream>

int main(int argc, char** argv)
{
    std::cout << "Hello, DirectX12!\n"; 

    QApplication app(argc, argv);

    DirectX12MainWindow main_win;
    main_win.show();

    return app.exec();
}