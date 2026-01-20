#include "ImageMapEditor.h"
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QScrollBar>
#include <QFileInfo>
#include <QClipboard>
#include <QApplication>
#include <cmath>

ImageMapEditor::ImageMapEditor(QWidget *parent)
    : QGraphicsView(parent)
{
    m_scene = new QGraphicsScene(this);
    setScene(m_scene);

    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::SmoothPixmapTransform);
    setDragMode(QGraphicsView::NoDrag);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    setBackgroundBrush(QBrush(QColor(45, 45, 48)));
    setFrameShape(QFrame::NoFrame);

    setMouseTracking(true);
}

bool ImageMapEditor::loadImage(const QString &filePath)
{
    QPixmap pixmap(filePath);
    if (pixmap.isNull()) {
        return false;
    }

    m_imagePath = filePath;
    setImage(pixmap);
    emit imageLoaded(filePath);
    return true;
}

void ImageMapEditor::setImage(const QPixmap &pixmap)
{
    if (m_imageItem) {
        m_scene->removeItem(m_imageItem);
        delete m_imageItem;
    }

    m_imageItem = m_scene->addPixmap(pixmap);
    m_imageItem->setZValue(-1000);
    m_scene->setSceneRect(pixmap.rect());

    zoomFit();
}

QPixmap ImageMapEditor::image() const
{
    if (m_imageItem) {
        return m_imageItem->pixmap();
    }
    return QPixmap();
}

void ImageMapEditor::setCurrentTool(EditorTool tool)
{
    if (m_isDrawing) {
        cancelCurrentDrawing();
    }

    m_currentTool = tool;

    switch (tool) {
    case EditorTool::Select:
        setCursor(Qt::ArrowCursor);
        break;
    case EditorTool::DrawRect:
    case EditorTool::DrawCircle:
    case EditorTool::DrawPolygon:
        setCursor(Qt::CrossCursor);
        break;
    }
}

void ImageMapEditor::addHotspot(HotspotItem *hotspot)
{
    m_scene->addItem(hotspot);
    m_hotspots.append(hotspot);
    emit hotspotAdded(hotspot);
}

void ImageMapEditor::removeHotspot(HotspotItem *hotspot)
{
    if (m_selectedHotspot == hotspot) {
        m_selectedHotspot = nullptr;
    }
    m_hotspots.removeOne(hotspot);
    m_scene->removeItem(hotspot);
    emit hotspotRemoved(hotspot);
    delete hotspot;
}

void ImageMapEditor::clearAllHotspots()
{
    for (HotspotItem *hotspot : m_hotspots) {
        m_scene->removeItem(hotspot);
        delete hotspot;
    }
    m_hotspots.clear();
    m_selectedHotspot = nullptr;
}

void ImageMapEditor::selectHotspot(HotspotItem *hotspot)
{
    if (m_selectedHotspot) {
        m_selectedHotspot->setSelected(false);
    }

    m_selectedHotspot = hotspot;

    if (hotspot) {
        hotspot->setSelected(true);
    }

    emit hotspotSelected(hotspot);
}

void ImageMapEditor::deleteSelectedHotspot()
{
    if (m_selectedHotspot) {
        removeHotspot(m_selectedHotspot);
    }
}

QString ImageMapEditor::generateImageMapHtml(const QString &mapName) const
{
    if (!m_imageItem || m_hotspots.isEmpty()) {
        return QString();
    }

    QString imageName = QFileInfo(m_imagePath).fileName();
    if (imageName.isEmpty()) {
        imageName = "image.png";
    }

    QStringList html;
    html << QString("<img src=\"%1\" usemap=\"#%2\" alt=\"Image Map\">")
                .arg(imageName.toHtmlEscaped())
                .arg(mapName.toHtmlEscaped());
    html << QString("<map name=\"%1\">").arg(mapName.toHtmlEscaped());

    for (const HotspotItem *hotspot : m_hotspots) {
        html << "  " + hotspot->generateAreaTag();
    }

    html << "</map>";

    return html.join("\n");
}

