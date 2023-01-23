/*************************************************************
Sakura Game Framework : TestAppGame.cpp
Author: Darrell Blake
*************************************************************/

#include "TestAppGame.h"

#include <SakuraMaths/SaMatrix44.h>
#include <SakuraMaths/SaVector2.h>
#include <Sakura/SaDebugDraw.h>
#include <Sakura/SaTexture.h>
#include <Sakura/SaResourceManager.h>
#include <Sakura/SaRenderState.h>
#include <Sakura/SaRand.h>
#include <Sakura/SaTime.h>
#include <Sakura/SaModel.h>
#include <Sakura/SaFile.h>
#include <Sakura/SaRender.h>
#include <Sakura/SaHashTable.h>
#include <Sakura/SaInputManager.h>

#include <cstdio>

using namespace SakuraMaths;

TestAppGame::TestAppGame(SaGameProperties& rProperties) : 
    SaGame(rProperties),
#ifndef MODEL_TEST
    m_pTestTriTexture(NULL),
    m_pTest2DTexture(NULL),
    //m_pWhiteTexture(NULL),
    m_pBasicEffect(NULL),
    m_pSpriteEffect(NULL),
    m_pBrickTexture(NULL),
    m_triRot(0.0f)
#else
    m_pBasicModelEffect(NULL),
    m_pTestModel(NULL)
#endif
{
}

TestAppGame::~TestAppGame()
{
}

float triVerts[9] =
{
    0.0f, 0.9f, 0.0f,
    -0.9f, -0.9f, 0.0f,
    0.9f, -0.9f, 0.0f
};

float triColors[9] =
{
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f
};

float triTexCoords[6] =
{
    0.5f, 0.0f,
    0.0f, 1.0f,
    1.0f, 1.0f
};

uint32_t triIndices[3] =
{
    0, 1, 2
};

float cubeSize = 0.7f;

float cubeVerts[24] = 
{
    //Back top right
    -cubeSize, cubeSize, cubeSize,

    //Back top left
    cubeSize, cubeSize, cubeSize,

    //Back bottom right
    -cubeSize, -cubeSize, cubeSize,

    //Back bottom left
    cubeSize, -cubeSize, cubeSize,

    //Front top right
    -cubeSize, cubeSize, -cubeSize,

    //Front top left
    cubeSize, cubeSize, -cubeSize,

    //Front bottom right
    -cubeSize, -cubeSize, -cubeSize,

    //Front bottom left
    cubeSize, -cubeSize, -cubeSize
};

float cubeColours[24] = 
{
    //Back top right
    1.0f, 0.0f, 0.0f,   //0

    //Back top left
    0.0f, 1.0f, 0.0f,   //1

    //Back bottom right
    0.0f, 0.0f, 1.0f,   //2

    //Back bottom left
    1.0f, 0.0f, 0.0f,   //3

    //Front top right
    0.0f, 1.0f, 0.0f,   //4

    //Front top left
    0.0f, 0.0f, 1.0f,   //5

    //Front bottom right
    1.0f, 0.0f, 0.0f,   //6

    //Front bottom left
    0.0f, 1.0f, 0.0f    //7
};

uint32_t cubeIndices[36] = 
{
    //Back
    0, 2, 1, 3, 1, 2,

    //Front
    4, 5, 6, 7, 6, 5,

    //Top
    4, 0, 5, 0, 1, 5,

    //Bottom
    2, 6, 7, 3, 2, 7,

    //Right
    0, 4, 6, 2, 0, 6,

    //Left
    1, 7, 5, 1, 3, 7
};

