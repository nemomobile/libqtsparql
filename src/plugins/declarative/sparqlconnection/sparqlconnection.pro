include(../declarativebase.pri)

TARGET	 = sparqlconnection
equals(QT_MAJOR_VERSION, 4): SOURCES		= plugin.cpp
equals(QT_MAJOR_VERSION, 5): SOURCES		= plugin-qt5.cpp