void ImageMapEditor::zoomIn()
{
    m_zoomFactor *= 1.25;
    setTransform(QTransform::fromScale(m_zoomFactor, m_zoomFactor));
}

void ImageMapEditor::zoomOut()
{
    m_zoomFactor /= 1.25;
    setTransform(QTransform::fromScale(m_zoomFactor, m_zoomFactor));
}

void ImageMapEditor::zoomFit()
{
    if (!m_imageItem) return;

    fitInView(m_imageItem, Qt::KeepAspectRatio);
    m_zoomFactor = transform().m11();
}

void ImageMapEditor::zoomReset()
{
    m_zoomFactor = 1.0;
    setTransform(QTransform());
}

void ImageMapEditor::setClipboardMode(bool enabled)
{
    m_clipboardMode = enabled;
    if (enabled) {
        setCursor(Qt::CrossCursor);
    } else {
        setCurrentTool(m_currentTool); // Reset cursor based on current tool
    }
}

void ImageMapEditor::mousePressEvent(QMouseEvent *event)
{
    QPointF scenePos = mapToScene(event->pos());
    emit coordinatesChanged(scenePos);

    // Handle clipboard mode
    if (m_clipboardMode && event->button() == Qt::LeftButton) {
        QString coords = QString("%1,%2").arg(qRound(scenePos.x())).arg(qRound(scenePos.y()));
        QApplication::clipboard()->setText(coords);
        emit coordinatesCopied(scenePos);
        return;
    }

    if (event->button() == Qt::LeftButton) {
        switch (m_currentTool) {
        case EditorTool::Select: {
            HotspotItem *item = hotspotAt(scenePos);
            selectHotspot(item);
            break;
        }
        case EditorTool::DrawRect: {
            m_isDrawing = true;
            m_drawStart = scenePos;
            m_currentDrawingItem = new HotspotItem(HotspotShape::Rectangle);
            m_currentDrawingItem->setRect(QRectF(scenePos, QSizeF(1, 1)));
            m_scene->addItem(m_currentDrawingItem);
            break;
        }
        case EditorTool::DrawCircle: {
            m_isDrawing = true;
            m_drawStart = scenePos;
            m_currentDrawingItem = new HotspotItem(HotspotShape::Circle);
            m_currentDrawingItem->setCenter(scenePos);
            m_currentDrawingItem->setRadius(1);
            m_scene->addItem(m_currentDrawingItem);
            break;
        }
        case EditorTool::DrawPolygon: {
            if (!m_isDrawing) {
                m_isDrawing = true;
                m_currentDrawingItem = new HotspotItem(HotspotShape::Polygon);
                m_currentDrawingItem->addPolygonPoint(scenePos);
                m_scene->addItem(m_currentDrawingItem);
            } else if (m_currentDrawingItem) {
                m_currentDrawingItem->addPolygonPoint(scenePos);
                m_currentDrawingItem->update();
            }
            break;
        }
        }
    } else if (event->button() == Qt::RightButton) {
        if (m_isDrawing && m_currentTool == EditorTool::DrawPolygon) {
            finishCurrentDrawing();
        }
    }

    QGraphicsView::mousePressEvent(event);
}

void ImageMapEditor::mouseMoveEvent(QMouseEvent *event)
{
    QPointF scenePos = mapToScene(event->pos());
    emit coordinatesChanged(scenePos);

    if (m_isDrawing && m_currentDrawingItem) {
        switch (m_currentTool) {
        case EditorTool::DrawRect: {
            QRectF rect = QRectF(m_drawStart, scenePos).normalized();
            m_currentDrawingItem->setRect(rect);
            m_currentDrawingItem->setPos(0, 0);
            break;
        }
        case EditorTool::DrawCircle: {
            qreal dx = scenePos.x() - m_drawStart.x();
            qreal dy = scenePos.y() - m_drawStart.y();
            qreal radius = std::sqrt(dx * dx + dy * dy);
            m_currentDrawingItem->setRadius(radius);
            break;
        }
        case EditorTool::DrawPolygon:
            // Polygon points are added on click
            break;
        default:
            break;
        }
    }

    QGraphicsView::mouseMoveEvent(event);
}

