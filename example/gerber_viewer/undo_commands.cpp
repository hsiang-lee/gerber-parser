#include "undo_commands.h"
#include "gerber_widget.h"

ZoomOutCmd::ZoomOutCmd(GerberWidget* widget) : widget_(widget) {

}

void ZoomOutCmd::undo() {
    if (!widget_) {
        return;
    }

    widget_->ZoomInInternal();
}

void ZoomOutCmd::redo() {
    if (!widget_) {
        return;
    }

    widget_->ZoomOutInternal();
}

ZoomInCmd::ZoomInCmd(GerberWidget* widget) : widget_(widget) {

}

void ZoomInCmd::undo() {
    if (!widget_) {
        return;
    }

    widget_->ZoomOutInternal();
}

void ZoomInCmd::redo() {
    if (!widget_) {
        return;
    }

    widget_->ZoomInInternal();
}

ScaleCmd::ScaleCmd(GerberWidget* widget, double angle ,int x, int y) :
    widget_(widget),
    angle_(angle),
    center_x_(x),
    center_y_(y)
{

}

void ScaleCmd::undo() {
    if (!widget_) {
        return;
    }

    widget_->Scale(-angle_, center_x_, center_y_);
}

void ScaleCmd::redo() {
    if (!widget_) {
        return;
    }

    widget_->Scale(angle_, center_x_, center_y_);
}

RotateCmd::RotateCmd(GerberWidget* widget) :
    widget_(widget)
{

}

void RotateCmd::undo() {
    if (!widget_) {
        return;
    }

    widget_->RotateInternal(false);
}

void RotateCmd::redo() {
    if (!widget_) {
        return;
    }

    widget_->RotateInternal(true);
}
