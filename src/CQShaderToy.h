#ifndef CQShaderToy_H
#define CQShaderToy_H

#include <QFrame>
#include <QTextEdit>
#include <QGLWidget>

class Window;

class QOpenGLShaderProgram;
class QTimer;

class Canvas : public QGLWidget {
  Q_OBJECT

 public:
  Canvas(Window *window);

  void initializeGL() override;

  void paintGL() override;

  void resizeGL(int, int) override;

  void reloadShaders();

  QSize sizeHint() const override;

 private Q_SLOTS:
  void redrawSlot();

 private:
  QOpenGLShaderProgram *program_ { nullptr };

 private:
  Window* window_        { nullptr };
  QTimer* redrawTimer_   { nullptr };
  int     redrawTimeOut_ { 100 };
  double  elapsed_       { 0.0 };
  int     frame_         { 0 };
};

class Editor : public QTextEdit {
  Q_OBJECT

 public:
  Editor(Window *window);

  void init();

 private Q_SLOTS:
  void textChangedSlot();
  void updateTextSlot();

 Q_SIGNALS:
  void editChanged();

 private:
  Window* window_        { nullptr };
  QTimer* textTimer_     { nullptr };
  int     textTimeOut_   { 500 };
};

class Window : public QFrame {
  Q_OBJECT

 public:
  Window(QWidget *parent=nullptr);

  QString fragmentShader() const;
  void setFragmentShader(const QString &str);

  QString vertexShader() const;

  void setErrorText(const QString &str);

 private Q_SLOTS:
  void fragmentSlot();
  void vertexSlot();

 private:
  Canvas*    canvas_         { nullptr };
  Editor*    fragmentEditor_ { nullptr };
  Editor*    vertexEditor_   { nullptr };
  QTextEdit* errorText_      { nullptr };
};

#endif
