#pragma once

#include <QWidget>
#include <QPushButton>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QFileDialog>

class MumaUnpackerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MumaUnpackerWidget(QWidget* parent = nullptr);
    ~MumaUnpackerWidget() = default;
    QPushButton* m_startBtn;
    QPushButton* m_stopBtn;
    QPushButton* m_dumpBtn;
    QPushButton* m_contBtn;

private slots:
    void onStartUnpackingClicked();
    void onContinueUnpackingClicked();
    void onStopUnpackedClicked();
    void onDumpUnpackedClicked();
    void onOpenFileClicked();
    void onCheckboxChanged(int state);
    void onSelectAllClicked();
    void onClearAllClicked();

private:
    QVector<QCheckBox*> m_checkboxes;
    QPushButton* m_openBtn;
};
