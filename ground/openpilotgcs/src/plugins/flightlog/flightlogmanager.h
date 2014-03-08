/**
 ******************************************************************************
 *
 * @file       flightlogmanager.h
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2012.
 * @addtogroup [Group]
 * @{
 * @addtogroup FlightLogManager
 * @{
 * @brief [Brief]
 *****************************************************************************/
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef FLIGHTLOGMANAGER_H
#define FLIGHTLOGMANAGER_H

#include <QObject>
#include <QList>
#include <QQmlListProperty>
#include <QSemaphore>
#include <QXmlStreamWriter>
#include <QTextStream>

#include "uavobjectmanager.h"
#include "debuglogentry.h"
#include "debuglogstatus.h"
#include "debuglogcontrol.h"

class UAVOLogSettingsWrapper : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(int setting READ setting WRITE setSetting NOTIFY settingChanged)

public:
    enum UAVLogSetting {DISABLED = 0, ON_CHANGE, EVERY_10MS, EVERY_50MS, EVERY_100MS,
                  EVERY_500MS, EVERY_1S, EVERY_5S, EVERY_10S, EVERY_30S, EVERY_1M};

    explicit UAVOLogSettingsWrapper();
    explicit UAVOLogSettingsWrapper(UAVObject* object);
    ~UAVOLogSettingsWrapper();

    QString name() const
    {
        return m_object->getName();
    }

    int setting() const
    {
        return m_setting;
    }

public slots:
    void setSetting(int setting)
    {
        if (m_setting != (int)setting) {
            m_setting = (int)setting;
            emit settingChanged((int)setting);
        }
    }

signals:
    void settingChanged(int setting);
    void nameChanged();

private:
    UAVObject *m_object;
    int m_setting;
};

class ExtendedDebugLogEntry : public DebugLogEntry {
    Q_OBJECT
    Q_PROPERTY(QString LogString READ getLogString WRITE setLogString NOTIFY LogStringUpdated)

public:
    explicit ExtendedDebugLogEntry();
    ~ExtendedDebugLogEntry();

    QString getLogString();
    void toXML(QXmlStreamWriter *xmlWriter, quint32 baseTime);
    void toCSV(QTextStream *csvStream, quint32 baseTime);
    UAVDataObject *uavObject()
    {
        return m_object;
    }

    void setData(const DataFields & data, UAVObjectManager *objectManager);

public slots:
    void setLogString(QString arg)
    {
        Q_UNUSED(arg);
    }

signals:
    void LogStringUpdated(QString arg);

private:
    UAVDataObject *m_object;
};

class FlightLogManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(DebugLogStatus *flightLogStatus READ flightLogStatus)
    Q_PROPERTY(QQmlListProperty<ExtendedDebugLogEntry> logEntries READ logEntries NOTIFY logEntriesChanged)
    Q_PROPERTY(QStringList flightEntries READ flightEntries NOTIFY flightEntriesChanged)
    Q_PROPERTY(bool disableControls READ disableControls WRITE setDisableControls NOTIFY disableControlsChanged)
    Q_PROPERTY(bool disableExport READ disableExport WRITE setDisableExport NOTIFY disableExportChanged)
    Q_PROPERTY(bool adjustExportedTimestamps READ adjustExportedTimestamps WRITE setAdjustExportedTimestamps NOTIFY adjustExportedTimestampsChanged)

    Q_PROPERTY(QQmlListProperty<UAVOLogSettingsWrapper> uavoEntries READ uavoEntries NOTIFY uavoEntriesChanged)
    Q_PROPERTY(QStringList logSettings READ logSettings NOTIFY logSettingsChanged)


public:
    explicit FlightLogManager(QObject *parent = 0);
    ~FlightLogManager();

    QQmlListProperty<ExtendedDebugLogEntry> logEntries();
    QQmlListProperty<UAVOLogSettingsWrapper> uavoEntries();

    QStringList flightEntries();

    QStringList logSettings() {
        return m_logSettings;
    }

    DebugLogStatus *flightLogStatus() const
    {
        return m_flightLogStatus;
    }

    bool disableControls() const
    {
        return m_disableControls;
    }

    bool disableExport() const
    {
        return m_disableExport;
    }

    void clearLogList();

    bool adjustExportedTimestamps() const
    {
        return m_adjustExportedTimestamps;
    }

signals:
    void logEntriesChanged();
    void flightEntriesChanged();
    void logSettingsChanged();

    void uavoEntriesChanged();

    void disableControlsChanged(bool arg);
    void disableExportChanged(bool arg);

    void adjustExportedTimestampsChanged(bool arg);

public slots:
    void clearAllLogs();
    void retrieveLogs(int flightToRetrieve = -1);
    void exportLogs();
    void cancelExportLogs();

    void setDisableControls(bool arg)
    {
        if (m_disableControls != arg) {
            m_disableControls = arg;
            emit disableControlsChanged(arg);
        }
    }

    void setDisableExport(bool arg)
    {
        if (m_disableExport != arg) {
            m_disableExport = arg;
            emit disableExportChanged(arg);
        }
    }

    void setAdjustExportedTimestamps(bool arg)
    {
        if (m_adjustExportedTimestamps != arg) {
            m_adjustExportedTimestamps = arg;
            emit adjustExportedTimestampsChanged(arg);
        }
    }

private slots:
    void updateFlightEntries(quint16 currentFlight);
    void setupUAVOWrappers();
    void setupLogSettings();

private:
    UAVObjectManager *m_objectManager;
    DebugLogControl *m_flightLogControl;
    DebugLogStatus *m_flightLogStatus;
    DebugLogEntry *m_flightLogEntry;

    QList<ExtendedDebugLogEntry *> m_logEntries;
    QStringList m_flightEntries;
    QStringList m_logSettings;

    QList<UAVOLogSettingsWrapper *> m_uavoEntries;

    static const int UAVTALK_TIMEOUT = 4000;
    bool m_disableControls;
    bool m_disableExport;
    bool m_cancelDownload;
    bool m_adjustExportedTimestamps;

    void exportToOPL(QString fileName);
    void exportToCSV(QString fileName);
    void exportToXML(QString fileName);

};

#endif // FLIGHTLOGMANAGER_H
