
#include "main.h"

const float gridScale = 0.02f;

void Vizzer::init(ml::ApplicationData &app)
{
    m_square.load(app.graphics, ml::TriMeshf(ml::shapes::plane(vec3f::origin, vec3f(gridScale, gridScale, 0.0f), vec3f::eZ)));
	
    m_vsColor.load(app.graphics, "shaders/test.shader");
    m_psColor.load(app.graphics, "shaders/test.shader");

    m_constants.init(app.graphics);

    ml::vec3f eye(-0.5f, -0.5f, 1.5f);
    ml::vec3f worldUp(0.0f, 0.0f, 1.0f);
    m_camera = ml::Cameraf(eye, worldUp, ml::vec3f::eX, 60.0f, (float)app.window.width() / app.window.height(), 0.01f, 1000.0f);

    m_font.init(app.graphics, "Calibri");

    fluid.init();
}

void Vizzer::render(ml::ApplicationData &app)
{
    auto &g = app.graphics.castD3D11();

    m_timer.frame();

    m_vsColor.bind(app.graphics);
    m_psColor.bind(app.graphics);

    //fluid.step();

    for (UINT y = 0; y < fluid.gridSize; y++)
    {
        for (UINT x = 0; x < fluid.gridSize; x++)
        {
            mat4f transform = mat4f::translation(vec3f(x * gridScale, y * gridScale, 0.0f));

            ConstantBuffer constants;
            constants.worldViewProj = transform * m_camera.cameraPerspective();
            m_constants.update(app.graphics, constants);
            m_constants.bindVertexShader(app.graphics, 0);

            m_square.render(g);
        }
    }

    m_font.drawString(app.graphics, "FPS: " + ml::convert::toString(m_timer.framesPerSecond()), ml::vec2i(10, 5), 24.0f, ml::RGBColor::Red);
}

void Vizzer::resize(ml::ApplicationData &app)
{
    m_camera.updateAspectRatio((float)app.window.width() / app.window.height());
}

void Vizzer::keyDown(ml::ApplicationData &app, UINT key)
{

}

void Vizzer::keyPressed(ml::ApplicationData &app, UINT key)
{
    const float distance = 0.1f;
    const float theta = 5.0f;

    if(key == KEY_S) m_camera.move(-distance);
    if(key == KEY_W) m_camera.move(distance);
    if(key == KEY_A) m_camera.strafe(-distance);
    if(key == KEY_D) m_camera.strafe(distance);
	if(key == KEY_E) m_camera.jump(distance);
	if(key == KEY_Q) m_camera.jump(-distance);

    if(key == KEY_UP) m_camera.lookUp(theta);
    if(key == KEY_DOWN) m_camera.lookUp(-theta);
    if(key == KEY_LEFT) m_camera.lookRight(theta);
    if(key == KEY_RIGHT) m_camera.lookRight(-theta);
}

void Vizzer::mouseDown(ml::ApplicationData &app, ml::MouseButtonType button)
{

}

void Vizzer::mouseWheel(ml::ApplicationData &app, int wheelDelta)
{
    const float distance = 0.002f;
    m_camera.move(distance * wheelDelta);
}

void Vizzer::mouseMove(ml::ApplicationData &app)
{
    const float distance = 0.01f;
    const float theta = 0.5f;

    ml::vec2i posDelta = app.input.mouse.pos - app.input.prevMouse.pos;

    if(app.input.mouse.buttons[ml::MouseButtonRight])
    {
        m_camera.strafe(-distance * posDelta.x);
        m_camera.jump(distance * posDelta.y);
    }

    if(app.input.mouse.buttons[ml::MouseButtonLeft])
    {
        m_camera.lookRight(-theta * posDelta.x);
        m_camera.lookUp(theta * posDelta.y);
    }

}