// (c) by Stefan Roettger, licensed under GPL 2+

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#ifdef HAVE_QT5
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QTableWidget>
#else
#include <QtGui/QMainWindow>
#include <QtGui/QTableWidget>
#endif

#include <mini/mini_generic.h>
#include <mini/mini_object.h>

class Object;
class ViewerWindow;

class QMenu;
class QAction;
class QTabWidget;
class QGroupBox;
class QBoxLayout;
class QVBoxLayout;
class QHBoxLayout;
class QSplitter;
class QLabel;
class QCheckBox;
class QRadioButton;
class QButtonGroup;
class QSlider;
class QDialogButtonBox;
class QPushButton;
class QLineEdit;

class MyQTableWidget;

//! qt viewer main window that
//! contains the viewer window with the opengl rendering context and
//! contains the qt gui elements that interact with the renderer
class MainWindow: public QMainWindow
{
   Q_OBJECT;

public:
   MainWindow(QWidget *parent = 0);
   virtual ~MainWindow();

public slots:
   void receiveChange(const ministring &action, const ministring &value);
   void updateTable(const ministring &action, const ministring &key);

   void runAction(const ministring &action, int row = -1);
   void receiveProgress(double percentage, const ministring &job, unsigned int jobs);

private slots:
   void about();
   void open();
   void clear(bool all = false);

   void toggleStereo(int);
   void toggleWireFrame(int);

   void click(int, int);
   void doubleclick(int, int);
   void checkFog(int);
   void setFogDensity(int);
   void checkContours(int);
   void checkSeaLevel(int);
   void setSeaLevel(int);
   void checkLight(int);
   void setLight(int);
   void checkExagger(int);
   void setExagger(int);

   void filterDEMToggled(bool);
   void filterIMGToggled(bool);
   void filterALLToggled(bool);

   void repoPathChanged(QString);
   void exportPathChanged(QString);
   void tmpPathChanged(QString);

   void browseRepoPath();
   void browseExportPath();
   void browseTmpPath();

   void splitMetersChanged(QString);

   void gridLevelChanged(QString);
   void gridLevelsChanged(QString);
   void gridStepChanged(QString);

   void shadePowerChanged(QString);
   void shadeAmbientChanged(QString);
   void jpegQualityChanged(QString);

   void contourSpacingChanged(QString);
   void contourThicknessChanged(QString);
   void contourBorderChanged(QString);

   void blackLevelChanged(QString);
   void whiteLevelChanged(QString);
   void contrastLinearityChanged(QString);

   void redGammaChanged(QString);
   void greenGammaChanged(QString);
   void blueGammaChanged(QString);

   void checkSliders(int);

private:
   void createActions();
   void createMenus();
   void createWidgets();

   QSlider *createSlider(int minimum, int maximum, int value);

   QGroupBox *createEdit(ministring name, ministring value,
                         QLineEdit **lineEdit);

   QGroupBox *createPathEdit(ministring name, ministring value,
                             QLineEdit **lineEdit, QPushButton **browseButton);

   QMenu *fileMenu;
   QMenu *helpMenu;

   QAction *aboutAction;
   QAction *openAction;
   QAction *clearAction;
   QAction *quitAction;

   QGroupBox *mainGroup;
   QBoxLayout *mainLayout;
   QSplitter *mainSplitter;

   ViewerWindow *viewerWindow;
   QTabWidget *tabWidget;
   QDialogButtonBox *buttonBox;

   QGroupBox *viewerGroup;
   QBoxLayout *viewerLayout;

   QGroupBox *prefGroup;
   QVBoxLayout *prefLayout;

   QGroupBox *paramGroup;
   QVBoxLayout *paramLayout;

   QPushButton *openButton;
   QPushButton *clearButton;
   QPushButton *quitButton;

   MyQTableWidget *viewerTable;
   QBoxLayout *tableBox;
   QGroupBox *sliderBox;
   QBoxLayout *sliderLayout;

