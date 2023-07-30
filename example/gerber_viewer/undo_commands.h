#pragma once
#include <QUndoCommand>

class GerberWidget;

class ZoomOutCmd : public QUndoCommand {
public:
    ZoomOutCmd(GerberWidget* widget);

protected:
    void undo() override;
    void redo() override;

private:
    GerberWidget* widget_;
};

class ZoomInCmd : public QUndoCommand {
public:
    ZoomInCmd(GerberWidget* widget);

protected:
    void undo() override;
    void redo() override;

private:
    GerberWidget* widget_;
};

class ScaleCmd : public QUndoCommand {
public:
    ScaleCmd(GerberWidget* widget, double angle, int x, int y);

protected:
    void undo() override;
    void redo() override;

private:
    GerberWidget* widget_;
    double angle_;
    int center_x_;
    int center_y_;
};

class RotateCmd : public QUndoCommand {
public:
    RotateCmd(GerberWidget* widget);

protected:
    void undo() override;
    void redo() override;

private:
    GerberWidget* widget_;
};
