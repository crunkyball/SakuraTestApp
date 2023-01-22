/*************************************************************
Sakura Game Framework : TestAppGame.h
Author: Darrell Blake
*************************************************************/

#ifndef _TEST_APP_GAME_H
#define _TEST_APP_GAME_H

#define MODEL_TEST

#include <Sakura/SaCore.h>
#include <Sakura/SaGame.h>
#include <Sakura/SaCamera.h>
#include <Sakura/SaLight.h>
#include <Sakura/SaInputComponentFreeCam.h>
#include <Sakura/SaSprite.h>
#include <SakuraThread/SaThread.h>
#include <SakuraThread/SaMutex.h>
#include <SakuraThread/SaSemaphore.h>
#include <Sakura/SaResource.h>

using namespace Sakura;
using namespace SakuraThread;

namespace Sakura
{
    class SaTexture;
    class SaEffect;
    class SaModel;
}

class TestAppGame : public SaGame
{
public:
    TestAppGame(SaGameProperties& rProperties);
    ~TestAppGame();

    void Initialise();
    void Release();

    void Update(uint32_t dt);
    void Render3D();
    void Render2D();

    static void TestThreadFunc1(void* pUserData);
    static void TestThreadFunc2(void* pUserData);

private:
    SaInputComponentFreeCam m_freeCamInput;
    SaCamera m_camera;
    SaCamera m_camera2;
    SaLight m_light[2];

#ifndef MODEL_TEST
    void RenderThread_CustomRenderInit(const SaRenderJob* pJob);
    void RenderThread_CustomRender(const SaRenderJob* pJob);
    void RenderThread_CustomRenderTri(const SaRenderJob* pJob);
    void RenderThread_CustomRenderCube(const SaRenderJob* pJob);

    SaResource<SaTexture>* m_pTestTriTexture;
    SaResource<SaTexture>* m_pTest2DTexture;
    //SaResource<SaTexture>* m_pWhiteTexture;
    SaResource<SaEffect>* m_pBasicEffect;
    SaResource<SaEffect>* m_pSpriteEffect;
    SaResource<SaTexture>* m_pBrickTexture;

    SaSprite m_testSprite;

    uint32_t m_vao, m_vbo[3];
    uint32_t m_cubeVBO[2];
    float m_triRot;

    static const uint32_t BRICK_X_NUM = 6;
    static const uint32_t BRICK_Y_NUM = 12;
    SaSprite m_brickArray[BRICK_X_NUM][BRICK_Y_NUM];
#else
    SaResource<SaEffect>* m_pBasicModelEffect;
    SaResource<SaModel>* m_pTestModel;
#endif
};

#endif  //_TEST_APP_GAME_H
