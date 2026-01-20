#include "MainWindow.h"
#include <QMenuBar>
#include <QStatusBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QClipboard>
#include <QApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QStyle>
#include <QScrollArea>
#include <QSplitter>
#include <QIcon>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Image Map Generator");
    setWindowIcon(QIcon(":/icons/icons/app.svg"));
    setMinimumSize(1200, 800);

    // Apply dark theme
    QString styleSheet = R"(
        QMainWindow {
            background-color: #1e1e2e;
        }
        QMenuBar {
            background-color: #181825;
            color: #cdd6f4;
            padding: 4px;
        }
        QMenuBar::item:selected {
            background-color: #313244;
        }
        QMenu {
            background-color: #181825;
            color: #cdd6f4;
            border: 1px solid #45475a;
        }
        QMenu::item:selected {
            background-color: #45475a;
        }
        QToolBar {
            background-color: #181825;
            border: none;
            spacing: 4px;
            padding: 4px;
        }
        QToolButton {
            background-color: transparent;
            color: #cdd6f4;
            border: none;
            border-radius: 4px;
            padding: 8px 12px;
            font-weight: bold;
        }
        QToolButton:hover {
            background-color: #313244;
        }
        QToolButton:checked {
            background-color: #89b4fa;
            color: #1e1e2e;
        }
        QDockWidget {
            color: #cdd6f4;
            font-weight: bold;
        }
        QDockWidget::title {
            background-color: #181825;
            padding: 8px;
            text-align: left;
        }
        QWidget#dockContent {
            background-color: #1e1e2e;
        }
        QListWidget {
            background-color: #181825;
            color: #cdd6f4;
            border: 1px solid #313244;
            border-radius: 6px;
        }
        QListWidget::item {
            padding: 8px;
            border-bottom: 1px solid #313244;
        }
        QListWidget::item:selected {
            background-color: #45475a;
        }
        QListWidget::item:hover {
            background-color: #313244;
        }
        QLineEdit {
            background-color: #181825;
            color: #cdd6f4;
            border: 1px solid #45475a;
            border-radius: 6px;
            padding: 8px;
        }
        QLineEdit:focus {
            border-color: #89b4fa;
        }
        QTextEdit {
            background-color: #181825;
            color: #a6e3a1;
            border: 1px solid #313244;
            border-radius: 6px;
            font-family: 'Consolas', 'Monaco', monospace;
            font-size: 12px;
        }
        QPushButton {
            background-color: #89b4fa;
            color: #1e1e2e;
            border: none;
            border-radius: 6px;
            padding: 10px 20px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #b4befe;
        }
        QPushButton:pressed {
            background-color: #74c7ec;
        }
        QPushButton#dangerButton {
            background-color: #f38ba8;
        }
        QPushButton#dangerButton:hover {
            background-color: #eba0ac;
        }
        QLabel {
            color: #cdd6f4;
        }
        QLabel#sectionLabel {
            color: #89b4fa;
            font-weight: bold;
            font-size: 13px;
        }
        QLabel#infoLabel {
            color: #a6adc8;
            font-size: 11px;
        }
        QStatusBar {
            background-color: #181825;
            color: #a6adc8;
        }
        QGroupBox {
            color: #cdd6f4;
            font-weight: bold;
            border: 1px solid #313244;
            border-radius: 8px;
            margin-top: 12px;
            padding-top: 8px;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 12px;
            padding: 0 6px;
        }
        QScrollArea {
            border: none;
            background-color: transparent;
        }
    )";
    setStyleSheet(styleSheet);

    m_editor = new ImageMapEditor(this);
    setCentralWidget(m_editor);

    setupMenuBar();
    setupToolBar();
    setupDockWidgets();
    setupStatusBar();

    // Connect signals
    connect(m_editor, &ImageMapEditor::hotspotAdded, this, &MainWindow::onHotspotAdded);
    connect(m_editor, &ImageMapEditor::hotspotRemoved, this, &MainWindow::onHotspotRemoved);
    connect(m_editor, &ImageMapEditor::hotspotSelected, this, &MainWindow::onHotspotSelected);
    connect(m_editor, &ImageMapEditor::coordinatesChanged, this, &MainWindow::onCoordinatesChanged);
    connect(m_editor, &ImageMapEditor::coordinatesCopied, this, &MainWindow::onCoordinatesCopied);

    // Set initial tool
    setCurrentTool(EditorTool::Select);
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupMenuBar()
{
    QMenuBar *menuBar = this->menuBar();

    // File menu
    QMenu *fileMenu = menuBar->addMenu("&File");

    QAction *openAction = fileMenu->addAction("&Open Image...");
    openAction->setShortcut(QKeySequence::Open);
    connect(openAction, &QAction::triggered, this, &MainWindow::openImage);

    fileMenu->addSeparator();

    QAction *loadProjectAction = fileMenu->addAction("&Load Project...");
    loadProjectAction->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_O));
    connect(loadProjectAction, &QAction::triggered, this, &MainWindow::loadProject);

    QAction *saveProjectAction = fileMenu->addAction("&Save Project...");
    saveProjectAction->setShortcut(QKeySequence::Save);
    connect(saveProjectAction, &QAction::triggered, this, &MainWindow::saveProject);

    fileMenu->addSeparator();

    QAction *exportAction = fileMenu->addAction("&Export HTML...");
    exportAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_E));
    connect(exportAction, &QAction::triggered, this, &MainWindow::exportHtml);

    fileMenu->addSeparator();

    QAction *exitAction = fileMenu->addAction("E&xit");
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, this, &QMainWindow::close);

    // Edit menu
    QMenu *editMenu = menuBar->addMenu("&Edit");

    QAction *deleteAction = editMenu->addAction("&Delete Hotspot");
    deleteAction->setShortcut(QKeySequence::Delete);
    connect(deleteAction, &QAction::triggered, this, &MainWindow::onDeleteHotspot);

    QAction *clearAction = editMenu->addAction("&Clear All Hotspots");
    connect(clearAction, &QAction::triggered, this, &MainWindow::onClearAllHotspots);

    // View menu
    QMenu *viewMenu = menuBar->addMenu("&View");

    QAction *zoomInAction = viewMenu->addAction("Zoom &In");
    zoomInAction->setShortcut(QKeySequence::ZoomIn);
    connect(zoomInAction, &QAction::triggered, m_editor, &ImageMapEditor::zoomIn);

    QAction *zoomOutAction = viewMenu->addAction("Zoom &Out");
    zoomOutAction->setShortcut(QKeySequence::ZoomOut);
    connect(zoomOutAction, &QAction::triggered, m_editor, &ImageMapEditor::zoomOut);

    QAction *zoomFitAction = viewMenu->addAction("Zoom to &Fit");
    zoomFitAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_0));
    connect(zoomFitAction, &QAction::triggered, m_editor, &ImageMapEditor::zoomFit);

    QAction *zoomResetAction = viewMenu->addAction("&Reset Zoom");
    zoomResetAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_1));
    connect(zoomResetAction, &QAction::triggered, m_editor, &ImageMapEditor::zoomReset);

    // Help menu
    QMenu *helpMenu = menuBar->addMenu("&Help");

    QAction *aboutAction = helpMenu->addAction("&About");
    connect(aboutAction, &QAction::triggered, this, [this]() {
        QMessageBox::about(this, "About Image Map Generator",
                           "<h2>Image Map Generator</h2>"
                           "<p>Create HTML image maps visually!</p>"
                           "<p>Draw rectangles, circles, and polygons on your image "
                           "and generate the HTML code automatically.</p>"
                           "<p><b>Usage:</b></p>"
                           "<ul>"
                           "<li>Open an image</li>"
                           "<li>Select a drawing tool</li>"
                           "<li>Draw hotspots on the image</li>"
                           "<li>Set URLs for each hotspot</li>"
                           "<li>Copy or export the HTML code</li>"
                           "</ul>");
    });
}

