#ifndef LOADER_H
#define LOADER_H


#include <iostream>
#include <vector>

#include "Vector2f.h"


class Loader{

public:
    Loader() = delete;

    // return: 0 file not found 1 search found -1 search not found
    static int SearchInFile(const std::string& fileName, const std::string& search, bool drawError = true);
    // return: positve number if read succesfuly a positive number, 0 or negative if number is negative or some error occurred
    static int GetNumberVerticesFromFile(const std::string& fileName);
    // return: 1 if loaded succesfuly, 0 or negative if some error occurred
    static int LoadSegmentFromFile(std::vector<Vector2f>& segmentPoints, const std::string& fileName);
    // return: 1 if loaded succesfuly, 0 or negative if some error occurred
    static int LoadVerticesFromFile(std::vector<Vector2f>& vertices, std::vector<unsigned int>& indices,
                                    const std::string& fileName, unsigned int numberVertices, bool loadIndices = false);
    // saves polygon to file (relative path is from the exectuable)
    static void SavePolygonToFile(const std::vector<Vector2f>& vertices, const std::vector<unsigned int>& indices,
                                  const std::vector<Vector2f>& segmentPoints, const std::vector<std::vector<unsigned int>*>& polygonsIndices,
                                  const std::string& fileName);

    // return: positve number if read succesfuly a positive number, 0 or negative if number is negative or some error occurred
    static int GetNumberSmallPolygonsFromFile(const std::string& fileName);

    // return: positve number if read succesfuly a positive number, 0 or negative if number is negative or some error occurred
    static int LoadSmallPolygonsIndicesFromFile(std::vector<std::vector<unsigned int>*>& polygonsIndices,
                                               const std::string& fileName, unsigned int numberSmallPolygons);

private:
    // return: 0 file not found 1 search found -1 search not found
    static int OpenFileAndSearch(std::ifstream& file, const std::string fileName, const std::string& search, bool drawError = true);
};


#endif // LOADER_H
