#pragma once

#include <atlas/utils/Geometry.hpp>
#include <atlas/gl/Buffer.hpp>
#include <atlas/gl/VertexArrayObject.hpp>
#include <atlas/gl/Texture.hpp>

namespace lab2
{
    class GolfBall : public atlas::utils::Geometry
    {
    public:
        GolfBall();

        void updateGeometry(atlas::core::Time<> const& t) override;
        void drawGui() override;
        void renderGeometry(atlas::math::Matrix4 const& projection,
            atlas::math::Matrix4 const& view) override;

        void resetGeometry() override;


    private:



        atlas::gl::Buffer mVertexBuffer;
        atlas::gl::Buffer mIndexBuffer;
        atlas::gl::VertexArrayObject mVao;
        atlas::gl::VertexArrayObject mControlVao;
        atlas::gl::VertexArrayObject mSpringVao;
        atlas::gl::Buffer mControlBuffer;
        atlas::gl::Buffer mSpringBuffer;

        atlas::math::Vector mStar1Position;
        atlas::math::Vector mStar2Position;
        std::vector<atlas::math::Point> mControlPoints;
        std::vector<atlas::math::Point> mSpringPoints;





        float theta1, dtheta1, d2theta1, theta2, dtheta2, d2theta2, l1, l2, m1, m2, g, k;
        float x, dx, d2x;



        GLsizei mIndexCount;
    };
}