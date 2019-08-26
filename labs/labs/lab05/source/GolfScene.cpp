#include "GolfScene.hpp"

#include <atlas/utils/GUI.hpp>
#include <atlas/gl/GL.hpp>
#include <atlas/core/Log.hpp>

namespace lab2
{
    GolfScene::GolfScene() :
        mPlay(false),
        mFPSOption(0),
        mFPS(60.0f),
        mCounter(mFPS)
    { }

    void GolfScene::updateScene(double time)
    {
        using atlas::core::Time;
        
        ModellingScene::updateScene(time);
        if (mPlay && mCounter.isFPS(mTime))
        {
            const float delta = 1.0f / mFPS;
            mAnimTime.currentTime += delta;
            mAnimTime.deltaTime = delta;
            mAnimTime.totalTime = mAnimTime.currentTime;

            mBall.updateGeometry(mAnimTime);
        }
    }


    void GolfScene::renderScene()
    {
        using atlas::utils::Gui;
        
        Gui::getInstance().newFrame();
        const float grey = 92.0f / 255.0f;
        const float black = 0.0f;
        glClearColor(grey, grey, grey, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        mProjection = glm::perspective(
            glm::radians(mCamera.getCameraFOV()),
            (float)mWidth / mHeight, 1.0f, 1000000000.0f);

        //mView = mCamera.getCameraMatrix();

        mView = glm::lookAt(
                glm::vec3(1,70,0),
                glm::vec3(0,0,0),
                glm::vec3(0,1,0)
        );


        mGrid.renderGeometry(mProjection, mView);
        mBall.renderGeometry(mProjection, mView);


        // Global HUD
        ImGui::SetNextWindowSize(ImVec2(50, 60), ImGuiSetCond_FirstUseEver);
        ImGui::Begin("Control");
        if (mPlay)
        {
            if (ImGui::Button("Pause"))
            {
                mPlay = !mPlay;
            }
        }
        else
        {
            if (ImGui::Button("Play"))
            {
                mPlay = !mPlay;
            }
        }

        if (ImGui::Button("Reset"))
        {
            mBall.resetGeometry();
            mAnimTime.currentTime = 0.0f;
            mAnimTime.totalTime = 0.0f;
            mPlay = false;
        }
        

        ImGui::End();

        mBall.drawGui();
        ImGui::Render();
    }
}