#include <QCoreApplication>
#include "chatserver.h"
#include <iostream>

int main(int argc, char *argv[])
{
    int ret;
    try {
        QCoreApplication a(argc, argv);
        ChatServer m_chatServer;
        if (!m_chatServer.listen(QHostAddress::Any, 3333)) {
            return EXIT_FAILURE;
        }
        m_chatServer.logMessage("Server started on port 3333!");
        ret = a.exec();
    } catch (const std::bad_alloc &) {
        return EXIT_FAILURE;
    }
    return ret;
}
