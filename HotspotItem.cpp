#include "HotspotItem.h"
#include <QGraphicsSceneMouseEvent>
#include <QCursor>
#include <cmath>

HotspotItem::HotspotItem(HotspotShape shape, QGraphicsItem *parent)
    : QGraphicsItem(parent)
    , m_shape(shape)
    , m_color(QColor(65, 155, 249, 100))
{
    m_id = QUuid::createUuid().toString(QUuid::WithoutBraces).left(8);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsMovable);
    setAcceptHoverEvents(true);
    setCursor(Qt::OpenHandCursor);
}

void HotspotItem::setRect(const QRectF &rect)
{
    prepareGeometryChange();
    m_rect = rect;
}

void HotspotItem::setCenter(const QPointF &center)
{
    prepareGeometryChange();
    m_center = center;
}

void HotspotItem::setRadius(qreal radius)
{
    prepareGeometryChange();
    m_radius = radius;
}

void HotspotItem::setPolygon(const QPolygonF &polygon)
{
    prepareGeometryChange();
    m_polygon = polygon;
}

void HotspotItem::addPolygonPoint(const QPointF &point)
{
    prepareGeometryChange();
    m_polygon.append(point);
}

void HotspotItem::closePolygon()
{
    m_polygonClosed = true;
}

QString HotspotItem::generateCoords() const
{
    QStringList coords;

    switch (m_shape) {
    case HotspotShape::Rectangle: {
        QRectF r = m_rect.translated(pos());
        coords << QString::number(qRound(r.left()))
               << QString::number(qRound(r.top()))
               << QString::number(qRound(r.right()))
               << QString::number(qRound(r.bottom()));
        break;
    }
    case HotspotShape::Circle: {
        QPointF c = m_center + pos();
        coords << QString::number(qRound(c.x()))
               << QString::number(qRound(c.y()))
               << QString::number(qRound(m_radius));
        break;
    }
    case HotspotShape::Polygon: {
        for (const QPointF &pt : m_polygon) {
            QPointF p = pt + pos();
            coords << QString::number(qRound(p.x()))
                   << QString::number(qRound(p.y()));
        }
        break;
    }
    }

    return coords.join(",");
}

QString HotspotItem::generateShapeName() const
{
    switch (m_shape) {
    case HotspotShape::Rectangle: return "rect";
    case HotspotShape::Circle: return "circle";
    case HotspotShape::Polygon: return "poly";
    }
    return "rect";
}

QString HotspotItem::generateAreaTag() const
{
    QString tag = QString("<area shape=\"%1\" coords=\"%2\" href=\"%3\"")
                      .arg(generateShapeName())
                      .arg(generateCoords())
                      .arg(m_url.isEmpty() ? "#" : m_url.toHtmlEscaped());

    if (!m_altText.isEmpty()) {
        tag += QString(" alt=\"%1\"").arg(m_altText.toHtmlEscaped());
    } else {
        tag += " alt=\"\"";
    }

    if (!m_title.isEmpty()) {
        tag += QString(" title=\"%1\"").arg(m_title.toHtmlEscaped());
    }

    tag += ">";
    return tag;
}

QRectF HotspotItem::boundingRect() const
{
    const qreal padding = 4;

    switch (m_shape) {
    case HotspotShape::Rectangle:
        return m_rect.adjusted(-padding, -padding, padding, padding);
    case HotspotShape::Circle: {
        QRectF r(m_center.x() - m_radius - padding,
                 m_center.y() - m_radius - padding,
                 (m_radius + padding) * 2,
                 (m_radius + padding) * 2);
        return r;
    }
    case HotspotShape::Polygon:
        return m_polygon.boundingRect().adjusted(-padding, -padding, padding, padding);
    }
    return QRectF();
}

void HotspotItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    painter->setRenderHint(QPainter::Antialiasing);

    QColor fillColor = m_color;
    QColor borderColor = m_color.darker(120);
    borderColor.setAlpha(200);

    if (m_selected || isSelected()) {
        fillColor = QColor(255, 140, 0, 120);
        borderColor = QColor(255, 100, 0, 220);
    }

    QPen pen(borderColor, 2, Qt::SolidLine);
    painter->setPen(pen);
    painter->setBrush(fillColor);

    switch (m_shape) {
    case HotspotShape::Rectangle:
        painter->drawRect(m_rect);
        break;
    case HotspotShape::Circle:
        painter->drawEllipse(m_center, m_radius, m_radius);
        break;
    case HotspotShape::Polygon:
        if (m_polygonClosed) {
            painter->drawPolygon(m_polygon);
        } else {
            painter->drawPolyline(m_polygon);
            // Draw points
            painter->setBrush(borderColor);
            for (const QPointF &pt : m_polygon) {
                painter->drawEllipse(pt, 4, 4);
            }
        }
        break;
    }

    // Draw label
    if (!m_title.isEmpty() || !m_url.isEmpty()) {
        QString label = m_title.isEmpty() ? m_url : m_title;
        if (label.length() > 20) {
            label = label.left(18) + "...";
        }

        QRectF br = boundingRect();
        QPointF textPos;

        switch (m_shape) {
        case HotspotShape::Rectangle:
            textPos = m_rect.center();
            break;
        case HotspotShape::Circle:
            textPos = m_center;
            break;
        case HotspotShape::Polygon:
            textPos = m_polygon.boundingRect().center();
            break;
        }

        QFont font = painter->font();
        font.setPixelSize(11);
        font.setBold(true);
        painter->setFont(font);

        QFontMetrics fm(font);
        QRectF textRect = fm.boundingRect(label);
        textRect.moveCenter(textPos);

        // Background for text
        painter->setPen(Qt::NoPen);
        painter->setBrush(QColor(0, 0, 0, 160));
        painter->drawRoundedRect(textRect.adjusted(-4, -2, 4, 2), 3, 3);

        painter->setPen(Qt::white);
        painter->drawText(textRect, Qt::AlignCenter, label);
    }
}

QPainterPath HotspotItem::shape() const
{
    QPainterPath path;

    switch (m_shape) {
    case HotspotShape::Rectangle:
        path.addRect(m_rect);
        break;
    case HotspotShape::Circle:
        path.addEllipse(m_center, m_radius, m_radius);
        break;
    case HotspotShape::Polygon:
        path.addPolygon(m_polygon);
        break;
    }

    return path;
}

void HotspotItem::setSelected(bool selected)
{
    m_selected = selected;
    update();
}

void HotspotItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = true;
        m_dragStart = event->scenePos();
        setCursor(Qt::ClosedHandCursor);
    }
    QGraphicsItem::mousePressEvent(event);
}

void HotspotItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseMoveEvent(event);
}

void HotspotItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    m_dragging = false;
    setCursor(Qt::OpenHandCursor);
    QGraphicsItem::mouseReleaseEvent(event);
}