void MainWindow::setupToolBar()
{
    QToolBar *toolBar = addToolBar("Tools");
    toolBar->setMovable(false);
    toolBar->setIconSize(QSize(22, 22));
    toolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    // Open image button
    QAction *openAction = toolBar->addAction(QIcon(":/icons/icons/open.svg"), "Open");
    openAction->setToolTip("Open an image file");
    connect(openAction, &QAction::triggered, this, &MainWindow::openImage);

    toolBar->addSeparator();

    // Tool group
    m_toolGroup = new QActionGroup(this);

    m_selectAction = toolBar->addAction(QIcon(":/icons/icons/select.svg"), "Select");
    m_selectAction->setCheckable(true);
    m_selectAction->setChecked(true);
    m_selectAction->setToolTip("Select and move hotspots");
    m_toolGroup->addAction(m_selectAction);
    connect(m_selectAction, &QAction::triggered, this, &MainWindow::onToolSelect);

    m_rectAction = toolBar->addAction(QIcon(":/icons/icons/rectangle.svg"), "Rectangle");
    m_rectAction->setCheckable(true);
    m_rectAction->setToolTip("Draw rectangular hotspot");
    m_toolGroup->addAction(m_rectAction);
    connect(m_rectAction, &QAction::triggered, this, &MainWindow::onToolRect);

    m_circleAction = toolBar->addAction(QIcon(":/icons/icons/circle.svg"), "Circle");
    m_circleAction->setCheckable(true);
    m_circleAction->setToolTip("Draw circular hotspot");
    m_toolGroup->addAction(m_circleAction);
    connect(m_circleAction, &QAction::triggered, this, &MainWindow::onToolCircle);

    m_polygonAction = toolBar->addAction(QIcon(":/icons/icons/polygon.svg"), "Polygon");
    m_polygonAction->setCheckable(true);
    m_polygonAction->setToolTip("Draw polygon hotspot (right-click or double-click to finish)");
    m_toolGroup->addAction(m_polygonAction);
    connect(m_polygonAction, &QAction::triggered, this, &MainWindow::onToolPolygon);

    toolBar->addSeparator();

    // Clipboard mode toggle
    m_clipboardModeAction = toolBar->addAction(QIcon(":/icons/icons/clipboard.svg"), "Clipboard");
    m_clipboardModeAction->setCheckable(true);
    m_clipboardModeAction->setToolTip("Clipboard Mode: Click to copy coordinates");
    connect(m_clipboardModeAction, &QAction::toggled, this, &MainWindow::onClipboardModeToggled);

    toolBar->addSeparator();

    // Zoom buttons
    QAction *zoomInAction = toolBar->addAction(QIcon(":/icons/icons/zoom-in.svg"), "Zoom In");
    zoomInAction->setToolTip("Zoom in");
    connect(zoomInAction, &QAction::triggered, m_editor, &ImageMapEditor::zoomIn);

    QAction *zoomOutAction = toolBar->addAction(QIcon(":/icons/icons/zoom-out.svg"), "Zoom Out");
    zoomOutAction->setToolTip("Zoom out");
    connect(zoomOutAction, &QAction::triggered, m_editor, &ImageMapEditor::zoomOut);

    QAction *zoomFitAction = toolBar->addAction(QIcon(":/icons/icons/zoom-fit.svg"), "Fit");
    zoomFitAction->setToolTip("Fit image to window");
    connect(zoomFitAction, &QAction::triggered, m_editor, &ImageMapEditor::zoomFit);
}