void TestAppGame::Initialise()
{
    SaGame::Initialise();

#ifndef MODEL_TEST
    SaRenderJob job(BIND_MEMBER_FUNC(&TestAppGame::RenderThread_CustomRenderInit, this));
    SaRender::AddRenderJob(job);

    m_pTestTriTexture = SaResourceManager::LoadTexture("Lenna.png");
    m_pTest2DTexture = SaResourceManager::LoadTexture("Lenna.png");
    //m_pWhiteTexture = SaResourceManager::LoadTexture("White.png");

    m_pBasicEffect = SaResourceManager::LoadEffect("SaBasicEffect");
    m_pSpriteEffect = SaResourceManager::LoadEffect("SaSpriteEffect");

    m_testSprite.Create(m_pTest2DTexture->GetPtr(), m_pSpriteEffect->GetPtr());
    m_testSprite.SetPosition(SaVector2(100.0f, 100.0f));
    m_testSprite.SetScale(SaVector2(0.5f, 0.5f));

    m_pBrickTexture = SaResourceManager::LoadTexture("Brick.png");

    SaRand rand(SaTime::GetTimeMilliseconds());

    for (uint32_t i = 0; i < 100; i++)
    {
        SA_PRINTF("%u\n", rand.GetUint32(0, 100));
    }

    for (uint32_t x = 0; x < BRICK_X_NUM; x++)
    {
        for (uint32_t y = 0; y < BRICK_Y_NUM; y++)
        {
            m_brickArray[x][y].Create(m_pBrickTexture->GetPtr(), m_pSpriteEffect->GetPtr());
            m_brickArray[x][y].SetPosition(SaVector2(500.0f + (32.0f * x), 100.0f + (32.0f * y)));
            m_brickArray[x][y].SetColour(SaColour(rand.GetFloat(), rand.GetFloat(), rand.GetFloat(), 1.0f));
        }
    }
#else
    m_pBasicModelEffect = SaResourceManager::LoadEffect("SaBasicModelEffect");
    m_pTestModel = SaResourceManager::LoadModel("Suzanne.sak");
#endif

    uint32_t width = GetGameProperties().m_xResolution;
    uint32_t height = GetGameProperties().m_yResolution;

    static const uint32_t numViewports = 2;
    uint32_t viewportWidth = width / numViewports;

    float aspectRatio = static_cast<float>(viewportWidth) / height;

    m_camera.Initialise(aspectRatio);
    m_camera.SetInputComponent(&m_freeCamInput);
    //Shift the camera up a bit initially.
    m_camera.SetPosition(m_camera.GetPosition() + SaVector3::Y_AXIS * 1.5f);
    m_camera.SetPosition(m_camera.GetPosition() - SaVector3::Z_AXIS * 7.0f);

    m_camera2.Initialise(aspectRatio);
    m_camera2.SetPosition(m_camera2.GetPosition() + SaVector3::Y_AXIS * 2.0f);
    m_camera2.SetPosition(m_camera2.GetPosition() - SaVector3::Z_AXIS * 5.0f);

    SaWindow::ViewportConfig viewportConfigs[numViewports];

    viewportConfigs[0].xPosition = 0;
    viewportConfigs[0].yPosition = 0;
    viewportConfigs[0].width = viewportWidth-1;
    viewportConfigs[0].height = height;

    viewportConfigs[1].xPosition = viewportWidth+1;
    viewportConfigs[1].yPosition = 0;
    viewportConfigs[1].width = viewportWidth-1;
    viewportConfigs[1].height = height;

    GetWindow().SetViewports(numViewports, viewportConfigs);

    GetWindow().GetViewport(0).AttachCamera(&m_camera);
    GetWindow().GetViewport(1).AttachCamera(&m_camera2);

    m_light[0].Initialise(SaColour(0.95f, 0.95f, 0.95f, 1.0f));
    m_light[0].SetPosition(SaVector3(0.0f, 3.0f, -3.0f));

    m_light[1].Initialise(SaColour(0.3f, 0.95f, 0.3f, 1.0f));
    m_light[1].SetPosition(SaVector3(0.0f, -1.0f, 3.0f));
}

void TestAppGame::Release()
{
#ifndef MODEL_TEST
    if (m_pTestTriTexture)
        SaResourceManager::UnloadTexture(m_pTestTriTexture);
    if (m_pTest2DTexture)
        SaResourceManager::UnloadTexture(m_pTest2DTexture);
    /*if (m_pWhiteTexture)
        SaResourceManager::UnloadTexture(m_pWhiteTexture);*/
    if (m_pBasicEffect)
        SaResourceManager::UnloadEffect(m_pBasicEffect);
    if (m_pSpriteEffect)
        SaResourceManager::UnloadEffect(m_pSpriteEffect);
    if (m_pBrickTexture)
        SaResourceManager::UnloadTexture(m_pBrickTexture);

    glDeleteBuffers(3, m_vbo);
    glDeleteBuffers(2, m_cubeVBO);
    //glDeleteVertexArrays(1, &m_vao);  //Not used in GLES.
#else
    if (m_pBasicModelEffect)
        SaResourceManager::UnloadEffect(m_pBasicModelEffect);
    if (m_pTestModel)
        SaResourceManager::UnloadModel(m_pTestModel);
#endif

    SaGame::Release();
}

