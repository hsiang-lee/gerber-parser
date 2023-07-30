#include "gerber_widget.h"
#include "undo_commands.h"

#include <QOpenGLWidget>
#include <QMouseEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QPainter>
#include <QUndoStack>

#include "engines/qpainter_engine.h"
#include "gerber_parser/gerber_parser.h"
#include "gerber/gerber.h"

GerberWidget::GerberWidget(QWidget* parent)
    : QWidget(parent)
{
    setCursor(Qt::OpenHandCursor);
    setMouseTracking(true);
    undo_stack_.setUndoLimit(50);

    connect(&undo_stack_, &QUndoStack::canRedoChanged, this, &GerberWidget::undoStatusChanged);
    connect(&undo_stack_, &QUndoStack::canUndoChanged, this, &GerberWidget::undoStatusChanged);
}

void GerberWidget::Refresh(const std::vector<QString>& files) {
    gerbers_.clear();
    engine_.reset();
    for (const auto& file : files) {
        auto parser = std::make_unique<GerberParser>(file.toLocal8Bit().toStdString());

        try {
            gerbers_.push_back(parser->GetGerber());
        }
        catch (const std::exception& e) {
            QMessageBox::warning(this, "Message", e.what(), QMessageBox::Ok);
        }
    }

    update();
}

void GerberWidget::ZoomIn() {
    undo_stack_.push(new ZoomInCmd(this));
}

void GerberWidget::ZoomOut() {
    undo_stack_.push(new ZoomOutCmd(this));
}

void GerberWidget::ZoomInInternal() {
    if (engine_) {
        engine_->Scale(0.05, 0.5, 0.5);
        update();
    }
}

void GerberWidget::ZoomOutInternal() {
    if (engine_) {
        engine_->Scale(-0.05, 0.5, 0.5);
        update();
    }
}

void GerberWidget::Scale(double angle, int x, int y) {
    if (engine_) {
        engine_->Scale(angle * 0.0006, x, x);
        update();
    }
}

void GerberWidget::Rotate() {
    undo_stack_.push(new RotateCmd(this));
}

void GerberWidget::RotateInternal(bool clockwise) {
    if (engine_) {
        rotate_angle_ += (clockwise?90:-90);
        rotate_angle_ %= 360;
        engine_->Rotate(-rotate_angle_);
        update();

        emit rotateAngleChanged();
    }
}

int GerberWidget::RotateAngle() const {
    return rotate_angle_;
}

void GerberWidget::ShowNormal() {
    if (engine_) {
        engine_->Reset();
        update();
    }

    undo_stack_.clear();
}

void GerberWidget::BeginMeasure() {
    setCursor(Qt::CrossCursor);
}

void GerberWidget::Undo() {
    undo_stack_.undo();
}

void GerberWidget::Redo() {
    undo_stack_.redo();
}

bool GerberWidget::CanUndo()
{
    return undo_stack_.canUndo();
}

bool GerberWidget::CanRedo()
{
    return undo_stack_.canRedo();
}

void GerberWidget::paintEvent(QPaintEvent* e) {
    if (!engine_ && !gerbers_.empty()) {
        BoundBox box;
        for (const auto& each : gerbers_) {
            box.Update(each->GetBBox());
        }

        engine_ = std::make_shared<QPainterEngine>(this, box, 0.025);
    }

    QPainter painter(this);
    painter.setBrush(QColor(255, 255, 255));
    painter.drawRect(0, -1, width() + 1, height() + 10);
    for (auto& each : gerbers_) {
        engine_->RenderGerber(each);
    }

    if (engine_) {
        emit layoutChanged(engine_->PainterWindow(), engine_->Dev2Logic(rect()));
    }

    e->accept();
}

void GerberWidget::resizeEvent(QResizeEvent* event) {
    if (engine_) {
        engine_->Resize();
        QWidget::resizeEvent(event);
    }
}

void GerberWidget::wheelEvent(QWheelEvent* event) {
    const auto angle = event->angleDelta().y();
    undo_stack_.push(new ScaleCmd(this, angle, event->x(), event->y()));
}

void GerberWidget::mouseMoveEvent(QMouseEvent* event) {
    if (!engine_) {
        return;
    }

    if (cursor().shape() == Qt::CrossCursor) {
        emit posChanged(engine_->Dev2Logic(event->pos()));
    }

    if (!(event->buttons() & Qt::LeftButton)) {
        event->accept();
        return;
    }

    if (cursor().shape() == Qt::CrossCursor) {
        engine_->SetSelectRect(QRect(last_pt_, event->pos()));
        emit selectedChanged(QRect(engine_->Dev2Logic(last_pt_), engine_->Dev2Logic(event->pos())));
    }
    else if (cursor().shape() == Qt::OpenHandCursor) {
        const auto movement = event->pos() - last_pt_;
        engine_->Move(movement.x(), movement.y());
        last_pt_ = event->pos();
    }

    update();
    event->accept();
}

void GerberWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton) {
        last_pt_ = event->pos();
    }

    event->accept();
}