void MainWindow::setupDockWidgets()
{
    // Properties dock
    m_propertiesDock = new QDockWidget("Hotspot Properties", this);
    m_propertiesDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);

    QWidget *propertiesWidget = new QWidget();
    propertiesWidget->setObjectName("dockContent");
    QVBoxLayout *propLayout = new QVBoxLayout(propertiesWidget);
    propLayout->setSpacing(12);
    propLayout->setContentsMargins(12, 12, 12, 12);

    QLabel *shapeTitle = new QLabel("Shape Info");
    shapeTitle->setObjectName("sectionLabel");
    propLayout->addWidget(shapeTitle);

    m_shapeLabel = new QLabel("No hotspot selected");
    m_shapeLabel->setObjectName("infoLabel");
    propLayout->addWidget(m_shapeLabel);

    m_coordsLabel = new QLabel("");
    m_coordsLabel->setObjectName("infoLabel");
    m_coordsLabel->setWordWrap(true);
    propLayout->addWidget(m_coordsLabel);

    propLayout->addSpacing(8);

    QLabel *linkTitle = new QLabel("Link Settings");
    linkTitle->setObjectName("sectionLabel");
    propLayout->addWidget(linkTitle);

    QFormLayout *formLayout = new QFormLayout();
    formLayout->setSpacing(8);

    m_urlEdit = new QLineEdit();
    m_urlEdit->setPlaceholderText("https://example.com");
    formLayout->addRow("URL:", m_urlEdit);
    connect(m_urlEdit, &QLineEdit::textChanged, this, &MainWindow::updateHotspotProperties);

    m_altEdit = new QLineEdit();
    m_altEdit->setPlaceholderText("Alt text for accessibility");
    formLayout->addRow("Alt Text:", m_altEdit);
    connect(m_altEdit, &QLineEdit::textChanged, this, &MainWindow::updateHotspotProperties);

    m_titleEdit = new QLineEdit();
    m_titleEdit->setPlaceholderText("Tooltip text");
    formLayout->addRow("Title:", m_titleEdit);
    connect(m_titleEdit, &QLineEdit::textChanged, this, &MainWindow::updateHotspotProperties);

    propLayout->addLayout(formLayout);

    propLayout->addStretch();

    QPushButton *deleteBtn = new QPushButton(QIcon(":/icons/icons/delete.svg"), "Delete Hotspot");
    deleteBtn->setObjectName("dangerButton");
    connect(deleteBtn, &QPushButton::clicked, this, &MainWindow::onDeleteHotspot);
    propLayout->addWidget(deleteBtn);

    m_propertiesDock->setWidget(propertiesWidget);
    addDockWidget(Qt::RightDockWidgetArea, m_propertiesDock);

    // Hotspots list dock
    m_hotspotsDock = new QDockWidget("Hotspots", this);
    m_hotspotsDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);

    QWidget *hotspotsWidget = new QWidget();
    hotspotsWidget->setObjectName("dockContent");
    QVBoxLayout *hotspotsLayout = new QVBoxLayout(hotspotsWidget);
    hotspotsLayout->setContentsMargins(12, 12, 12, 12);
    hotspotsLayout->setSpacing(8);

    m_hotspotsList = new QListWidget();
    connect(m_hotspotsList, &QListWidget::itemSelectionChanged,
            this, &MainWindow::onHotspotListSelectionChanged);
    hotspotsLayout->addWidget(m_hotspotsList);

    QPushButton *clearAllBtn = new QPushButton("Clear All");
    clearAllBtn->setObjectName("dangerButton");
    connect(clearAllBtn, &QPushButton::clicked, this, &MainWindow::onClearAllHotspots);
    hotspotsLayout->addWidget(clearAllBtn);

    m_hotspotsDock->setWidget(hotspotsWidget);
    addDockWidget(Qt::RightDockWidgetArea, m_hotspotsDock);

    // Code preview dock
    m_codeDock = new QDockWidget("HTML Code", this);
    m_codeDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);

    QWidget *codeWidget = new QWidget();
    codeWidget->setObjectName("dockContent");
    QVBoxLayout *codeLayout = new QVBoxLayout(codeWidget);
    codeLayout->setContentsMargins(12, 12, 12, 12);
    codeLayout->setSpacing(8);

    QHBoxLayout *mapNameLayout = new QHBoxLayout();
    QLabel *mapNameLabel = new QLabel("Map Name:");
    m_mapNameEdit = new QLineEdit("imagemap");
    connect(m_mapNameEdit, &QLineEdit::textChanged, this, &MainWindow::updateCodePreview);
    mapNameLayout->addWidget(mapNameLabel);
    mapNameLayout->addWidget(m_mapNameEdit);
    codeLayout->addLayout(mapNameLayout);

    m_codePreview = new QTextEdit();
    m_codePreview->setReadOnly(true);
    m_codePreview->setPlaceholderText("HTML code will appear here after adding hotspots...");
    codeLayout->addWidget(m_codePreview);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *copyBtn = new QPushButton(QIcon(":/icons/icons/copy.svg"), "Copy to Clipboard");
    connect(copyBtn, &QPushButton::clicked, this, &MainWindow::copyHtmlToClipboard);
    btnLayout->addWidget(copyBtn);

    QPushButton *exportBtn = new QPushButton(QIcon(":/icons/icons/save.svg"), "Export HTML");
    connect(exportBtn, &QPushButton::clicked, this, &MainWindow::exportHtml);
    btnLayout->addWidget(exportBtn);
    codeLayout->addLayout(btnLayout);

    m_codeDock->setWidget(codeWidget);
    addDockWidget(Qt::BottomDockWidgetArea, m_codeDock);

    // Stack the right docks
    tabifyDockWidget(m_propertiesDock, m_hotspotsDock);
    m_propertiesDock->raise();
}

