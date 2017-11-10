
TEMPLATE = app
TARGET = imgproc

CONFIG += exceptions
CONFIG += rtti
QT += widgets

SOURCES += \
	main.cpp \
	ImageScrollWidget.cpp \
	MainWindow.cpp \
	ImageScrollTab.cpp \
	ImageTab.cpp \
	RadioButtonsDialog.cpp \
	images_processor.cpp

HEADERS += \
	ImageScrollWidget.h \
	MainWindow.h \
	ImageScrollTab.h \
	ImageTab.h \
	RadioButtonsDialog.h \
	images_processor.h

