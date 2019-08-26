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
        mControlBuffer(GL_ARRAY_BUFFER),
        mIndexBuffer(GL_ELEMENT_ARRAY_BUFFER),
        mStar1Position(0, 0, 0),
        mStar2Position(-5, 0, 5),
        g(9.8f),
        theta1(1.0f),
        dtheta1(0.0f),
        d2theta1(0.0f),
        theta2(0.7f),
        dtheta2(0.0f),
        d2theta2(0.0f),
        l1(10.0f),
        l2(15.0f),
        m1(1.0f),
        m2(1.0f),
        k(1.0f),
        x(10.0f),
        dx(0.0f),
        d2x(0.0f)


    {
        using atlas::utils::Mesh;
        namespace gl = atlas::gl;
        namespace math = atlas::math;

        Mesh sphere;
        std::string path{ DataDirectory };
        path = path + "sphere.obj";
        Mesh::fromFile(path, sphere);

        mControlPoints = std::vector<atlas::math::Point>
                {
                        mStar1Position,
                        mStar2Position
                };

        mSpringPoints = std::vector<atlas::math::Point>
                {
                        {0,0,0},
                        mStar1Position
                };


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

        float mu = 1.0 + m1/m2;

            d2theta1 = 0;
            //d2theta2 = ( k * x - g * theta2 * (m1 + m2))/m1;
            d2theta2 = (k / m2 * x - mu * g * sin(theta2) - l2 * sin(theta2) * cos(theta2))
                       / (mu - cos(theta2) * cos(theta2));

            //d2x = (m2 * g * theta2 - k * x)/m1;
            d2x = (l2 * sin(theta2) * dtheta2 * d2theta2 + g * sin(theta2) * cos(theta2) - k / m2 * x)
                  / (l2 * (mu - cos(theta2) * cos(theta2)));

            dtheta1 += t.deltaTime * d2theta1;
            theta1 += t.deltaTime * dtheta1;

            dtheta2 += t.deltaTime * d2theta2;
            theta2 += t.deltaTime * dtheta2;

            if(x > 15.0) {
                dx = 0;
                x = 14.999;
            }
            else if(x < -15.0)
            {
                dx = 0;
                x = -14.999;
            }
            else
                dx += t.deltaTime * d2x;

            x += t.deltaTime * dx;

            mStar1Position.z = x;
            mStar1Position.x = 0;

            mStar2Position.z = sin(theta2) * l2 + x;
            mStar2Position.x = cos(theta2) * l2;

            //set the vertex array for pedulum points
            mControlPoints.pop_back();
            mControlPoints.pop_back();
            mControlPoints.push_back(mStar1Position);
            mControlPoints.push_back(mStar2Position);

            //set the vertex array for spring
            mSpringPoints.pop_back();
            mSpringPoints.push_back(mStar1Position);


    }



    void GolfBall::drawGui()
    {
        ImGui::SetNextWindowSize(ImVec2(300, 140), ImGuiSetCond_FirstUseEver);
        ImGui::Begin("Configurations");

       // std::vector<const char*> integratorNames = { "Euler Intergrator",
       // "Implicit Euler Integrator", "Verlet Integrator" };
       // ImGui::Combo("Integrator", &mIntegrator, integratorNames.data(),
       //     ((int)integratorNames.size()));
        ImGui::InputFloat("Set m1", &m1, 1.0f, 5.0f, 1);
        ImGui::InputFloat("Set m2", &m2, 1.0f, 5.0f, 1);
        ImGui::InputFloat("Set k", &k, 0.1f, 5.0f, 1);
        ImGui::InputFloat("Set pendulum length", &l2, 1.0f, 5.0f, 1);
        ImGui::InputFloat("Set angle", &theta2, 1.0f, 5.0f, 1);



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
        glUniform3f(mUniforms["colour"], 1, 0, 0);

        //render two balls
        {
            const math::Vector white{1.0f, 1.0f, 1.0f};
            auto model = glm::translate(math::Matrix4(1.0f), mStar1Position) * glm::scale(math::Matrix4(1.0f), math::Vector(0.5f));
            glUniformMatrix4fv(mUniforms["model"], 1, GL_FALSE, &model[0][0]);
            glUniform3fv(mUniforms["materialColour"], 1, &white[0]);
            glDrawElements(GL_TRIANGLES, mIndexCount, GL_UNSIGNED_INT, 0);
        }
        {
            const math::Vector red{1.0f, 0.0f, 0.0f};
            auto model = glm::translate(math::Matrix4(1.0f), mStar2Position) * glm::scale(math::Matrix4(1.0f), math::Vector(0.5f));
            glUniformMatrix4fv(mUniforms["model"], 1, GL_FALSE, &model[0][0]);
            glUniform3fv(mUniforms["materialColour"], 1, &red[0]);
            glDrawElements(GL_TRIANGLES, mIndexCount, GL_UNSIGNED_INT, 0);

        }
        mIndexBuffer.unBindBuffer();
        mVao.unBindVertexArray();

        //render the pendulum string
        mControlVao.bindVertexArray();
        mControlBuffer.bindBuffer();
        mControlBuffer.bufferData(atlas::gl::size<atlas::math::Point>(
                mControlPoints.size()), mControlPoints.data(), GL_STATIC_DRAW);
        mControlBuffer.vertexAttribPointer(
                VERTICES_LAYOUT_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, atlas::gl::bufferOffset<float>(0));
        mControlVao.enableVertexAttribArray(VERTICES_LAYOUT_LOCATION);

        mControlBuffer.unBindBuffer();
        //mControlVao.unBindVertexArray();
        //mControlVao.bindVertexArray();
        {
            auto model = glm::translate(math::Matrix4(1.0f), {0,0,0});
            glUniformMatrix4fv(mUniforms["model"], 1, GL_FALSE, &model[0][0]);

            glPointSize(5.0f);
            //glDrawArrays(GL_LINE_STRIP, 0, 2);
            glPointSize(1.0f);

            glDrawArrays(GL_LINE_STRIP, 0, 2);
        }

        mControlVao.unBindVertexArray();


        //render the spring
        mSpringVao.bindVertexArray();
        mSpringBuffer.bindBuffer();
        mSpringBuffer.bufferData(
                atlas::gl::size<atlas::math::Point>(mSpringPoints.size()), mSpringPoints.data(), GL_STATIC_DRAW);
        mSpringBuffer.vertexAttribPointer(
                VERTICES_LAYOUT_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, atlas::gl::bufferOffset<float>(0));
        mSpringVao.enableVertexAttribArray(VERTICES_LAYOUT_LOCATION);
        mSpringBuffer.unBindBuffer();

        glUniform3f(mUniforms["colour"], 0, 1, 0);

        {
            auto model = glm::translate(math::Matrix4(1.0f), {0,0,0});
            glUniformMatrix4fv(mUniforms["model"], 1, GL_FALSE, &model[0][0]);
            glDrawArrays(GL_LINE_STRIP, 0, 2);

        }
        mSpringVao.unBindVertexArray();



        mShaders[0].disableShaders();
    }

    void GolfBall::resetGeometry()
    {
        mStar1Position = {0, 0, -10};
        mStar2Position = {0, 0, -20};
        x = 0.0f;
        dx = 0.0f;
        d2x = 0.0f;
        dtheta2 = 1.0f;
        d2theta1 = 0.0f;
        d2theta2 = 0.0f;


    }



}
