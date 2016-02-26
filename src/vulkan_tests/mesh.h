#pragma once

#include "vertex.h"

#include <vector>

class mesh
{
public:
    std::vector<vertex> vertices;
public:
    mesh(std::vector<vertex> vertices) :
        vertices(vertices)
    {
    }

    ~mesh() {};

    static mesh* createCube()
    {
        //auto vertices = new float[24];
        //auto indices = new float[36];

        //auto lbb = glm::vec3(-0.5f, -0.5f, -0.5f);
        //auto lbf = glm::vec3(-0.5f, -0.5f, 0.5f);
        //auto ltf = glm::vec3(-0.5f, 0.5f, 0.5f);
        //auto ltb = glm::vec3(-0.5f, 0.5f, -0.5f);
        //auto rbb = glm::vec3(0.5f, -0.5f, -0.5f);
        //auto rbf = glm::vec3(0.5f, -0.5f, 0.5f);
        //auto rtf = glm::vec3(0.5f, 0.5f, 0.5f);
        //auto rtb = glm::vec3(0.5f, 0.5f, -0.5f);

        //vertices[0] = vertex(ltb, glm::vec2(0.0f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f)); // 0
        //vertices[1] = vertex(lbb, glm::vec2(0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f)); // 1
        //vertices[2] = vertex(ltf, glm::vec2(1.0f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f)); // 2
        //vertices[3] = vertex(lbf, glm::vec2(1.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f)); // 3

        //vertices[4] = vertex(ltb, glm::vec2(0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // 4
        //vertices[5] = vertex(ltf, glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // 5
        //vertices[6] = vertex(rtb, glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // 6
        //vertices[7] = vertex(rtf, glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // 7

        //vertices[8] = vertex(rtf, glm::vec2(0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // 8
        //vertices[9] = vertex(rbf, glm::vec2(0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // 9
        //vertices[10] = vertex(rtb, glm::vec2(1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // 10
        //vertices[11] = vertex(rbb, glm::vec2(1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // 11

        //vertices[12] = vertex(lbf, glm::vec2(0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)); // 12
        //vertices[13] = vertex(lbb, glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)); // 13
        //vertices[14] = vertex(rbf, glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)); // 14
        //vertices[15] = vertex(rbb, glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)); // 15

        //vertices[16] = vertex(ltf, glm::vec2(0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // 16
        //vertices[17] = vertex(lbf, glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // 17
        //vertices[18] = vertex(rtf, glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // 18
        //vertices[19] = vertex(rbf, glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // 19

        //vertices[20] = vertex(rtb, glm::vec2(0.0f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f)); // 20
        //vertices[21] = vertex(rbb, glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)); // 21
        //vertices[22] = vertex(ltb, glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f)); // 22
        //vertices[23] = vertex(lbb, glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)); // 23

        //indices[0] = 0;
        //indices[1] = 1;
        //indices[2] = 2;
        //indices[3] = 3;
        //indices[4] = 2;
        //indices[5] = 1;
        //indices[6] = 4;
        //indices[7] = 5;
        //indices[8] = 6;
        //indices[9] = 7;
        //indices[10] = 6;
        //indices[11] = 5;
        //indices[12] = 8;
        //indices[13] = 9;
        //indices[14] = 10;
        //indices[15] = 11;
        //indices[16] = 10;
        //indices[17] = 9;
        //indices[18] = 12;
        //indices[19] = 13;
        //indices[20] = 14;
        //indices[21] = 15;
        //indices[22] = 14;
        //indices[23] = 13;
        //indices[24] = 16;
        //indices[25] = 17;
        //indices[26] = 18;
        //indices[27] = 19;
        //indices[28] = 18;
        //indices[29] = 17;
        //indices[30] = 20;
        //indices[31] = 21;
        //indices[32] = 22;
        //indices[33] = 23;
        //indices[34] = 22;
        //indices[35] = 21;

        auto lbb = glm::vec3(-0.5f, -0.5f, -0.5f);
        auto lbf = glm::vec3(-0.5f, -0.5f, 0.5f);
        auto ltf = glm::vec3(-0.5f, 0.5f, 0.5f);
        auto ltb = glm::vec3(-0.5f, 0.5f, -0.5f);
        auto rbb = glm::vec3(0.5f, -0.5f, -0.5f);
        auto rbf = glm::vec3(0.5f, -0.5f, 0.5f);
        auto rtf = glm::vec3(0.5f, 0.5f, 0.5f);
        auto rtb = glm::vec3(0.5f, 0.5f, -0.5f);

        auto rightNormal = glm::vec3(1.0f, 0.0f, 0.0f);
        auto leftNormal = glm::vec3(-1.0f, 0.0f, 0.0f);
        auto upNormal = glm::vec3(0.0f, 1.0f, 0.0f);
        auto bottomNormal = glm::vec3(0.0f, -1.0f, 0.0f);
        auto frontNormal = glm::vec3(0.0f, 0.0f, 1.0f);
        auto backNormal = glm::vec3(0.0f, 0.0f, -1.0f);

        auto vertices = std::vector<vertex>();

        ////             __________________________
        ////            /|ltb                  rtb/|
        ////           / |                       / |
        ////          /  |                      /  |
        ////         /   |                     /   |
        ////        /    |                    /    |
        ////       /     |                   /     |
        ////      /      |                  /      |
        ////     /       |                 /       |
        ////    /_________________________/        |
        ////    |ltf     |             rtf|        |
        ////    |        |________________|________|
        ////    |       /lbb              |     rbb/
        ////    |      /                  |       /
        ////    |     /                   |      /
        ////    |    /                    |     /
        ////    |   /                     |    /
        ////    |  /                      |   /
        ////    | /                       |  /
        ////    |/lbf                  rbf| /
        ////    |_________________________|/

        //front
        vertices.push_back(vertex(lbf, frontNormal));
        vertices.push_back(vertex(rbf, frontNormal));
        vertices.push_back(vertex(ltf, frontNormal));
        vertices.push_back(vertex(rbf, frontNormal));
        vertices.push_back(vertex(rtf, frontNormal));
        vertices.push_back(vertex(ltf, frontNormal));

        //right
        vertices.push_back(vertex(rbf, rightNormal));
        vertices.push_back(vertex(rbb, rightNormal));
        vertices.push_back(vertex(rtb, rightNormal));
        vertices.push_back(vertex(rtb, rightNormal));
        vertices.push_back(vertex(rtf, rightNormal));
        vertices.push_back(vertex(rbf, rightNormal));

        //left
        vertices.push_back(vertex(lbf, leftNormal));
        vertices.push_back(vertex(ltf, leftNormal));
        vertices.push_back(vertex(ltb, leftNormal));
        vertices.push_back(vertex(ltb, leftNormal));
        vertices.push_back(vertex(lbb, leftNormal));
        vertices.push_back(vertex(lbf, leftNormal));

        // back
        vertices.push_back(vertex(ltb, backNormal));
        vertices.push_back(vertex(rtb, backNormal));
        vertices.push_back(vertex(rbb, backNormal));
        vertices.push_back(vertex(rbb, backNormal));
        vertices.push_back(vertex(lbb, backNormal));
        vertices.push_back(vertex(ltb, backNormal));

        //bottom
        vertices.push_back(vertex(lbf, bottomNormal));
        vertices.push_back(vertex(lbb, bottomNormal));
        vertices.push_back(vertex(rbb, bottomNormal));
        vertices.push_back(vertex(rbb, bottomNormal));
        vertices.push_back(vertex(rbf, bottomNormal));
        vertices.push_back(vertex(lbf, bottomNormal));

        //up
        vertices.push_back(vertex(ltb, upNormal));
        vertices.push_back(vertex(ltf, upNormal));
        vertices.push_back(vertex(rtf, upNormal));
        vertices.push_back(vertex(rtf, upNormal));
        vertices.push_back(vertex(rtb, upNormal));
        vertices.push_back(vertex(ltb, upNormal));

        return new mesh(vertices);
    }
};