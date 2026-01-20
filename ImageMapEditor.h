#ifndef IMAGEMAPEDITOR_H
#define IMAGEMAPEDITOR_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QList>
#include "HotspotItem.h"

enum class EditorTool {
    Select,
    DrawRect,
    DrawCircle,
    DrawPolygon
};

class ImageMapEditor : public QGraphicsView
{
    Q_OBJECT

public:
    explicit ImageMapEditor(QWidget *parent = nullptr);

    bool loadImage(const QString &filePath);
    void setImage(const QPixmap &pixmap);
    QPixmap image() const;
    QString imagePath() const { return m_imagePath; }

    void setCurrentTool(EditorTool tool);
    EditorTool currentTool() const { return m_currentTool; }

    QList<HotspotItem*> hotspots() const { return m_hotspots; }
    HotspotItem* selectedHotspot() const { return m_selectedHotspot; }

    void addHotspot(HotspotItem *hotspot);
    void removeHotspot(HotspotItem *hotspot);
    void clearAllHotspots();

    void selectHotspot(HotspotItem *hotspot);
    void deleteSelectedHotspot();

    QString generateImageMapHtml(const QString &mapName = "imagemap") const;

    void zoomIn();
    void zoomOut();
    void zoomFit();
    void zoomReset();

signals:
    void hotspotAdded(HotspotItem *hotspot);
    void hotspotRemoved(HotspotItem *hotspot);
    void hotspotSelected(HotspotItem *hotspot);
    void imageLoaded(const QString &path);
    void coordinatesChanged(const QPointF &pos);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void drawBackground(QPainter *painter, const QRectF &rect) override;

private:
    void finishCurrentDrawing();
    void cancelCurrentDrawing();
    HotspotItem* hotspotAt(const QPointF &scenePos);

    QGraphicsScene *m_scene;
    QGraphicsPixmapItem *m_imageItem = nullptr;
    QString m_imagePath;

    EditorTool m_currentTool = EditorTool::Select;
    QList<HotspotItem*> m_hotspots;
    HotspotItem *m_selectedHotspot = nullptr;

    // Drawing state
    bool m_isDrawing = false;
    QPointF m_drawStart;
    HotspotItem *m_currentDrawingItem = nullptr;

    qreal m_zoomFactor = 1.0;
};

#endif // IMAGEMAPEDITOR_H