void MainWindow::setupStatusBar()
{
    QStatusBar *status = statusBar();

    m_coordsStatusLabel = new QLabel("X: 0, Y: 0");
    m_coordsStatusLabel->setMinimumWidth(120);
    status->addWidget(m_coordsStatusLabel);

    m_imageInfoLabel = new QLabel("No image loaded");
    status->addPermanentWidget(m_imageInfoLabel);
}

void MainWindow::openImage()
{
    QString filePath = QFileDialog::getOpenFileName(this,
                                                    "Open Image",
                                                    QString(),
                                                    "Images (*.png *.jpg *.jpeg *.gif *.bmp *.webp);;All Files (*)");

    if (filePath.isEmpty()) {
        return;
    }

    if (m_editor->loadImage(filePath)) {
        QPixmap img = m_editor->image();
        m_imageInfoLabel->setText(QString("%1 × %2 px")
                                      .arg(img.width())
                                      .arg(img.height()));
        setWindowTitle(QString("Image Map Generator - %1").arg(QFileInfo(filePath).fileName()));
    } else {
        QMessageBox::warning(this, "Error", "Failed to load image.");
    }
}

void MainWindow::saveProject()
{
    QString filePath = QFileDialog::getSaveFileName(this,
                                                    "Save Project",
                                                    QString(),
                                                    "Image Map Project (*.imap);;All Files (*)");
    if (filePath.isEmpty()) {
        return;
    }

    QJsonObject project;
    project["imagePath"] = m_editor->imagePath();
    project["mapName"] = m_mapNameEdit->text();

    QJsonArray hotspotsArray;
    for (const HotspotItem *hotspot : m_editor->hotspots()) {
        QJsonObject h;
        h["shape"] = static_cast<int>(hotspot->hotspotShape());
        h["url"] = hotspot->url();
        h["alt"] = hotspot->altText();
        h["title"] = hotspot->title();
        h["posX"] = hotspot->pos().x();
        h["posY"] = hotspot->pos().y();

        switch (hotspot->hotspotShape()) {
        case HotspotShape::Rectangle: {
            QRectF r = hotspot->rect();
            h["x"] = r.x();
            h["y"] = r.y();
            h["width"] = r.width();
            h["height"] = r.height();
            break;
        }
        case HotspotShape::Circle: {
            h["centerX"] = hotspot->center().x();
            h["centerY"] = hotspot->center().y();
            h["radius"] = hotspot->radius();
            break;
        }
        case HotspotShape::Polygon: {
            QJsonArray points;
            for (const QPointF &pt : hotspot->polygon()) {
                QJsonObject p;
                p["x"] = pt.x();
                p["y"] = pt.y();
                points.append(p);
            }
            h["points"] = points;
            break;
        }
        }

        hotspotsArray.append(h);
    }
    project["hotspots"] = hotspotsArray;

    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(project).toJson());
        file.close();
    } else {
        QMessageBox::warning(this, "Error", "Failed to save project.");
    }
}