void TestAppGame::Update(uint32_t dt)
{
    SaGame::Update(dt);

    m_camera.Update(dt);
    m_camera2.Update(dt);
    m_light[0].Update(dt);
    m_light[1].Update(dt);

#ifndef MODEL_TEST
    float speed = 0.001f;
    m_triRot += speed * dt;
#else
    SaMatrix33 mat = SaMatrix33::CreateRotation(SaVector3::Y_AXIS, 0.001f * dt);
    m_light[0].SetPosition(mat * m_light[0].GetPosition());

    mat = SaMatrix33::CreateRotation(-SaVector3::Y_AXIS, 0.003f * dt);
    m_light[1].SetPosition(mat * m_light[1].GetPosition());
#endif

    const SaKeyboard& rKeyboard = SaInputManager::GetKeyboard();

    if (rKeyboard.IsKeyDown(SA_KEY_ESCAPE))
        Quit();
}

void TestAppGame::Render3D()
{
    SaRender::SetAlphaBlending(true);
    SaRender::SetBackFaceCulling(true);

    SaGame::Render3D();

    //Draw temporary grid.
    float fGridExtent = 10.0f;

    SaColour grey(0.9f, 0.9f, 0.9f, 1.0f);

    for (float x = -fGridExtent; x <= fGridExtent; x+=1.0f)
    {
        if (x == 0)
            continue;

        SaVector3 g1(x, 0.0f, -fGridExtent);
        SaVector3 g2(x, 0.0f, fGridExtent);
        SaDebugDraw::DrawLine3D(g1, g2, grey);
    }

    for (float z = -fGridExtent; z <= fGridExtent; z+=1.0f)
    {
        if (z == 0)
            continue;

        SaVector3 g1(-fGridExtent, 0.0f, z);
        SaVector3 g2(fGridExtent, 0.0f, z);
        SaDebugDraw::DrawLine3D(g1, g2, grey);
    }

    SaVector3 cX1(-fGridExtent, 0.0f, 0.0f);
    SaVector3 cX2(fGridExtent, 0.0f, 0.0f);
    SaDebugDraw::DrawLine3D(cX1, cX2, SaColour::RED);

    SaVector3 cZ1(0.0f, 0.0f, -fGridExtent);
    SaVector3 cZ2(0.0f, 0.0f, fGridExtent);
    SaDebugDraw::DrawLine3D(cZ1, cZ2, SaColour::GREEN);

#ifndef MODEL_TEST
    SaRenderJob job0(BIND_MEMBER_FUNC(&TestAppGame::RenderThread_CustomRender, this));
    SaRender::AddRenderFrameJob(job0);

    //Draw temporary triangle.
    SaMatrix44 transMat = SaMatrix44::CreateYRotation(m_triRot);
    transMat = SaMatrix44::CreateTranslation(0.0f, 1.0f, 5.0f) * transMat;

    SaRender::SetBackFaceCulling(false);

    if (m_pBasicEffect)
    {
        m_pBasicEffect->Get().Enable();

        m_pBasicEffect->Get().SetMatrixParameter(VERTEX_SHADER, "modelMatrix", transMat);
        m_pBasicEffect->Get().SetMatrixParameter(VERTEX_SHADER, "viewMatrix", SaRenderState::GetViewMatrix());
        m_pBasicEffect->Get().SetMatrixParameter(VERTEX_SHADER, "projectionMatrix", SaRenderState::GetProjectionMatrix());
        m_pBasicEffect->Get().SetTextureParameter(FRAGMENT_SHADER, "texture", m_pTestTriTexture->GetPtr());

        SaRenderJob job1(BIND_MEMBER_FUNC(&TestAppGame::RenderThread_CustomRenderTri, this));
        SaRender::AddRenderFrameJob(job1);

        m_pBasicEffect->Get().Disable();
    }

    SaRender::SetBackFaceCulling(true);

    //transMat = SaMatrix44::CreateTranslation(-2.0f, 1.0f, 5.0f);
    transMat = SaMatrix44::CreateXRotation(m_triRot * 0.5f);
    transMat = SaMatrix44::CreateYRotation(m_triRot * 0.3f) * transMat;
    transMat = SaMatrix44::CreateTranslation(-2.0f, 1.0f, 5.0f) * transMat;

    //Draw temporary cube.
    if (m_pBasicEffect)
    {
        m_pBasicEffect->Get().Enable();

        m_pBasicEffect->Get().SetMatrixParameter(VERTEX_SHADER, "modelMatrix", transMat);
        m_pBasicEffect->Get().SetMatrixParameter(VERTEX_SHADER, "viewMatrix", SaRenderState::GetViewMatrix());
        m_pBasicEffect->Get().SetMatrixParameter(VERTEX_SHADER, "projectionMatrix", SaRenderState::GetProjectionMatrix());
        //m_pBasicEffect->Get().SetTextureParameter(SaEffect::FRAGMENT_SHADER, "texture", m_pWhiteTexture);

        SaRenderJob job2(BIND_MEMBER_FUNC(&TestAppGame::RenderThread_CustomRenderCube, this));
        SaRender::AddRenderFrameJob(job2);

        m_pBasicEffect->Get().Disable();
    }
#else
    if (m_pBasicModelEffect && m_pTestModel)
    {
        m_pBasicModelEffect->Get().Enable();
        
        SaMatrix44 viewMat = SaRenderState::GetViewMatrix();
        SaMatrix44 projMat = SaRenderState::GetProjectionMatrix();
        
        SaMatrix44 modelMatrix = SaMatrix44::CreateTranslation(0.0f, 1.0f, 0.0f);//SaMatrix44::IDENTITY;
        SaMatrix44 modelViewMatrix = SaRenderState::GetViewMatrix() * modelMatrix;
        SaMatrix44 modelViewProjMatrix = SaRenderState::GetProjectionMatrix() * modelViewMatrix;
        m_pBasicModelEffect->Get().SetMatrixParameter(VERTEX_SHADER, "modelViewProj", modelViewProjMatrix);

        SaMatrix44 invModelMatrix;
        modelMatrix.GetInverse(invModelMatrix);
        
        //Lighting caluclations are done in object space.
        SaVector4 objSpaceCamPos = invModelMatrix * SaVector4(m_camera.GetPosition(), 1.0f);
        m_pBasicModelEffect->Get().SetVectorParameter(FRAGMENT_SHADER, "eyePosition", objSpaceCamPos.GetXYZ());
        
        m_pBasicModelEffect->Get().SetIntParameter(FRAGMENT_SHADER, "numLights", 2);
        
        SaVector4 objSpaceLightPos = invModelMatrix * SaVector4(m_light[0].GetPosition(), 1.0f);
        m_pBasicModelEffect->Get().SetVectorParameter(FRAGMENT_SHADER, "lights[0].position", objSpaceLightPos.GetXYZ());
        m_pBasicModelEffect->Get().SetColourParameter(FRAGMENT_SHADER, "lights[0].colour", m_light[0].GetColour());
        objSpaceLightPos = invModelMatrix * SaVector4(m_light[1].GetPosition(), 1.0f);
        m_pBasicModelEffect->Get().SetVectorParameter(FRAGMENT_SHADER, "lights[1].position", objSpaceLightPos.GetXYZ());
        m_pBasicModelEffect->Get().SetColourParameter(FRAGMENT_SHADER, "lights[1].colour", m_light[1].GetColour());
        
        m_pBasicModelEffect->Get().SetColourParameter(FRAGMENT_SHADER, "globalAmbient", SaColour(0.1f, 0.1f, 0.1f, 1.0f));
        m_pBasicModelEffect->Get().SetColourParameter(FRAGMENT_SHADER, "material.emissive", SaColour::BLACK);
        m_pBasicModelEffect->Get().SetColourParameter(FRAGMENT_SHADER, "material.ambient", SaColour(0.33f, 0.22f, 0.03f, 1.0f));
        m_pBasicModelEffect->Get().SetColourParameter(FRAGMENT_SHADER, "material.diffuse", /*SaColour(0.78f, 0.57f, 0.11f, 1.0f));*/m_pTestModel->Get().GetMaterialColour());
        m_pBasicModelEffect->Get().SetColourParameter(FRAGMENT_SHADER, "material.specular", SaColour(0.99f, 0.91f, 0.81f, 1.0f));
        m_pBasicModelEffect->Get().SetFloatParameter(FRAGMENT_SHADER, "material.shininess", 30.0f);
        
#ifdef SA_BUILD_DEBUG
        m_light[0].DebugRender();
        m_light[1].DebugRender();
#endif

        m_pTestModel->Get().Render();
        //m_pTestModel->Get().DebugDrawNormals();

        m_pBasicModelEffect->Get().Disable();
    }
#endif
}

