#include "GolfBall.hpp"
#include "Paths.hpp"
#include "LayoutLocations.glsl"

#include <atlas/utils/Mesh.hpp>
#include <atlas/core/STB.hpp>
#include <atlas/core/Float.hpp>
#include <atlas/utils/GUI.hpp>
#include <GolfScene.hpp>

namespace lab2
{
    GolfBall::GolfBall() :
        mVertexBuffer(GL_ARRAY_BUFFER),
        mIndexBuffer(GL_ELEMENT_ARRAY_BUFFER),
        mStar1Position(10, 0, 0),
        mStar2Position(-10, 0, 0),
        mApproxPosition(-20, 10, 20),
        mStar1Velocity(0, 0, -10),
        mStar2Velocity(0, 0, 10),
        mApproxVelocity(60,0,0),
        mForce(0, 0, -10),
        mForce1(0,0,0),
        mForce2(0,0,0),
        mPlanetMass(5.0f),
        mStar1Mass(10.0f),
        mStar2Mass(10.0f),
        mIntegrator(0),
        mForceDirection1(0,0,0),
        mForceDirection2(0,0,0),
        mStarForceDirection(0,0,0),
        mStarForce(0,0,0),
        G(50.0f)


    {
        using atlas::utils::Mesh;
        namespace gl = atlas::gl;
        namespace math = atlas::math;

        Mesh sphere;
        std::string path{ DataDirectory };
        path = path + "sphere.obj";
        Mesh::fromFile(path, sphere);

        mIndexCount = static_cast<GLsizei>(sphere.indices().size());

        std::vector<float> data;
        for (std::size_t i = 0; i < sphere.vertices().size(); ++i)
        {
            data.push_back(sphere.vertices()[i].x);
            data.push_back(sphere.vertices()[i].y);
            data.push_back(sphere.vertices()[i].z);

            data.push_back(sphere.normals()[i].x);
            data.push_back(sphere.normals()[i].y);
            data.push_back(sphere.normals()[i].z);

            data.push_back(sphere.texCoords()[i].x);
            data.push_back(sphere.texCoords()[i].y);
        }

        mVao.bindVertexArray();
        mVertexBuffer.bindBuffer();
        mVertexBuffer.bufferData(gl::size<float>(data.size()), data.data(),
            GL_STATIC_DRAW);
        mVertexBuffer.vertexAttribPointer(VERTICES_LAYOUT_LOCATION, 3, GL_FLOAT,
            GL_FALSE, gl::stride<float>(8), gl::bufferOffset<float>(0));
        mVertexBuffer.vertexAttribPointer(NORMALS_LAYOUT_LOCATION, 3, GL_FLOAT,
            GL_FALSE, gl::stride<float>(8), gl::bufferOffset<float>(3));
        mVertexBuffer.vertexAttribPointer(TEXTURES_LAYOUT_LOCATION, 2, GL_FLOAT,
            GL_FALSE, gl::stride<float>(8), gl::bufferOffset<float>(6));

        mVao.enableVertexAttribArray(VERTICES_LAYOUT_LOCATION);
        mVao.enableVertexAttribArray(NORMALS_LAYOUT_LOCATION);
        mVao.enableVertexAttribArray(TEXTURES_LAYOUT_LOCATION);

        mIndexBuffer.bindBuffer();
        mIndexBuffer.bufferData(gl::size<GLuint>(sphere.indices().size()),
            sphere.indices().data(), GL_STATIC_DRAW);

        mIndexBuffer.unBindBuffer();
        mVertexBuffer.unBindBuffer();
        mVao.unBindVertexArray();

        std::vector<gl::ShaderUnit> shaders
        {
            {std::string(ShaderDirectory) + "Golf.vs.glsl", GL_VERTEX_SHADER},
            {std::string(ShaderDirectory) + "Golf.fs.glsl", GL_FRAGMENT_SHADER}
        };

        mShaders.emplace_back(shaders);
        mShaders[0].setShaderIncludeDir(ShaderDirectory);
        mShaders[0].compileShaders();
        mShaders[0].linkShaders();

        auto var = mShaders[0].getUniformVariable("model");
        mUniforms.insert(UniformKey("model", var));
        var = mShaders[0].getUniformVariable("projection");
        mUniforms.insert(UniformKey("projection", var));
        var = mShaders[0].getUniformVariable("view");
        mUniforms.insert(UniformKey("view", var));
        var = mShaders[0].getUniformVariable("materialColour");
        mUniforms.insert(UniformKey("materialColour", var));

        mShaders[0].disableShaders();
        mModel = glm::scale(math::Matrix4(1.0f), math::Vector(1.0f));
    }

    void GolfBall::updateGeometry(atlas::core::Time<> const& t)
    {
        orbitingPlanet(t);

        //GolfScene::camView = mApproxPosition;

        binaryStarMovement(t);

        float timeSq = t.currentTime * t.currentTime;
        //mTruePosition = mOffset + 0.5f * (mForce / mMass) * timeSq;
    }



