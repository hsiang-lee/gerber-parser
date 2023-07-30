#include <QWidget>
#include <QUndoStack>
#include <memory>

class RotateCmd;
class ScaleCmd;
class ZoomInCmd;
class ZoomOutCmd;

class Gerber;
class QPainterEngine;

class GerberWidget : public QWidget
{
    Q_OBJECT;

public:
    GerberWidget(QWidget* parent);

    void Refresh(const std::vector<QString>& files);
    void ZoomIn();
    void ZoomOut();
    void Rotate();
    void ShowNormal();
    void BeginMeasure();

    void Undo();
    void Redo();

    bool CanUndo();
    bool CanRedo();

    int RotateAngle() const;

protected: signals:
    void posChanged(const QPoint& pos);
    void selectedChanged(const QRect& rect);
    void undoStatusChanged();
    void rotateAngleChanged();
    void layoutChanged(const QRect& view, const QRect& window);

protected:
    void paintEvent(QPaintEvent* e) override;
    void resizeEvent(QResizeEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    friend class ScaleCmd;
    friend class ZoomInCmd;
    friend class ZoomOutCmd;
    friend class RotateCmd;
    void ZoomInInternal();
    void ZoomOutInternal();
    void RotateInternal(bool clockwise);
    void Scale(double angle, int x, int y);

    int rotate_angle_{0};

    std::shared_ptr<QPainterEngine> engine_;
    std::vector<std::shared_ptr<Gerber>> gerbers_;

    QPoint last_pt_;

    QUndoStack undo_stack_;
};
