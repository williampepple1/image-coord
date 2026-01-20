#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QToolBar>
#include <QDockWidget>
#include <QListWidget>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QActionGroup>
#include <QGroupBox>

#include "ImageMapEditor.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openImage();
    void saveProject();
    void loadProject();
    void exportHtml();

    void onToolSelect();
    void onToolRect();
    void onToolCircle();
    void onToolPolygon();

    void onHotspotAdded(HotspotItem *hotspot);
    void onHotspotRemoved(HotspotItem *hotspot);
    void onHotspotSelected(HotspotItem *hotspot);
    void onHotspotListSelectionChanged();

    void onDeleteHotspot();
    void onClearAllHotspots();

    void updateHotspotProperties();
    void updateCodePreview();

    void onCoordinatesChanged(const QPointF &pos);
    void onCoordinatesCopied(const QPointF &pos);
    void onClipboardModeToggled(bool checked);
    void copyHtmlToClipboard();

private:
    void setupMenuBar();
    void setupToolBar();
    void setupDockWidgets();
    void setupStatusBar();
    void updateHotspotList();
    void setCurrentTool(EditorTool tool);

    ImageMapEditor *m_editor;

    // Toolbar actions
    QAction *m_selectAction;
    QAction *m_rectAction;
    QAction *m_circleAction;
    QAction *m_polygonAction;
    QAction *m_clipboardModeAction;
    QActionGroup *m_toolGroup;

    // Dock widgets
    QDockWidget *m_propertiesDock;
    QDockWidget *m_hotspotsDock;
    QDockWidget *m_codeDock;

    // Hotspots list
    QListWidget *m_hotspotsList;

    // Properties panel
    QLineEdit *m_urlEdit;
    QLineEdit *m_altEdit;
    QLineEdit *m_titleEdit;
    QLabel *m_coordsLabel;
    QLabel *m_shapeLabel;

    // Code preview
    QTextEdit *m_codePreview;
    QLineEdit *m_mapNameEdit;

    // Status bar
    QLabel *m_coordsStatusLabel;
    QLabel *m_imageInfoLabel;
};

#endif // MAINWINDOW_H
