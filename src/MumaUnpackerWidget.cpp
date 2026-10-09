#include "MumaUnpackerWidget.h"
#include "plugin.h"
#include "BreakpointsHandlers.h"

MumaUnpackerWidget::MumaUnpackerWidget(QWidget* parent)
    : QWidget(parent)
{
    setWindowTitle("Muma Unpacker");
    setWindowIcon(QIcon(":/icon.png"));

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QLabel* titleLabel = new QLabel("First select functions to break on:");
    titleLabel->setStyleSheet("font-weight: bold; font-size: 12px;");
    mainLayout->addWidget(titleLabel);

    QHBoxLayout* columnsLayout = new QHBoxLayout();
    QVBoxLayout* leftColumn = new QVBoxLayout();
    QVBoxLayout* middleColumn = new QVBoxLayout();
    QVBoxLayout* rightColumn = new QVBoxLayout();

    m_checkboxes.resize(functionsCount);
    for (int i = 0; i < functionsCount; i++)
    {
        m_checkboxes[i] = new QCheckBox(functions[i].name);
        m_checkboxes[i]->setChecked(functions[i].is_enabled); 

        connect(m_checkboxes[i], &QCheckBox::stateChanged, this, &MumaUnpackerWidget::onCheckboxChanged);
        if (i % 3 == 0)
        {
            leftColumn->addWidget(m_checkboxes[i]);
        }
        else if (i % 3 == 1)
        {
            middleColumn->addWidget(m_checkboxes[i]);
        }
        else
        {
            rightColumn->addWidget(m_checkboxes[i]);
        }
    }

    columnsLayout->addLayout(leftColumn);
    columnsLayout->addLayout(middleColumn);
    columnsLayout->addLayout(rightColumn);
    mainLayout->addLayout(columnsLayout);

    // Кнопки Select All / Clear All
    QHBoxLayout* selectLayout = new QHBoxLayout();
    QPushButton* selectAllBtn = new QPushButton("Select All");
    QPushButton* clearAllBtn = new QPushButton("Clear All");
    selectLayout->addWidget(selectAllBtn);
    selectLayout->addWidget(clearAllBtn);
    mainLayout->addLayout(selectLayout);

    connect(selectAllBtn, &QPushButton::clicked, this, &MumaUnpackerWidget::onSelectAllClicked);
    connect(clearAllBtn, &QPushButton::clicked, this, &MumaUnpackerWidget::onClearAllClicked);

    QFrame* line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    mainLayout->addWidget(line);

    mainLayout->addWidget(line);

    QLabel* titleLabel2 = new QLabel("Second open file to unpack");
    titleLabel2->setStyleSheet("font-weight: bold; font-size: 12px;");
    mainLayout->addWidget(titleLabel2);

    m_openBtn = new QPushButton("Open file");
    mainLayout->addWidget(m_openBtn);
    connect(m_openBtn, &QPushButton::clicked, this, &MumaUnpackerWidget::onOpenFileClicked);

    mainLayout->addWidget(line);

    QLabel* titleLabel3 = new QLabel("Third wait until buttons will be visible ");
    titleLabel3->setStyleSheet("font-weight: bold; font-size: 12px;");
    mainLayout->addWidget(titleLabel3);

    m_startBtn = new QPushButton("Start Auto Unpacking");
    m_contBtn = new QPushButton("Continue Unpacking");
    m_dumpBtn = new QPushButton("Dump Unpacked");
    m_stopBtn = new QPushButton("Stop Unpacking");

    mainLayout->addWidget(m_startBtn);
    mainLayout->addWidget(m_contBtn);
    mainLayout->addWidget(m_dumpBtn);
    mainLayout->addWidget(m_stopBtn);
    mainLayout->addStretch();

    m_startBtn->setEnabled(false);
    m_contBtn->setEnabled(false);
    m_dumpBtn->setEnabled(false);
    m_stopBtn->setEnabled(false);

    connect(m_startBtn, &QPushButton::clicked, this, &MumaUnpackerWidget::onStartUnpackingClicked);
    connect(m_contBtn, &QPushButton::clicked, this, &MumaUnpackerWidget::onContinueUnpackingClicked);
    connect(m_dumpBtn, &QPushButton::clicked, this, &MumaUnpackerWidget::onDumpUnpackedClicked);
    connect(m_stopBtn, &QPushButton::clicked, this, &MumaUnpackerWidget::onStopUnpackedClicked);
}

void MumaUnpackerWidget::onStartUnpackingClicked()
{
    is_auto_unpack_button_pressed = true;
    m_startBtn->setEnabled(false);
    m_contBtn->setEnabled(true);
    DbgCmdExec("erun");
}
void MumaUnpackerWidget::onContinueUnpackingClicked()
{
    char cmd[20];
    #ifdef _WIN64
        sprintf(cmd, "set cip,%llx", old_cip);
    #else
        sprintf(cmd, "set cip,%x", old_cip);
    #endif
    DbgCmdExec(cmd);
    DbgCmdExec("erun");
}

void MumaUnpackerWidget::onStopUnpackedClicked()
{
    DbgCmdExec("stop"); 
}

void MumaUnpackerWidget::onDumpUnpackedClicked()
{
    DbgCmdExec("Scylla");
}

void MumaUnpackerWidget::onOpenFileClicked()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "Open file for debugging",
        QString(),
        "Executables (*.exe *.dll *.sys);;All files (*.*)"
    );

    if (!fileName.isEmpty())
    {
        QString escaped = fileName;
        escaped.replace("\\", "\\\\");

        QString cmd = "init \"" + escaped + "\"";
        DbgCmdExec(cmd.toUtf8().constData());
        is_open_file_button_pressed = true;
        GuiDisplayWarning("MALWARE WARNING!", "NEVER unpack walware on yout host OS!\nUse VM instead!\n\nIf you are using your host OS you should close debugger now");
    }
}

void MumaUnpackerWidget::onCheckboxChanged(int state)
{
    QCheckBox* cb = qobject_cast<QCheckBox*>(sender());
    if (!cb) return;

    for (int i = 0; i < functionsCount; i++)
    {
        if (m_checkboxes[i] == cb)
        {
            functions[i].current_state = (state == Qt::Checked);
            if (state == Qt::Checked)
            {
                SetBreakOnFunction(i);
            }
            else
            {
                UnSetBreakOnFunction(i);
            }
            break;
        }
    }
}

void MumaUnpackerWidget::onSelectAllClicked()
{
    for (int i = 0; i < functionsCount; i++)
    {
        functions[i].current_state = true;
        m_checkboxes[i]->setChecked(true);
    }
    SetAllBreaks();
}
void MumaUnpackerWidget::onClearAllClicked()
{
    for (int i = 0; i < functionsCount; i++)
    {
        functions[i].current_state = false;
        m_checkboxes[i]->setChecked(false);
    }
    DeleteAllBreaks();
}