void MainWindow::loadProject()
{
    QString filePath = QFileDialog::getOpenFileName(this,
                                                    "Load Project",
                                                    QString(),
                                                    "Image Map Project (*.imap);;All Files (*)");
    if (filePath.isEmpty()) {
        return;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "Error", "Failed to open project file.");
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    if (!doc.isObject()) {
        QMessageBox::warning(this, "Error", "Invalid project file.");
        return;
    }

    QJsonObject project = doc.object();

    // Load image
    QString imagePath = project["imagePath"].toString();
    if (!imagePath.isEmpty()) {
        if (!m_editor->loadImage(imagePath)) {
            QMessageBox::warning(this, "Warning", "Could not load the original image. Please open it manually.");
        } else {
            QPixmap img = m_editor->image();
            m_imageInfoLabel->setText(QString("%1 × %2 px").arg(img.width()).arg(img.height()));
        }
    }

    m_mapNameEdit->setText(project["mapName"].toString("imagemap"));

    // Clear existing hotspots
    m_editor->clearAllHotspots();

    // Load hotspots
    QJsonArray hotspotsArray = project["hotspots"].toArray();
    for (const QJsonValue &val : hotspotsArray) {
        QJsonObject h = val.toObject();
        HotspotShape shape = static_cast<HotspotShape>(h["shape"].toInt());

        HotspotItem *hotspot = new HotspotItem(shape);
        hotspot->setUrl(h["url"].toString());
        hotspot->setAltText(h["alt"].toString());
        hotspot->setTitle(h["title"].toString());
        hotspot->setPos(h["posX"].toDouble(), h["posY"].toDouble());

        switch (shape) {
        case HotspotShape::Rectangle: {
            QRectF r(h["x"].toDouble(), h["y"].toDouble(),
                     h["width"].toDouble(), h["height"].toDouble());
            hotspot->setRect(r);
            break;
        }
        case HotspotShape::Circle: {
            hotspot->setCenter(QPointF(h["centerX"].toDouble(), h["centerY"].toDouble()));
            hotspot->setRadius(h["radius"].toDouble());
            break;
        }
        case HotspotShape::Polygon: {
            QJsonArray points = h["points"].toArray();
            for (const QJsonValue &pv : points) {
                QJsonObject p = pv.toObject();
                hotspot->addPolygonPoint(QPointF(p["x"].toDouble(), p["y"].toDouble()));
            }
            hotspot->closePolygon();
            break;
        }
        }

        m_editor->addHotspot(hotspot);
    }

    updateHotspotList();
    updateCodePreview();
}

