/*

uniform vec3 iResolution;
uniform float iTime;
uniform float iTimeDelta;
uniform float iFrame;
uniform float iChannelTime[4];
uniform vec4 iMouse;
uniform vec4 iDate;
uniform float iSampleRate;
uniform vec3 iChannelResolution[4];
uniform samplerXX iChanneli;
*/

#include <CQShaderToy.h>

#include <QApplication>
#include <QGLWidget>
#include <QOpenGLShaderProgram>
#include <QTabWidget>
#include <QSplitter>
#include <QHBoxLayout>
#include <QTimer>
#include <QFile>

#include <cmath>
#include <iostream>

static int canvasWidth  = 1024;
static int canvasHeight = 1024;

//---

int
main(int argc, char **argv)
{
  QApplication app(argc, argv);

  std::vector<std::string> filenames;

  for (int i = 1; i < argc; ++i) {
    if (argv[i][0] == '-') {
      auto arg = std::string(&argv[i][1]);

      if (arg == "h" || arg == "help") {
        std::cerr << "CQShaderToy <fragment_shader>\n";
        exit(0);
      }
      else {
        std::cerr << "Invalid arg '-" << arg << "'\n";
      }
    }
    else
      filenames.push_back(argv[i]);
  }

  auto *window = new Window;

  for (auto &filename : filenames) {
    QFile file(QString::fromStdString(filename));

    if (! file.open(QIODevice::ReadOnly | QIODevice::Text))
      continue;

    auto str = QString(file.readAll());

    window->setFragmentShader(str);
  }

  window->resize(int(1.8*canvasWidth), canvasHeight);

  window->show();

  app.exec();
}

//---

Window::
Window(QWidget *parent) :
 QFrame(parent)
{
  auto *layout = new QVBoxLayout(this);

  auto *frame = new QSplitter(Qt::Vertical);

  layout->addWidget(frame);

  //---

  auto *midFrame = new QSplitter(Qt::Horizontal);

  frame->addWidget(midFrame);

  //---

  canvas_ = new Canvas(this);

  midFrame->addWidget(canvas_);

  //---

  auto *tab = new QTabWidget;

  fragmentEditor_ = new Editor(this);
  vertexEditor_   = new Editor(this);

  fragmentEditor_->setPlainText("\
void mainImage(out vec4 fragColor, in vec2 fragCoord) {\n\
  vec2 uv = fragCoord/iResolution;\n\
  fragColor = vec4(uv.x, uv.y, 0.0, 1.0);\n\
}\n\
");

  vertexEditor_->setPlainText("\
attribute vec4 a_Position;\n\
attribute vec2 a_Coordinates;\n\
\n\
void main() {\n\
  gl_Position = vec4(a_Coordinates.x, a_Coordinates.y, 1.0f, 1.0f);\n\
}");

  fragmentEditor_->init();
  vertexEditor_  ->init();

  connect(fragmentEditor_, SIGNAL(editChanged()), this, SLOT(fragmentSlot()));
  connect(vertexEditor_  , SIGNAL(editChanged()), this, SLOT(vertexSlot()));

  tab->addTab(fragmentEditor_, "Fragment");
  tab->addTab(vertexEditor_  , "Vertex");

  midFrame->addWidget(tab);

  //---

  errorText_ = new QTextEdit;

  frame->addWidget(errorText_);
}

QString
Window::
fragmentShader() const
{
  auto str = QString("\
#version 330\n\
\n\
uniform float iTime;\n\
uniform int   iFrame;\n\
uniform vec2  iResolution;\n\
\n");

  str += fragmentEditor_->toPlainText();

str += QString("\n\
void main() {\n\
  mainImage(gl_FragColor, gl_FragCoord.xy);\n\
}\n");

  return str;
}

void
Window::
setFragmentShader(const QString &str)
{
  fragmentEditor_->setText(str);
}

QString
Window::
vertexShader() const
{
  auto str = QString("\
#version 330\n\
\n");

  str += vertexEditor_->toPlainText();

  return str;
}

