#include "engine.hpp"
#include "displayer.hpp"
#include "input.hpp"
#include "renderer.hpp"
#include "util.hpp"
#include "shader.hpp"
Engine::Engine()
{
}

Engine::~Engine()
{
}

void Engine::startup()
{
    scene = std::make_shared<Scene>();
    displayer = std::make_unique<Displayer>();
    soft_renderer = std::make_unique<SoftRenderer>(scene);
    input_processor = std::make_unique<InputProcessor>(scene);
}

void Engine::shutdown()
{
}
static void update_pbr_shader(PBRShader& pbr_shader,const Scene& scene){
    pbr_shader.view = scene.getCamera()->getViewMatrix();
    pbr_shader.projection = scene.getCamera()->getProjMatrix();
    pbr_shader.viewPos = scene.getCamera()->position;
    {
        const auto &lights = scene.getLights();
        pbr_shader.lightNum = std::min(PBRShader::MaxLightNum, (int)lights.size());
        for (int i = 0; i < pbr_shader.lightNum; i++)
        {
            pbr_shader.lightPos[i] = lights[i].light_position;
            pbr_shader.lightRadiance[i] = lights[i].light_radiance;
        }
    }
}
static void update_sky_shader(SkyShader& sky_shader,const Scene& scene){
    sky_shader.view = scene.getCamera()->getViewMatrix();
    sky_shader.projection = scene.getCamera()->getProjMatrix();
}
static void update_pbr_shader(PBRShader& pbr_shader,const Model& model){
    pbr_shader.model = model.getModelMatrix();
    pbr_shader.MVPMatrix = pbr_shader.projection * pbr_shader.view * pbr_shader.model;
    pbr_shader.albedoMap = model.getAlbedoMap();
    pbr_shader.normalMap = model.getNormalMap();
    pbr_shader.aoMap = model.getAOMap();
    pbr_shader.roughnessMap = model.getRoughnessMap();
    pbr_shader.metallicMap = model.getMetallicMap();
}
static void update_sky_shader(SkyShader& sky_shader,const Model& model){
    sky_shader.envMap = model.getEnvironmentMap().get();
    sky_shader.model = model.getModelMatrix();

}
void Engine::run()
{
    bool exit = false;

    uint32_t delta_t = 0;
    uint32_t last_t = 0;
    while (!exit)
    {
        static PBRShader pbr_shader;
        static SkyShader sky_shader;
        update_pbr_shader(pbr_shader,*scene);
        update_sky_shader(sky_shader,*scene);

        last_t = SDL_GetTicks();

        input_processor->processInput(exit, delta_t);

//        START_TIMER
        soft_renderer->clearFrameBuffer();
//        STOP_TIMER("clear framebuffer")

//        START_TIMER
        auto models = scene->getVisibleModels();
        for(auto model:models){
            update_pbr_shader(pbr_shader,*model);
            soft_renderer->render(pbr_shader,*model,true);
        }
        auto sky_box = scene->getSkyBox();
        if(sky_box){
            //cube's vertex behind view point if perform mvp transform will cause error
            //opengl will clip these primitive and reconstruct them
            //this algorithm should be hard for me
            //So I just use sphere to replace a cube with enough small triangle
            update_sky_shader(sky_shader,*sky_box);
            soft_renderer->render(sky_shader,*sky_box);
            std::cout<<"render sky box"<<std::endl;
        }
//        soft_renderer->render();
//        STOP_TIMER("render a frame")

//        START_TIMER
        displayer->draw(soft_renderer->getImage());
//        STOP_TIMER("draw frame")

        delta_t = SDL_GetTicks() - last_t;
    }
}