void TestAppGame::Render2D()
{
    SaGame::Render2D();

#ifndef MODEL_TEST
    //2D drawing test.
    SaVector2 pos1 = SaVector2(0.0f, 0.0f);
    SaVector2 pos2 = SaVector2(800.0f, 800.0f);
    SaDebugDraw::DrawLine2D(pos1, pos2, SaColour::BLACK);
    pos1 = SaVector2(800.0f, 0.0f);
    pos2 = SaVector2(0.0f, 800.0f);
    SaDebugDraw::DrawLine2D(pos1, pos2, SaColour::BLACK);

    pos1 = SaVector2(600.0f, 600.0f);
    pos2 = SaVector2(700.0f, 600.0f);
    SaVector2 pos3 = SaVector2(600.0f, 500.0f);
    SaVector2 pos4 = SaVector2(700.0f, 500.0f);
    SaDebugDraw::DrawSquare2D(pos1, pos2, pos3, pos4, SaColour::CYAN);

    //Sprite drawing test.
    if (m_pSpriteEffect)
    {
        m_testSprite.Render();

        for (uint32_t x = 0; x < BRICK_X_NUM; x++)
        {
            for (uint32_t y = 0; y < BRICK_Y_NUM; y++)
            {
                m_brickArray[x][y].Render();
            }
        }
    }
#else
#endif
}