    void GolfBall::drawGui()
    {
        ImGui::SetNextWindowSize(ImVec2(300, 140), ImGuiSetCond_FirstUseEver);
        ImGui::Begin("Configurations");

       // std::vector<const char*> integratorNames = { "Euler Intergrator",
       // "Implicit Euler Integrator", "Verlet Integrator" };
       // ImGui::Combo("Integrator", &mIntegrator, integratorNames.data(),
       //     ((int)integratorNames.size()));
        ImGui::InputFloat("Set Tatooine mass", &mPlanetMass, 1.0f, 5.0f, 1);
        ImGui::InputFloat("Set WHITE star mass", &mStar1Mass, 1.0f, 5.0f, 1);
        ImGui::InputFloat("Set RED star mass", &mStar2Mass, 1.0f, 5.0f, 1);
        ImGui::InputFloat("Set Tatooine speed", &mApproxVelocity.x, 1.0f, 5.0f, 1);
        ImGui::InputFloat("Set WHITE star speed", &mStar1Velocity.z, 1.0f, 5.0f, 1);
        ImGui::InputFloat("Set RED star speed", &mStar2Velocity.z, 1.0f, 5.0f, 1);
        ImGui::InputFloat("Set constant G", &G , 1.0f, 5.0f, 1);


        ImGui::End();
    }
    
    void GolfBall::renderGeometry(atlas::math::Matrix4 const& projection,
        atlas::math::Matrix4 const& view)
    {
        namespace math = atlas::math;

        mShaders[0].hotReloadShaders();
        if (!mShaders[0].shaderProgramValid())
        {
            return;
        }

        mShaders[0].enableShaders();

        mVao.bindVertexArray();
        mIndexBuffer.bindBuffer();

        glUniformMatrix4fv(mUniforms["projection"], 1, GL_FALSE,
            &projection[0][0]);
        glUniformMatrix4fv(mUniforms["view"], 1, GL_FALSE, &view[0][0]);


        {
            const math::Vector white{1.0f, 1.0f, 1.0f};
            auto model = glm::translate(math::Matrix4(1.0f), mStar1Position);
            glUniformMatrix4fv(mUniforms["model"], 1, GL_FALSE, &model[0][0]);
            glUniform3fv(mUniforms["materialColour"], 1, &white[0]);
            glDrawElements(GL_TRIANGLES, mIndexCount, GL_UNSIGNED_INT, 0);
        }


        {
            const math::Vector tatoonie{ 0.71f, 0.44f, 0.19f}; //{ 0.0f, 0.46f, 0.69f };
            auto model = glm::translate(math::Matrix4(1.0f), mApproxPosition) * glm::scale(math::Matrix4(1.0f), math::Vector(0.5f));
            glUniformMatrix4fv(mUniforms["model"], 1, GL_FALSE, &model[0][0]);
            glUniform3fv(mUniforms["materialColour"], 1, &tatoonie[0]);
            glDrawElements(GL_TRIANGLES, mIndexCount, GL_UNSIGNED_INT, 0);
        }

        {
            const math::Vector red{1.0f, 0.0f, 0.0f};
            auto model = glm::translate(math::Matrix4(1.0f), mStar2Position);
            glUniformMatrix4fv(mUniforms["model"], 1, GL_FALSE, &model[0][0]);
            glUniform3fv(mUniforms["materialColour"], 1, &red[0]);
            glDrawElements(GL_TRIANGLES, mIndexCount, GL_UNSIGNED_INT, 0);

        }

        mIndexBuffer.unBindBuffer();
        mVao.unBindVertexArray();
        mShaders[0].disableShaders();
    }

    void GolfBall::resetGeometry()
    {
        mStar1Position = {6, 0, 0};
        mStar2Position = {-6, 0, 0};
        mApproxPosition = {-20, 1, 20};
        mStar1Velocity = {0, 0, -10};
        mStar2Velocity = {0, 0, 10},
        mApproxVelocity = {50, 0, 0};

    }

    void GolfBall::orbitingPlanet(atlas::core::Time<> const& t)
    {
        // the force from star1 to planet
        mForceDirection1 = mStar1Position - mApproxPosition;
        float r1 = mForceDirection1.length();
        mForce1 = (G * mStar1Mass * mPlanetMass / (r1*r1)) * glm::normalize(mForceDirection1);

        // the force from star2 to planet
        mForceDirection2 = mStar2Position - mApproxPosition;
        float r2 = mForceDirection2.length();
        mForce2 = (G * mStar2Mass * mPlanetMass / (r2*r2)) * glm::normalize(mForceDirection2);

        mForce = mForce1 + mForce2;

        mApproxVelocity += t.deltaTime*mForce/mPlanetMass;

        mApproxPosition += t.deltaTime*mApproxVelocity;



        // TODO: Fill this in.
    }

    void GolfBall::binaryStarMovement(atlas::core::Time<> const &t)
    {
        float r = glm::distance(mStar1Position,mStar2Position);

        mStarForceDirection = mStar2Position - mStar1Position; //force direction from star1 to star2

        glm::normalize(mStarForceDirection);

        mStarForce = G*mStar1Mass*mStar2Mass/ (r*r) *mStarForceDirection;

        mStar1Velocity += t.deltaTime * mStarForce/mStar1Mass;
        mStar1Position += t.deltaTime * mStar1Velocity;

        mStar2Velocity += t.deltaTime * (-mStarForce)/mStar2Mass;
        mStar2Position += t.deltaTime * mStar2Velocity;




    }


}
