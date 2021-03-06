#include "mesh/Edge.hpp"
#include "mesh/Triangle.hpp"
#include "mesh/Vertex.hpp"
#include "testing/Testing.hpp"

using namespace precice::mesh;

BOOST_AUTO_TEST_SUITE(MeshTests)

BOOST_AUTO_TEST_CASE(Triangles)
{
  using Eigen::Vector3d;
  Vector3d coords1(0.0, 0.0, 0.0);
  Vector3d coords2(1.0, 0.0, 0.0);
  Vector3d coords3(1.0, 1.0, 0.0);
  {
    Vertex v1(coords1, 0);
    Vertex v2(coords2, 1);
    Vertex v3(coords3, 2);

    Edge e1(v1, v2, 0);
    Edge e2(v3, v2, 1);
    Edge e3(v3, v1, 2);

    Triangle triangle(e1, e2, e3, 0);

    Vertex &v1ref = triangle.vertex(0);
    BOOST_TEST(v1ref.getID() == v1.getID());

    Vertex &v2ref = triangle.vertex(1);
    BOOST_TEST(v2ref.getID() == v2.getID());

    Vertex &v3ref = triangle.vertex(2);
    BOOST_TEST(v3ref.getID() == v3.getID());

    Edge &e1ref = triangle.edge(0);
    BOOST_TEST(e1ref.getID() == e1.getID());

    Edge &e2ref = triangle.edge(1);
    BOOST_TEST(e2ref.getID() == e2.getID());

    Edge &e3ref = triangle.edge(2);
    BOOST_TEST(e3ref.getID() == e3.getID());

    int id = triangle.getID();
    BOOST_TEST(id == 0);
  }
  {
    Vertex v1(coords1, 0);
    Vertex v2(coords2, 1);
    Vertex v3(coords3, 2);

    Edge e1(v1, v2, 0);
    Edge e2(v3, v2, 1);
    Edge e3(v1, v3, 2);

    Triangle triangle(e1, e2, e3, 0);

    Vertex &v1ref = triangle.vertex(0);
    BOOST_TEST(v1ref.getID() == v1.getID());

    Vertex &v2ref = triangle.vertex(1);
    BOOST_TEST(v2ref.getID() == v2.getID());

    Vertex &v3ref = triangle.vertex(2);
    BOOST_TEST(v3ref.getID() == v3.getID());

    Edge &e1ref = triangle.edge(0);
    BOOST_TEST(e1ref.getID() == e1.getID());

    Edge &e2ref = triangle.edge(1);
    BOOST_TEST(e2ref.getID() == e2.getID());

    Edge &e3ref = triangle.edge(2);
    BOOST_TEST(e3ref.getID() == e3.getID());

    int id = triangle.getID();
    BOOST_TEST(id == 0);
  }
  {
    Vertex v1(coords1, 0);
    Vertex v2(coords2, 1);
    Vertex v3(coords3, 2);

    Edge e1(v1, v2, 0);
    Edge e2(v3, v2, 1);
    Edge e3(v3, v1, 2);

    Triangle triangle(e1, e3, e2, 0);

    Vertex &v1ref = triangle.vertex(0);
    BOOST_TEST(v1ref.getID() == v2.getID());

    Vertex &v2ref = triangle.vertex(1);
    BOOST_TEST(v2ref.getID() == v1.getID());

    Vertex &v3ref = triangle.vertex(2);
    BOOST_TEST(v3ref.getID() == v3.getID());

    Edge &e1ref = triangle.edge(0);
    BOOST_TEST(e1ref.getID() == e1.getID());

    Edge &e2ref = triangle.edge(1);
    BOOST_TEST(e2ref.getID() == e3.getID());

    Edge &e3ref = triangle.edge(2);
    BOOST_TEST(e3ref.getID() == e2.getID());

    int id = triangle.getID();
    BOOST_TEST(id == 0);
  }
}

BOOST_AUTO_TEST_SUITE_END() // Mesh