#ifndef MODEL_TEST
void TestAppGame::RenderThread_CustomRenderInit(const SaRenderJob* pJob)
{
    //glGenVertexArrays(1, &m_vao); //Not used in GLES.
    glGenBuffers(3, m_vbo);
    glGenBuffers(2, m_cubeVBO);

    //glBindVertexArray(m_vao); //Not used in GLES.

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), triVerts, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), triColors, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), triTexCoords, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_cubeVBO[0]);
    glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), cubeVerts, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_cubeVBO[1]);
    glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), cubeColours, GL_STATIC_DRAW);

    //Unbind buffer.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void TestAppGame::RenderThread_CustomRender(const SaRenderJob* pJob)
{

}

void TestAppGame::RenderThread_CustomRenderTri(const SaRenderJob* pJob)
{
    //glBindVertexArray(m_vao); //Not used in GLES.

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
    glVertexAttribPointer(SaEffect::POSITION_ATTR, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(SaEffect::POSITION_ATTR);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo[1]);
    glVertexAttribPointer(SaEffect::COLOUR_ATTR, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(SaEffect::COLOUR_ATTR);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo[2]);
    glVertexAttribPointer(SaEffect::TEXCOORD0_ATTR, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(SaEffect::TEXCOORD0_ATTR);

    //glDrawArrays(GL_TRIANGLES, 0, 3);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, triIndices);

    glDisableVertexAttribArray(SaEffect::TEXCOORD0_ATTR);
    glDisableVertexAttribArray(SaEffect::COLOUR_ATTR);
    glDisableVertexAttribArray(SaEffect::POSITION_ATTR);
}

void TestAppGame::RenderThread_CustomRenderCube(const SaRenderJob* pJob)
{
    //glBindVertexArray(m_vao); //Not used in GLES.

    glBindBuffer(GL_ARRAY_BUFFER, m_cubeVBO[0]);
    glVertexAttribPointer(SaEffect::POSITION_ATTR, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(SaEffect::POSITION_ATTR);

    glBindBuffer(GL_ARRAY_BUFFER, m_cubeVBO[1]);
    glVertexAttribPointer(SaEffect::COLOUR_ATTR, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(SaEffect::COLOUR_ATTR);

    //glDrawArrays(GL_TRIANGLES, 0, 36);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, cubeIndices);

    glDisableVertexAttribArray(SaEffect::COLOUR_ATTR);
    glDisableVertexAttribArray(SaEffect::POSITION_ATTR);
}
#endif
