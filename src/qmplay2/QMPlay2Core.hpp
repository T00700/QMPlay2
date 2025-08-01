/*
    QMPlay2 is a video and audio player.
    Copyright (C) 2010-2025  Błażej Szczygieł

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published
    by the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <QMPlay2Lib.hpp>
#include <PlaylistEntry.hpp>

#include <QAtomicInt>
#include <QObject>
#include <QMutex>
#include <QIcon>
#include <QHash>
#include <QMap>

#include <functional>
#include <memory>

enum LogFlags {InfoLog = 0x1, ErrorLog = 0x2, SaveLog = 0x4, AddTimeToLog = 0x8, DontShowInGUI = 0x10, LogOnce = 0x20};

template<typename T>
class QPointer;

class GPUInstance;
class QWheelEvent;
class CommonJS;
class QTranslator;
class Settings;
class QWidget;
class QPixmap;
class Module;

class QMPLAY2SHAREDLIB_EXPORT QMPlay2CoreClass : public QObject
{
    Q_OBJECT

    using ProcessWheelEventFn = std::function<void(QWheelEvent *)>;

public:
    enum class Renderer
    {
        Legacy,
        OpenGL,
        Vulkan,
    };

public:
    static inline QMPlay2CoreClass &instance()
    {
        return *qmplay2Core;
    }

#ifdef Q_OS_UNIX
    static QString getLibDir();
#endif

    static QString getLongFromShortLanguage(const QString &lng);

    static int getCPUFlags();
    static int getCPUMaxAlign();

#ifdef USE_OPENGL
    static bool isGlOnWindowForced();
#endif

    void init(bool loadModulesAndGpu, bool modulesInSubdirs, const QString &libPath, const QString &sharePath, const QString &profileName);
    void quit();

    bool canSuspend();
    void suspend();

    QStringList getModules(const QString &type, int typeLen) const;

    inline QVector<Module *> getPluginsInstance() const
    {
        return pluginsInstance;
    }

    inline QString getSettingsDir() const
    {
        return settingsDir;
    }
    inline QString getSettingsProfile() const
    {
        return settingsProfile;
    }
    inline QString getShareDir() const
    {
        return shareDir;
    }

    inline QIcon &getQMPlay2Icon() const
    {
        return *qmplay2Icon;
    }
    inline Settings &getSettings() const
    {
        return *settings;
    }
    inline Settings &getUrlPosSets() const
    {
        return *m_urlPosSets;
    }

    qreal getVideoDevicePixelRatio() const;

    QIcon getIconFromTheme(const QString &iconName, const QIcon &fallback = QIcon());

    Q_SIGNAL void wallpaperChanged(bool hasWallpaper, double alpha);
    Q_SIGNAL void wallpaperChanged(const QPixmap &wallpaper);

    Q_SIGNAL void dockVideo(QWidget *w);

    Q_SIGNAL void showSettings(const QString &moduleName);

    Q_SIGNAL void videoDockMoved();
    Q_SIGNAL void videoDockVisible(bool);

    Q_SIGNAL void statusBarMessage(const QString &txt, int ms);
    Q_SIGNAL void sendMessage(const QString &msg, const QString &title = QString(), int messageIcon = 1, int ms = 2000);
    Q_SIGNAL void processParam(const QString &param, const QString &data = QString());

    Q_SIGNAL void restoreCursor();
    Q_SIGNAL void waitCursor();
    Q_SIGNAL void busyCursor();

    Q_SIGNAL void updatePlaying(bool play, const QString &title, const QString &artist, const QString &album, int length, bool needCover, const QString &fileName, const QString &lyrics = QString());
    Q_SIGNAL void updateCover(const QString &title, const QString &artist, const QString &album, const QByteArray &cover);
    Q_SIGNAL void coverDataFromMediaFile(const QByteArray &cover);
    Q_SIGNAL void coverFile(const QString &filePath);
    Q_SIGNAL void updateInformationPanel();

    Q_SIGNAL void playStateChanged(const QString &playState);
    Q_SIGNAL void fullScreenChanged(bool fs);
    Q_SIGNAL void speedChanged(double speed);
    Q_SIGNAL void volumeChanged(double vol);
    Q_SIGNAL void posChanged(int pos);
    Q_SIGNAL void seeked(int pos);

    void log(const QString &, int logFlags = ErrorLog);
    inline void logInfo(const QString &txt, const bool showInGUI = true, const bool logOnce = false)
    {
        log(txt, InfoLog | SaveLog | AddTimeToLog | (showInGUI ? 0 : DontShowInGUI) | (logOnce ? LogOnce : 0));
    }
    inline void logError(const QString &txt, const bool showInGUI = true, const bool logOnce = false)
    {
        log(txt, ErrorLog | SaveLog | AddTimeToLog | (showInGUI ? 0 : DontShowInGUI) | (logOnce ? LogOnce : 0));
    }
    inline QString getLogFilePath()
    {
        return logFilePath;
    }

    inline void setWorking(bool w)
    {
        w ? working.ref() : working.deref();
    }
    inline bool isWorking()
    {
        return working > 0;
    }

    QStringList getLanguages() const;
    void setLanguage();

    inline QMap<QString, QString> getLanguagesMap() const
    {
        return languages;
    }

    inline QString getLanguage() const
    {
        return lang;
    }

    virtual QWidget *getVideoDock() const = 0;
    virtual QWidget *getMainWindow() const = 0;

    void addVideoDeintMethod(QWidget *w); //Needed properties: "text", "module"
    QList<QWidget *> getVideoDeintMethods() const;

    void addCookies(const QString &url, const QByteArray &newCookies, const bool removeAfterUse = true);
    QByteArray getCookies(const QString &url);

    void addResource(const QString &url, const QByteArray &data);
    void modResource(const QString &url, const bool removeAfterUse);
    bool hasResource(const QString &url) const;
    QByteArray getResource(const QString &url);

    void addRawHeaders(const QString &url, const QByteArray &data, const bool removeAfterUse = true);
    QByteArray getRawHeaders(const QString &url);

    void addNameForUrl(const QString &url, const QString &name, const bool removeAfterUse = true);
    QString getNameForUrl(const QString &url);

    void addDescriptionForUrl(const QString &url, const QString &description, const bool removeAfterUse = true);
    QString getDescriptionForUrl(const QString &url);

    QString writePlaylistResource(const QString &name, const QString &args, const PlaylistEntries &playlistEntries);

    QString rendererName() const;
    Renderer renderer() const;
    inline bool isVulkanRenderer() const
    {
        return (renderer() == Renderer::Vulkan);
    }

    inline std::shared_ptr<GPUInstance> gpuInstance() const
    {
        return m_gpuInstance;
    }

    bool isGlOnWindow() const;

    inline CommonJS *getCommonJS() const
    {
        return m_commonJS;
    }

    void registerProcessWheelEventFn(const ProcessWheelEventFn &fn);
    void processWheelEvent(QWheelEvent *e);

private slots:
    void restoreCursorSlot();
    void waitCursorSlot();
    void busyCursorSlot();

protected:
    QMPlay2CoreClass();
    virtual ~QMPlay2CoreClass();

    QIcon *qmplay2Icon;
    Settings *settings;
    Settings *m_urlPosSets = nullptr;

private:
    static QMPlay2CoreClass *qmplay2Core;

    QVector<Module *> pluginsInstance;
    QTranslator *translator, *qtTranslator;
    QString shareDir, langDir, settingsDir, logFilePath, settingsProfile;
    QAtomicInt working;
    QStringList logs;
    QMap<QString, QString> languages;
    QList<QPointer<QWidget>> videoFilters;
    QString lang;

    struct
    {
        mutable QMutex mutex;
        QHash<QString, QPair<QByteArray, bool>> data;
    } cookies, resources, rawHeaders, namesForUrl, descriptionsForUrl;

    std::shared_ptr<GPUInstance> m_gpuInstance;

    CommonJS *m_commonJS = nullptr;

    int m_suspend = 0;

    ProcessWheelEventFn m_processWheelEventFn;
};

#define QMPlay2Core QMPlay2CoreClass::instance()