   QButtonGroup *tableFilter;
   QLabel *workerActivity;

   QRadioButton *filter1;
   QRadioButton *filter2;
   QRadioButton *filter3;

   QGroupBox *fogGroup;
   QHBoxLayout *fogLayout;
   QCheckBox *fogCheck;
   QSlider *fogDensitySlider;

   QGroupBox *contourGroup;
   QHBoxLayout *contourLayout;
   QCheckBox *contourCheck;

   QGroupBox *seaGroup;
   QHBoxLayout *seaLayout;
   QCheckBox *seaLevelCheck;
   QSlider *seaLevelSlider;

   QGroupBox *lightGroup;
   QHBoxLayout *lightLayout;
   QCheckBox *lightCheck;
   QSlider *lightSlider;

   QGroupBox *exaggerGroup;
   QHBoxLayout *exaggerLayout;
   QCheckBox *exaggerCheck;
   QSlider *exaggerSlider;

   QGroupBox *stereoGroup;
   QHBoxLayout *stereoLayout;
   QCheckBox *stereoCheck;

   QGroupBox *wireGroup;
   QHBoxLayout *wireLayout;
   QCheckBox *wireFrameCheck;

   QLineEdit *lineEdit_repoPath;
   QPushButton *browseButton_repoPath;

   QLineEdit *lineEdit_exportPath;
   QPushButton *browseButton_exportPath;

   QLineEdit *lineEdit_tmpPath;
   QPushButton *browseButton_tmpPath;

   QLineEdit *lineEdit_splitMeters;

   QLineEdit *lineEdit_gridLevel;
   QLineEdit *lineEdit_gridLevels;
   QLineEdit *lineEdit_gridStep;

   QLineEdit *lineEdit_shadePower;
   QLineEdit *lineEdit_shadeAmbient;
   QLineEdit *lineEdit_jpegQuality;

   QLineEdit *lineEdit_contourSpacing;
   QLineEdit *lineEdit_contourThickness;
   QLineEdit *lineEdit_contourBorder;

   QLineEdit *lineEdit_blackLevel;
   QLineEdit *lineEdit_whiteLevel;
   QLineEdit *lineEdit_contrastLinearity;

   QLineEdit *lineEdit_redGamma;
   QLineEdit *lineEdit_greenGamma;
   QLineEdit *lineEdit_blueGamma;

   QCheckBox *sliderButton;

   void getNameInfo(Object *obj,
                    QString &name, QString &info);

protected:
   ministrings m_Keys;

   ministring repoPath;
   ministring exportPath;
   ministring tmpPath;

   double split_meters;

   int grid_level;
   int grid_levels;
   int grid_step;

   double shadePower;
   double shadeAmbient;
   double jpegQuality;

   double contourSpacing;
   double contourThickness;
   double contourBorder;

   double blackLevel;
   double whiteLevel;
   double contrastLinearity;

   double redGamma;
   double greenGamma;
   double blueGamma;

   void initSettings();
   void saveSettings();

   void keyPressEvent(QKeyEvent *event);
   void keyReleaseEvent(QKeyEvent *event);
};

// subclass table widget
class MyQTableWidget: public QTableWidget
{
   Q_OBJECT

public:
   MyQTableWidget(ViewerWindow *viewer, QWidget *parent = 0)
      : QTableWidget(parent)
   {
      viewerWindow = viewer;
   }

   QSize minimumSizeHint() const;
   QSize sizeHint() const;

protected:
   void keyPressEvent(QKeyEvent *event);
   void mousePressEvent(QMouseEvent *event);

private slots:
   void showContextMenu(const QPoint &pos);

signals:
   void activate(const ministring &action, int row = -1);

protected:
   ViewerWindow *viewerWindow;

   void dragEnterEvent(QDragEnterEvent *event);
   void dragMoveEvent(QDragMoveEvent *event);
   void dropEvent(QDropEvent *event);
   void dragLeaveEvent(QDragLeaveEvent *event);
};

#endif
