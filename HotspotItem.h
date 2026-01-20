#ifndef HOTSPOTITEM_H
#define HOTSPOTITEM_H

#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsPolygonItem>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QString>
#include <QPolygonF>
#include <QUuid>

enum class HotspotShape {
    Rectangle,
    Circle,
    Polygon
};

class HotspotItem : public QGraphicsItem
{
public:
    explicit HotspotItem(HotspotShape shape, QGraphicsItem *parent = nullptr);

    void setUrl(const QString &url) { m_url = url; }
    QString url() const { return m_url; }

    void setAltText(const QString &alt) { m_altText = alt; }
    QString altText() const { return m_altText; }

    void setTitle(const QString &title) { m_title = title; }
    QString title() const { return m_title; }

    QString id() const { return m_id; }

    HotspotShape hotspotShape() const { return m_shape; }

    // For rectangle
    void setRect(const QRectF &rect);
    QRectF rect() const { return m_rect; }

    // For circle
    void setCenter(const QPointF &center);
    void setRadius(qreal radius);
    QPointF center() const { return m_center; }
    qreal radius() const { return m_radius; }

    // For polygon
    void setPolygon(const QPolygonF &polygon);
    void addPolygonPoint(const QPointF &point);
    QPolygonF polygon() const { return m_polygon; }
    void closePolygon();
    bool isPolygonClosed() const { return m_polygonClosed; }

    // Generate HTML coords attribute
    QString generateCoords() const;
    QString generateShapeName() const;
    QString generateAreaTag() const;

    // QGraphicsItem interface
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    QPainterPath shape() const override;

    void setSelected(bool selected);
    bool isItemSelected() const { return m_selected; }

    void setColor(const QColor &color) { m_color = color; }
    QColor color() const { return m_color; }

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
    QString m_id;
    QString m_url;
    QString m_altText;
    QString m_title;
    HotspotShape m_shape;
    QColor m_color;
    bool m_selected = false;

    // Rectangle data
    QRectF m_rect;

    // Circle data
    QPointF m_center;
    qreal m_radius = 0;

    // Polygon data
    QPolygonF m_polygon;
    bool m_polygonClosed = false;

    // Dragging
    QPointF m_dragStart;
    bool m_dragging = false;
};

#endif // HOTSPOTITEM_H
