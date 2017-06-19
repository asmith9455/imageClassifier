#include "Clickablelabel.h"

ClickableLabel::ClickableLabel(QWidget* parent, Qt::WindowFlags f)
    : QLabel(parent) {

}

ClickableLabel::~ClickableLabel() {}

void ClickableLabel::mousePressEvent(QMouseEvent* event) {
    int mouseX = event->x();
    int mouseY = event->y();
    emit clicked(mouseX, mouseY);
}