void
Window::
setErrorText(const QString &str)
{
  errorText_->setText(str);
}

void
Window::
fragmentSlot()
{
  canvas_->reloadShaders();
}

void
Window::
vertexSlot()
{
  canvas_->reloadShaders();
}

//---

Editor::
Editor(Window *window) :
 window_(window)
{
}

void
Editor::
init()
{
  textTimer_ = new QTimer(this);

  textTimer_->setSingleShot(true);

  connect(textTimer_, SIGNAL(timeout()), this, SLOT(updateTextSlot()));

  connect(this, SIGNAL(textChanged()), this, SLOT(textChangedSlot()));
}

void
Editor::
textChangedSlot()
{
  textTimer_->start(textTimeOut_);
}

void
Editor::
updateTextSlot()
{
  Q_EMIT(editChanged());
}

//---

Canvas::
Canvas(Window *window) :
 QGLWidget(window), window_(window)
{
  redrawTimer_ = new QTimer(this);

  connect(redrawTimer_, SIGNAL(timeout()), this, SLOT(redrawSlot()));

  redrawTimer_->start(redrawTimeOut_);
}

void
Canvas::
initializeGL()
{
  glClearColor(0.0, 0.0, 0.0, 0.0);

  reloadShaders();
}

void
Canvas::
redrawSlot()
{
  elapsed_ += redrawTimeOut_/1000.0;

  ++frame_;

  updateGL();
}

void
Canvas::
reloadShaders()
{
  //std::cerr << "reloadShaders:";
  //std::cerr << "  Vertex: " << window_->vertexShader().toStdString() << "\n";
  //std::cerr << "  Fragment: " << window_->fragmentShader().toStdString() << "\n";

//delete program_; program_ = nullptr;

  if (! program_)
    program_ = new QOpenGLShaderProgram;

  QString errorText;

  program_->removeAllShaders();

  bool rc = true;

  if (! program_->addShaderFromSourceCode(QOpenGLShader::Vertex, window_->vertexShader()))
    rc = false;

  if (! program_->addShaderFromSourceCode(QOpenGLShader::Fragment, window_->fragmentShader()))
    rc = false;

  if (! program_->link())
    rc = false;

  if (! rc)
    errorText += program_->log() + "\n";

  window_->setErrorText(errorText);

  updateGL();
}

void
Canvas::
paintGL()
{
  glClear(GL_COLOR_BUFFER_BIT);

  program_->bind();

  //---

  int elapsedLocation = program_->uniformLocation("iTime");
  program_->setUniformValue(elapsedLocation, GLfloat(elapsed_));

  //---

  int frameLocation = program_->uniformLocation("iFrame");
  program_->setUniformValue(frameLocation, GLint(frame_));

  //---

  int resLocation = program_->uniformLocation("iResolution");

  program_->setUniformValue(resLocation, GLfloat(width()), GLfloat(height()));

  //---

  QVector3D vertices[] = {
    QVector3D(-1.0f, -1.0f, 1.0f),
    QVector3D( 1.0f, -1.0f, 1.0f),
    QVector3D( 1.0f,  1.0f, 1.0f),
    QVector3D( 1.0f,  1.0f, 1.0f),
    QVector3D(-1.0f,  1.0f, 1.0f),
    QVector3D(-1.0f, -1.0f, 1.0f)
  };

  int coordsLocation = program_->attributeLocation("a_Coordinates");

  program_->enableAttributeArray(coordsLocation);

  program_->setAttributeArray(coordsLocation, vertices);

  glDrawArrays(GL_TRIANGLES, 0, 6);

  program_->disableAttributeArray(coordsLocation);

  program_->release();
}

void
Canvas::
resizeGL(int width, int height)
{
  glViewport(0, 0, width, height);
}

QSize
Canvas::
sizeHint() const
{
  return QSize(canvasWidth, canvasHeight);
}