void MainWindow::exportHtml()
{
    QString html = m_editor->generateImageMapHtml(m_mapNameEdit->text());
    if (html.isEmpty()) {
        QMessageBox::information(this, "Export", "No hotspots to export. Draw some hotspots first!");
        return;
    }

    QString filePath = QFileDialog::getSaveFileName(this,
                                                    "Export HTML",
                                                    QString(),
                                                    "HTML Files (*.html);;All Files (*)");
    if (filePath.isEmpty()) {
        return;
    }

    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QString fullHtml = QString(
                               "<!DOCTYPE html>\n"
                               "<html lang=\"en\">\n"
                               "<head>\n"
                               "    <meta charset=\"UTF-8\">\n"
                               "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
                               "    <title>Image Map</title>\n"
                               "</head>\n"
                               "<body>\n"
                               "    %1\n"
                               "</body>\n"
                               "</html>\n"
                               ).arg(html);

        file.write(fullHtml.toUtf8());
        file.close();
        QMessageBox::information(this, "Export", "HTML file exported successfully!");
    } else {
        QMessageBox::warning(this, "Error", "Failed to export HTML file.");
    }
}

void MainWindow::onToolSelect()
{
    setCurrentTool(EditorTool::Select);
}

void MainWindow::onToolRect()
{
    setCurrentTool(EditorTool::DrawRect);
}

void MainWindow::onToolCircle()
{
    setCurrentTool(EditorTool::DrawCircle);
}

void MainWindow::onToolPolygon()
{
    setCurrentTool(EditorTool::DrawPolygon);
}

void MainWindow::setCurrentTool(EditorTool tool)
{
    m_editor->setCurrentTool(tool);

    switch (tool) {
    case EditorTool::Select:
        m_selectAction->setChecked(true);
        break;
    case EditorTool::DrawRect:
        m_rectAction->setChecked(true);
        break;
    case EditorTool::DrawCircle:
        m_circleAction->setChecked(true);
        break;
    case EditorTool::DrawPolygon:
        m_polygonAction->setChecked(true);
        break;
    }
}

void MainWindow::onHotspotAdded(HotspotItem *hotspot)
{
    Q_UNUSED(hotspot)
    updateHotspotList();
    updateCodePreview();
}

void MainWindow::onHotspotRemoved(HotspotItem *hotspot)
{
    Q_UNUSED(hotspot)
    updateHotspotList();
    updateCodePreview();
}

void MainWindow::onHotspotSelected(HotspotItem *hotspot)
{
    // Update properties panel
    if (hotspot) {
        QString shapeName;
        switch (hotspot->hotspotShape()) {
        case HotspotShape::Rectangle: shapeName = "Rectangle"; break;
        case HotspotShape::Circle: shapeName = "Circle"; break;
        case HotspotShape::Polygon: shapeName = "Polygon"; break;
        }
        m_shapeLabel->setText(QString("Shape: %1").arg(shapeName));
        m_coordsLabel->setText(QString("Coords: %1").arg(hotspot->generateCoords()));

        // Block signals to prevent recursive updates
        m_urlEdit->blockSignals(true);
        m_altEdit->blockSignals(true);
        m_titleEdit->blockSignals(true);

        m_urlEdit->setText(hotspot->url());
        m_altEdit->setText(hotspot->altText());
        m_titleEdit->setText(hotspot->title());

        m_urlEdit->blockSignals(false);
        m_altEdit->blockSignals(false);
        m_titleEdit->blockSignals(false);

        m_urlEdit->setEnabled(true);
        m_altEdit->setEnabled(true);
        m_titleEdit->setEnabled(true);
    } else {
        m_shapeLabel->setText("No hotspot selected");
        m_coordsLabel->setText("");
        m_urlEdit->clear();
        m_altEdit->clear();
        m_titleEdit->clear();
        m_urlEdit->setEnabled(false);
        m_altEdit->setEnabled(false);
        m_titleEdit->setEnabled(false);
    }

    // Update list selection
    m_hotspotsList->blockSignals(true);
    for (int i = 0; i < m_hotspotsList->count(); ++i) {
        QListWidgetItem *item = m_hotspotsList->item(i);
        HotspotItem *h = item->data(Qt::UserRole).value<HotspotItem*>();
        item->setSelected(h == hotspot);
    }
    m_hotspotsList->blockSignals(false);

    m_propertiesDock->raise();
}

