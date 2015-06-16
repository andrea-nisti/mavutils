#-------------------------------------------------
#
# Project created by QtCreator 2014-09-12T11:19:25
#
#-------------------------------------------------

TEMPLATE = subdirs

HEADERS = common/*  \
          mavlink/*



SUBDIRS = mavping \
          mavrcoverride \
          mavreader \
          mavping2radios \
          mocap2mav


QT = core gui

QT += widgets
      

