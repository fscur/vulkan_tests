#pragma once

#include "vertex.h"

#include <vector>

class mesh
{
public:
    std::vector<vertex> vertices;
    std::vector<unsigned int> indices;
    glm::mat4 matrix;
public:
    mesh(std::vector<vertex> vertices, std::vector<unsigned int> indices, glm::mat4 matrix) :
        vertices(vertices),
        indices(indices),
        matrix(matrix)
    {
    }

    mesh(std::vector<vertex> vertices, std::vector<unsigned int> indices) :
        mesh::mesh(vertices, indices, glm::mat4())
    {
    }

    ~mesh() {};

    static mesh* createCube()
    {
        std::vector<vertex> vertices;;
        std::vector<unsigned int> indices;;

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

        vertices.push_back(vertex(ltb, glm::vec2(0.0f, 1.0f), leftNormal)); // 0
        vertices.push_back(vertex(lbb, glm::vec2(0.0f, 0.0f), leftNormal)); // 1
        vertices.push_back(vertex(ltf, glm::vec2(1.0f, 1.0f), leftNormal)); // 2
        vertices.push_back(vertex(lbf, glm::vec2(1.0f, 0.0f), leftNormal)); // 3

        vertices.push_back(vertex(ltb, glm::vec2(0.0f, 1.0f), upNormal)); // 4
        vertices.push_back(vertex(ltf, glm::vec2(0.0f, 0.0f), upNormal)); // 5
        vertices.push_back(vertex(rtb, glm::vec2(1.0f, 1.0f), upNormal)); // 6
        vertices.push_back(vertex(rtf, glm::vec2(1.0f, 0.0f), upNormal)); // 7
                                                                          
        vertices.push_back(vertex(rtf, glm::vec2(0.0f, 1.0f), rightNormal)); // 8
        vertices.push_back(vertex(rbf, glm::vec2(0.0f, 0.0f), rightNormal)); // 9
        vertices.push_back(vertex(rtb, glm::vec2(1.0f, 1.0f), rightNormal)); // 10
        vertices.push_back(vertex(rbb, glm::vec2(1.0f, 0.0f), rightNormal)); // 11

        vertices.push_back(vertex(lbf, glm::vec2(0.0f, 1.0f), bottomNormal)); // 12
        vertices.push_back(vertex(lbb, glm::vec2(0.0f, 0.0f), bottomNormal)); // 13
        vertices.push_back(vertex(rbf, glm::vec2(1.0f, 1.0f), bottomNormal)); // 14
        vertices.push_back(vertex(rbb, glm::vec2(1.0f, 0.0f), bottomNormal)); // 15

        vertices.push_back(vertex(ltf, glm::vec2(0.0f, 1.0f), frontNormal)); // 16
        vertices.push_back(vertex(lbf, glm::vec2(0.0f, 0.0f), frontNormal)); // 17
        vertices.push_back(vertex(rtf, glm::vec2(1.0f, 1.0f), frontNormal)); // 18
        vertices.push_back(vertex(rbf, glm::vec2(1.0f, 0.0f), frontNormal)); // 19

        vertices.push_back(vertex(rtb, glm::vec2(0.0f, 1.0f), backNormal)); // 20
        vertices.push_back(vertex(rbb, glm::vec2(0.0f, 0.0f), backNormal)); // 21
        vertices.push_back(vertex(ltb, glm::vec2(1.0f, 1.0f), backNormal)); // 22
        vertices.push_back(vertex(lbb, glm::vec2(1.0f, 0.0f), backNormal)); // 23

        indices.push_back(0);
        indices.push_back(1);
        indices.push_back(2);
        indices.push_back(3);
        indices.push_back(2);
        indices.push_back(1);
        indices.push_back(4);
        indices.push_back(5);
        indices.push_back(6);
        indices.push_back(7);
        indices.push_back(6);
        indices.push_back(5);
        indices.push_back(8);
        indices.push_back(9);
        indices.push_back(10);
        indices.push_back(11);
        indices.push_back(10);
        indices.push_back(9);
        indices.push_back(12);
        indices.push_back(13);
        indices.push_back(14);
        indices.push_back(15);
        indices.push_back(14);
        indices.push_back(13);
        indices.push_back(16);
        indices.push_back(17);
        indices.push_back(18);
        indices.push_back(19);
        indices.push_back(18);
        indices.push_back(17);
        indices.push_back(20);
        indices.push_back(21);
        indices.push_back(22);
        indices.push_back(23);
        indices.push_back(22);
        indices.push_back(21);

        return new mesh(vertices, indices, glm::mat4(
                                                    1.0f, 0.0f, 0.0f, 0.0f,
                                                    0.0f, 1.0f, 0.0f, 0.0f,
                                                    0.0f, 0.0f, 1.0f, 0.0f,
                                                    0.0f, 0.0f, 0.0f, 1.0f));

        //auto lbb = glm::vec3(-0.5f, -0.5f, -0.5f);
        //auto lbf = glm::vec3(-0.5f, -0.5f, 0.5f);
        //auto ltf = glm::vec3(-0.5f, 0.5f, 0.5f);
        //auto ltb = glm::vec3(-0.5f, 0.5f, -0.5f);
        //auto rbb = glm::vec3(0.5f, -0.5f, -0.5f);
        //auto rbf = glm::vec3(0.5f, -0.5f, 0.5f);
        //auto rtf = glm::vec3(0.5f, 0.5f, 0.5f);
        //auto rtb = glm::vec3(0.5f, 0.5f, -0.5f);

        //auto rightNormal = glm::vec3(1.0f, 0.0f, 0.0f);
        //auto leftNormal = glm::vec3(-1.0f, 0.0f, 0.0f);
        //auto upNormal = glm::vec3(0.0f, 1.0f, 0.0f);
        //auto bottomNormal = glm::vec3(0.0f, -1.0f, 0.0f);
        //auto frontNormal = glm::vec3(0.0f, 0.0f, 1.0f);
        //auto backNormal = glm::vec3(0.0f, 0.0f, -1.0f);

        //auto vertices = std::vector<vertex>();

        //////             __________________________
        //////            /|ltb                  rtb/|
        //////           / |                       / |
        //////          /  |                      /  |
        //////         /   |                     /   |
        //////        /    |                    /    |
        //////       /     |                   /     |
        //////      /      |                  /      |
        //////     /       |                 /       |
        //////    /_________________________/        |
        //////    |ltf     |             rtf|        |
        //////    |        |________________|________|
        //////    |       /lbb              |     rbb/
        //////    |      /                  |       /
        //////    |     /                   |      /
        //////    |    /                    |     /
        //////    |   /                     |    /
        //////    |  /                      |   /
        //////    | /                       |  /
        //////    |/lbf                  rbf| /
        //////    |_________________________|/

        ////front
        //vertices.push_back(vertex(lbf, frontNormal));
        //vertices.push_back(vertex(rbf, frontNormal));
        //vertices.push_back(vertex(ltf, frontNormal));
        //vertices.push_back(vertex(rbf, frontNormal));
        //vertices.push_back(vertex(rtf, frontNormal));
        //vertices.push_back(vertex(ltf, frontNormal));

        ////right
        //vertices.push_back(vertex(rbf, rightNormal));
        //vertices.push_back(vertex(rbb, rightNormal));
        //vertices.push_back(vertex(rtb, rightNormal));
        //vertices.push_back(vertex(rtb, rightNormal));
        //vertices.push_back(vertex(rtf, rightNormal));
        //vertices.push_back(vertex(rbf, rightNormal));

        ////left
        //vertices.push_back(vertex(lbf, leftNormal));
        //vertices.push_back(vertex(ltf, leftNormal));
        //vertices.push_back(vertex(ltb, leftNormal));
        //vertices.push_back(vertex(ltb, leftNormal));
        //vertices.push_back(vertex(lbb, leftNormal));
        //vertices.push_back(vertex(lbf, leftNormal));

        //// back
        //vertices.push_back(vertex(ltb, backNormal));
        //vertices.push_back(vertex(rtb, backNormal));
        //vertices.push_back(vertex(rbb, backNormal));
        //vertices.push_back(vertex(rbb, backNormal));
        //vertices.push_back(vertex(lbb, backNormal));
        //vertices.push_back(vertex(ltb, backNormal));

        ////bottom
        //vertices.push_back(vertex(lbf, bottomNormal));
        //vertices.push_back(vertex(lbb, bottomNormal));
        //vertices.push_back(vertex(rbb, bottomNormal));
        //vertices.push_back(vertex(rbb, bottomNormal));
        //vertices.push_back(vertex(rbf, bottomNormal));
        //vertices.push_back(vertex(lbf, bottomNormal));

        ////up
        //vertices.push_back(vertex(ltb, upNormal));
        //vertices.push_back(vertex(ltf, upNormal));
        //vertices.push_back(vertex(rtf, upNormal));
        //vertices.push_back(vertex(rtf, upNormal));
        //vertices.push_back(vertex(rtb, upNormal));
        //vertices.push_back(vertex(ltb, upNormal));

        //return new mesh(vertices);
    }
};