void MainWindow::onHotspotListSelectionChanged()
{
    QList<QListWidgetItem*> selected = m_hotspotsList->selectedItems();
    if (selected.isEmpty()) {
        m_editor->selectHotspot(nullptr);
    } else {
        HotspotItem *hotspot = selected.first()->data(Qt::UserRole).value<HotspotItem*>();
        m_editor->selectHotspot(hotspot);
    }
}

void MainWindow::onDeleteHotspot()
{
    m_editor->deleteSelectedHotspot();
}

void MainWindow::onClearAllHotspots()
{
    if (m_editor->hotspots().isEmpty()) {
        return;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Clear All", "Are you sure you want to delete all hotspots?",
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        m_editor->clearAllHotspots();
        updateHotspotList();
        updateCodePreview();
    }
}

void MainWindow::updateHotspotProperties()
{
    HotspotItem *hotspot = m_editor->selectedHotspot();
    if (!hotspot) {
        return;
    }

    hotspot->setUrl(m_urlEdit->text());
    hotspot->setAltText(m_altEdit->text());
    hotspot->setTitle(m_titleEdit->text());
    hotspot->update();

    updateHotspotList();
    updateCodePreview();
}

void MainWindow::updateHotspotList()
{
    m_hotspotsList->clear();

    for (HotspotItem *hotspot : m_editor->hotspots()) {
        QString shapeName;
        switch (hotspot->hotspotShape()) {
        case HotspotShape::Rectangle: shapeName = "▭ Rect"; break;
        case HotspotShape::Circle: shapeName = "○ Circle"; break;
        case HotspotShape::Polygon: shapeName = "⬡ Poly"; break;
        }

        QString title = hotspot->title().isEmpty() ? hotspot->url() : hotspot->title();
        if (title.isEmpty()) {
            title = "(no link)";
        }
        if (title.length() > 25) {
            title = title.left(23) + "...";
        }

        QListWidgetItem *item = new QListWidgetItem(QString("%1 - %2").arg(shapeName, title));
        item->setData(Qt::UserRole, QVariant::fromValue(hotspot));
        m_hotspotsList->addItem(item);
    }
}

void MainWindow::updateCodePreview()
{
    QString html = m_editor->generateImageMapHtml(m_mapNameEdit->text());
    m_codePreview->setPlainText(html);
}

void MainWindow::onCoordinatesChanged(const QPointF &pos)
{
    m_coordsStatusLabel->setText(QString("X: %1, Y: %2")
                                     .arg(qRound(pos.x()))
                                     .arg(qRound(pos.y())));
}

void MainWindow::onCoordinatesCopied(const QPointF &pos)
{
    statusBar()->showMessage(QString("Copied: %1,%2")
                                 .arg(qRound(pos.x()))
                                 .arg(qRound(pos.y())), 2000);
}

void MainWindow::onClipboardModeToggled(bool checked)
{
    m_editor->setClipboardMode(checked);

    if (checked) {
        statusBar()->showMessage("Clipboard mode ON - Click to copy coordinates", 3000);
    } else {
        statusBar()->showMessage("Clipboard mode OFF", 1500);
    }
}

void MainWindow::copyHtmlToClipboard()
{
    QString html = m_codePreview->toPlainText();
    if (html.isEmpty()) {
        QMessageBox::information(this, "Copy", "No HTML code to copy. Draw some hotspots first!");
        return;
    }

    QApplication::clipboard()->setText(html);
    QMessageBox::information(this, "Copied", "HTML code copied to clipboard!");
}
