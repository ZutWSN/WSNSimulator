#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <iterator>


struct Vertice
{
    uint16_t node_id;
    uint16_t previousNodeID;
    uint16_t sinkPathLength;
    std::vector<uint16_t> neighbourVerticesIndexes;
    std::vector<uint16_t> neighbourVerticesDistances;
    std::vector<uint16_t> sinkPath;
};

int main()
{
    uint16_t sinkID = 0;
    uint16_t sinkIndex = 0;
    Vertice sink{sinkID, sinkID, 0, {1, 2, 3}, {3, 4, 2}, {}};
    Vertice directCluster1{1, 1, UINT16_MAX, {sinkIndex, 4, 5, 7}, {3, 1, 2, 2}};
    Vertice directCluster2{2, 2, UINT16_MAX, {sinkIndex, 5, 6, 8}, {4, 6, 5, 9}};
    Vertice directCluster3{3, 3, UINT16_MAX, {sinkIndex, 7, 8}, {2, 4, 5}};
    Vertice cluster1{4, 4, UINT16_MAX, {1, 5, 6}, {1, 5, 3}};
    Vertice cluster2{5, 5, UINT16_MAX, {1, 2, 4, 6}, {2, 6, 5, 1}};
    Vertice cluster3{6, 6, UINT16_MAX, {2, 4, 5}, {5, 3, 1}};
    Vertice cluster4{7, 7, UINT16_MAX, {2, 3}, {9, 5}};
    Vertice cluster5{8, 8, UINT16_MAX, {1, 3}, {2, 4}};

    std::vector<Vertice> vertices{sink, directCluster1, directCluster2, directCluster3, cluster1, cluster2, cluster3, cluster4, cluster5};
    std::vector<uint16_t> unvisitedVertices;
    for(uint16_t i = 0; i < vertices.size(); i++)
    {
        unvisitedVertices.push_back(i);
    }
    while(!unvisitedVertices.empty())
    {
        uint16_t closestVertexIndex = 0;
        uint16_t unvisitedIndex = 0;
        uint16_t minDistance = UINT16_MAX;
        for(uint16_t i = 0; i < unvisitedVertices.size(); i++)
        {
            if(vertices[unvisitedVertices[i]].sinkPathLength < minDistance)
            {
                closestVertexIndex = unvisitedVertices[i];
                unvisitedIndex = i;
                minDistance = vertices[unvisitedVertices[i]].sinkPathLength;
            }
        }
        uint16_t initDistance = (minDistance != UINT16_MAX) ? minDistance : 0;
        uint16_t pathLength = UINT16_MAX;
        uint16_t nextVertexIndex = 0;
        for(uint16_t i = 0; i < vertices[closestVertexIndex].neighbourVerticesIndexes.size(); i++)
        {
            auto currentIndex = vertices[closestVertexIndex].neighbourVerticesIndexes[i];
            if(std::find(unvisitedVertices.begin(), unvisitedVertices.end(), currentIndex) != std::end(unvisitedVertices))
            {
                uint16_t distance = initDistance + vertices[closestVertexIndex].neighbourVerticesDistances[i];
                if(vertices[currentIndex].sinkPathLength > distance)
                {
                    vertices[currentIndex].sinkPathLength = distance;
                    vertices[currentIndex].sinkPath = {vertices[closestVertexIndex].sinkPath};
                    vertices[currentIndex].sinkPath.insert(vertices[currentIndex].sinkPath.begin(), vertices[closestVertexIndex].node_id);
                    if(distance < pathLength)
                    {
                        pathLength = distance;
                        nextVertexIndex = currentIndex;
                    }
                }
            }
        }
        unvisitedVertices.erase(unvisitedVertices.begin() + unvisitedIndex);
        for(uint16_t i = 0; i < vertices[nextVertexIndex].neighbourVerticesIndexes.size(); i++)
        {
            auto idx = vertices[nextVertexIndex].neighbourVerticesIndexes[i];
            if(std::find(unvisitedVertices.begin(), unvisitedVertices.end(), idx) != std::end(unvisitedVertices))
            {
                if(vertices[idx].sinkPathLength > (pathLength + vertices[nextVertexIndex].neighbourVerticesDistances[i]))
                {
                   vertices[idx].sinkPathLength = pathLength + vertices[nextVertexIndex].neighbourVerticesDistances[i];
                   vertices[idx].sinkPath = {vertices[nextVertexIndex].sinkPath};
                   vertices[idx].sinkPath.insert(vertices[idx].sinkPath.begin(), vertices[nextVertexIndex].node_id);
                }
            }
        }

    }

    //show results:
    for(auto && vertice : vertices)
    {
        std::cout << "\nVertice ID : " << vertice.node_id << "\nSink path:\n";
        for(auto && node : vertice.sinkPath)
        {
            std::cout << node << " ";
        }
        std::cout << "\n";
    }
    return 0;
}
