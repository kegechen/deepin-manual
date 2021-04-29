/*
 * Copyright (C) 2017 ~ 2018 Deepin Technology Co., Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "controller/search_manager.h"
#include "controller/search_db.h"

#include <DSysInfo>
#include <QThread>

SearchManager::SearchManager(QObject *parent)
    : QObject(parent)
    , db_(nullptr)
    , db_thread_(nullptr)
{
    initSearchManager();
}

/**
 * @brief SearchManager::initSearchManager
 * 初始化数据库类对象， 绑定信号槽
 */
void SearchManager::initSearchManager()
{
    db_thread_ = new QThread(this);
    db_ = new SearchDb();
    db_thread_->start();
    db_->moveToThread(db_thread_);

    connect(this, &SearchManager::searchAnchor, db_, &SearchDb::searchAnchor);
    connect(db_, &SearchDb::searchAnchorResult, this, &SearchManager::searchAnchorResult);
    connect(this, &SearchManager::searchContent, db_, &SearchDb::searchContent);
    connect(db_, &SearchDb::searchContentResult, this, &SearchManager::searchContentResult);
    connect(db_, &SearchDb::searchContentMismatch, this, &SearchManager::searchContentMismatch);
    connect(db_thread_, &QThread::destroyed, db_, &QObject::deleteLater);

    QString strDB = DMAN_SEARCH_DB;
    if (Dtk::Core::DSysInfo::UosServer == Dtk::Core::DSysInfo::uosType()) {
        strDB += "/server/search.db";
    } else if (Dtk::Core::DSysInfo::UosHome == Dtk::Core::DSysInfo::uosEditionType()) {
        strDB += "/personal/search.db";
    } else if (Dtk::Core::DSysInfo::UosEducation == Dtk::Core::DSysInfo::uosEditionType()) {
        strDB += "/education/search.db";
    } else if (Dtk::Core::DSysInfo::UosCommunity == Dtk::Core::DSysInfo::uosEditionType()) {
        strDB += "/community/search.db";
    } else {
        strDB += "/professional/search.db";
    }

    emit db_->initDbAsync(strDB);
}

SearchManager::~SearchManager()
{
    db_thread_->quit();
    db_thread_->wait();
    delete db_thread_;
    db_thread_ = nullptr;
    delete db_;
    db_ = nullptr;
}
