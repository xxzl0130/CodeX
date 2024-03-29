﻿#pragma once
#include <QtWidgets>
#include <QProgressBar>
#include <QLabel>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QJsonDocument>
#include <QObject>
#include <QNetworkProxy>
#include <QSslConfiguration>

#include "ChipDataWindow/ChipDataWindow.h"
#include "ChipView/ChipView.h"
#include "Chip/Chip.h"
#include "ChipSolver/ChipSolver.h"
#include "ChipTableView/ChipTableView.h"
#include "ChipTableView/SolutionTableModel.h"

constexpr auto IniShowLimit = "/Sys/ShowLimit";
constexpr auto IniCalcLimit = "/Sys/CalcLimit";
constexpr auto IniNoLimit = "/Sys/NoLimit";
constexpr auto IniThreads = "/Sys/Threads";