void ImageMapEditor::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && m_isDrawing) {
        if (m_currentTool == EditorTool::DrawRect || m_currentTool == EditorTool::DrawCircle) {
            finishCurrentDrawing();
        }
        // Polygon continues until right-click or double-click
    }

    QGraphicsView::mouseReleaseEvent(event);
}

void ImageMapEditor::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (m_isDrawing && m_currentTool == EditorTool::DrawPolygon) {
        finishCurrentDrawing();
        return;
    }

    QGraphicsView::mouseDoubleClickEvent(event);
}

void ImageMapEditor::wheelEvent(QWheelEvent *event)
{
    if (event->modifiers() & Qt::ControlModifier) {
        if (event->angleDelta().y() > 0) {
            zoomIn();
        } else {
            zoomOut();
        }
        event->accept();
    } else {
        QGraphicsView::wheelEvent(event);
    }
}

void ImageMapEditor::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace) {
        deleteSelectedHotspot();
    } else if (event->key() == Qt::Key_Escape) {
        if (m_isDrawing) {
            cancelCurrentDrawing();
        } else {
            selectHotspot(nullptr);
        }
    } else if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        if (m_isDrawing && m_currentTool == EditorTool::DrawPolygon) {
            finishCurrentDrawing();
        }
    }

    QGraphicsView::keyPressEvent(event);
}

void ImageMapEditor::drawBackground(QPainter *painter, const QRectF &rect)
{
    QGraphicsView::drawBackground(painter, rect);

    // Draw checkerboard pattern for transparency
    if (m_imageItem) {
        const int gridSize = 10;
        QColor color1(200, 200, 200);
        QColor color2(255, 255, 255);

        QRectF imageRect = m_imageItem->boundingRect();

        for (int x = imageRect.left(); x < imageRect.right(); x += gridSize) {
            for (int y = imageRect.top(); y < imageRect.bottom(); y += gridSize) {
                QRectF cell(x, y, gridSize, gridSize);
                if (cell.intersects(rect)) {
                    bool isEven = ((x / gridSize) + (y / gridSize)) % 2 == 0;
                    painter->fillRect(cell.intersected(imageRect), isEven ? color1 : color2);
                }
            }
        }
    }
}

void ImageMapEditor::finishCurrentDrawing()
{
    if (!m_currentDrawingItem) {
        m_isDrawing = false;
        return;
    }

    bool validShape = false;

    switch (m_currentTool) {
    case EditorTool::DrawRect:
        validShape = m_currentDrawingItem->rect().width() > 5 &&
                     m_currentDrawingItem->rect().height() > 5;
        break;
    case EditorTool::DrawCircle:
        validShape = m_currentDrawingItem->radius() > 5;
        break;
    case EditorTool::DrawPolygon:
        validShape = m_currentDrawingItem->polygon().size() >= 3;
        if (validShape) {
            m_currentDrawingItem->closePolygon();
        }
        break;
    default:
        break;
    }

    if (validShape) {
        m_hotspots.append(m_currentDrawingItem);
        emit hotspotAdded(m_currentDrawingItem);
        selectHotspot(m_currentDrawingItem);
    } else {
        m_scene->removeItem(m_currentDrawingItem);
        delete m_currentDrawingItem;
    }

    m_currentDrawingItem = nullptr;
    m_isDrawing = false;
}

void ImageMapEditor::cancelCurrentDrawing()
{
    if (m_currentDrawingItem) {
        m_scene->removeItem(m_currentDrawingItem);
        delete m_currentDrawingItem;
        m_currentDrawingItem = nullptr;
    }
    m_isDrawing = false;
}

HotspotItem* ImageMapEditor::hotspotAt(const QPointF &scenePos)
{
    QList<QGraphicsItem*> items = m_scene->items(scenePos);
    for (QGraphicsItem *item : items) {
        HotspotItem *hotspot = dynamic_cast<HotspotItem*>(item);
        if (hotspot) {
            return hotspot;
        }
    }
    return nullptr;
}
