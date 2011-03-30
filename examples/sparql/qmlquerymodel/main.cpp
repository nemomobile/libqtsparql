#include <qdeclarative.h>
#include <QtDeclarative/QDeclarativeView>
#include <QtDeclarative/QDeclarativeEngine>
#include <QtGui/QApplication>
#include <QtCore/QDebug>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.addLibraryPath("../../../plugins");

    QDeclarativeView view;
    view.engine()->addImportPath("../../../imports");
    qWarning() << "about to load file";
    view.setSource(QUrl::fromLocalFile("albums.qml"));
    qWarning() << "loaded file";
    QList<QDeclarativeError> errors = view.errors();
    qWarning() << "errors: " << errors;
    view.show();
    return app.exec();
}
